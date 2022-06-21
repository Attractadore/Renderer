#!/usr/bin/env python3
import xml.etree.ElementTree as ET
from sys import argv
from pathlib import Path

def parse_commands(root):
    funcs = []
    for child in root.find("commands"):
        name = child.attrib.get("name", None)
        if not name:
            name = child[0][1].text
        funcs.append(name)
    return funcs

def parse_core_version(node, out: dict):
    core_version = node.attrib["name"]
    for req in node:
        for field in req:
            if field.tag == "command":
                cmd_name = field.attrib["name"]
                if cmd_name not in out:
                    out[cmd_name] = []
                out[cmd_name].append((core_version,))

def parse_core(root, out: dict):
    for child in root:
        if child.tag == "feature":
            parse_core_version(child, out)

def parse_extension(node, out: dict):
    # Skip platform specifics since this would mess with ABI
    if "platform" in node.attrib:
        return
    extension_name = node.attrib.get("name", None)
    if not extension_name:
        return
    for req in node:
        cmd_requires = [extension_name]
        if feature := req.attrib.get("feature", None):
            cmd_requires.append(feature)
        if extension := req.attrib.get("extension", None):
            cmd_requires.append(extension)
        cmd_requires = tuple(cmd_requires)
        for field in req:
            if field.tag == "command":
                cmd_name = field.attrib["name"]
                if cmd_name not in out:
                    out[cmd_name] = []
                out[cmd_name].append(cmd_requires)

def parse_extensions(root, out: dict):
    for extension in root.find("extensions"):
        parse_extension(extension, out)

def parse_all(root):
    funcs = {}
    parse_core(root, funcs)
    parse_extensions(root, funcs)
    return funcs

def parse_device_commands(root):
    aliases = {}
    device_commands = set()
    for command in root.find("commands"):
        if alias_target := command.attrib.get("alias", None):
            cmd_name = command.attrib["name"]
            aliases[cmd_name] = alias_target
            continue
        proto = command[0]
        cmd_type = command[1][0].text
        if cmd_type not in ("VkDevice", "VkQueue", "VkCommandBuffer"):
            continue
        cmd_name = proto[1].text
        if cmd_name == "vkGetDeviceProcAddr":
            continue
        device_commands.add(cmd_name)
    for alias, target in aliases.items():
        if target in device_commands:
            device_commands.add(alias)
    return device_commands

def format_cond(g):
    return " || ".join(" && ".join(feature for feature in conj) for conj in g)

def format_func_header(f, g):
    cond = format_cond(g)
    return (
    "#if {0}\n"
    "   PFN_{1} {2};\n"
    "#endif // {0}".format(cond, f, f[2:]))

def format_funcs_header(funcs):
    return "\n".join(format_func_header(f, g) for f, g in funcs.items())

def generate_header(funcs):
    return (
    "#pragma once\n"
    "#include <vulkan/vulkan.h>\n"
    "\n"
    "#if __cplusplus\n"
    "extern \"C\" {{\n"
    "#endif\n"
    "\n"
    "typedef struct {{\n"
    "{0}\n"
    "}} VulkanDeviceDispatchTable;\n"
    "\n"
    "void LoadVulkanDeviceDispatchTable(VulkanDeviceDispatchTable* table, VkDevice device);\n"
    "#if __cplusplus\n"
    "}}\n"
    "#endif\n"
    ).format(
        format_funcs_header(funcs)
    )

def format_func_source(f, g, table_name, context_name, load_name):
    cond = format_cond(g)
    return (
    "#if {cond}\n"
    "   {table}->{name} = (PFN_{vk_name}) {load}({context}, \"{vk_name}\");\n"
    "#endif // {cond}".format(
        cond=cond,
        table=table_name,
        context=context_name,
        load=load_name,
        vk_name=f,
        name=f[2:]))

def format_funcs_source(funcs, table_name, context_name, load_name):
    return "\n".join(format_func_source(f, g, table_name, context_name, load_name) for f, g in funcs.items())

def generate_source(funcs, header_name):
    table_name = "table"
    context_name = "context"
    load_name = "load"
    return (
    "#include \"{header}\"\n"
    "typedef void* (*load_func_t)(void* context, const char* func_name);\n"
    "\n"
    "static void LoadVulkanDeviceDispatchTableImpl("
    "VulkanDeviceDispatchTable* {table}, "
    "void* {context}, "
    "load_func_t {load}) {{\n"
    "{0}\n"
    "}}\n"
    "\n"
    "void LoadVulkanDeviceDispatchTable(VulkanDeviceDispatchTable* table, VkDevice device) {{\n"
    "   LoadVulkanDeviceDispatchTableImpl(table, device, (load_func_t) vkGetDeviceProcAddr);\n"
    "}}\n"
    ).format(
        format_funcs_source(funcs, table_name, context_name, load_name),
        header=header_name,
        table=table_name,
        context=context_name,
        load=load_name)

def main():
    vk_xml = argv[1]
    header_out = Path(argv[2])
    source_out = Path(argv[3])

    tree = None
    with open(vk_xml, "rb") as f:
        tree = ET.parse(vk_xml)
    root = tree.getroot()

    funcs = parse_all(root)
    device_funcs = parse_device_commands(root)
    funcs = dict(
        filter(lambda kv: kv[0] in device_funcs, funcs.items()))

    header_name = header_out.name
    h = generate_header(funcs)
    s = generate_source(funcs, header_name)

    header_out.parent.mkdir(exist_ok=True, parents=True)
    header_out.write_text(h)
    source_out.parent.mkdir(exist_ok=True, parents=True)
    source_out.write_text(s)

if __name__ == "__main__":
    main()
