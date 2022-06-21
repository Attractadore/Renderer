#!/usr/bin/env python3
import xml.etree.ElementTree as ET
from sys import argv
from pathlib import Path

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

def parse_command_param(param):
    tp = param.find("type")
    param_type = " ".join("{const}{tp}{ptr}".format(
        const=param.text if param.text else "",
        tp=tp.text,
        ptr=tp.tail if tp.tail else "").split())
    param_name = param.find("name").text.strip()
    return (param_type, param_name)

def parse_command_params(params):
    return tuple(parse_command_param(param) for param in params)

def parse_device_commands(root):
    aliases = {}
    device_commands = {}
    for command in root.find("commands"):
        if alias_target := command.attrib.get("alias", None):
            cmd_name = command.attrib["name"]
            aliases[cmd_name] = alias_target
            continue
        proto = command[0]
        return_type = proto.find("type").text
        params = command.findall("param")
        cmd_type = params[0].find("type").text
        if cmd_type not in ("VkDevice", "VkQueue", "VkCommandBuffer"):
            continue
        cmd_name = proto[1].text
        if cmd_name == "vkGetDeviceProcAddr":
            continue
        device_commands[cmd_name] = (return_type, parse_command_params(params))
    for alias, target in aliases.items():
        if target in device_commands:
            # TODO: this is OK if vk.xml is the same version as vulkan.h,
            # but if vk.xml is newer, than this in result in aliases using
            # using undefined types
            device_commands[alias] = device_commands[target]
    return device_commands

def format_cond(g):
    return " || ".join(" && ".join(feature for feature in conj) for conj in g)

def format_params(params):
    return ", ".join(f"{t} {n}" for t, n in params)

def format_args(args, impl_name):
    return ", ".join(f"{arg}".format(impl=impl_name) for arg in args)

def format_func_header(f):
    vk_name, ((retval, params), g) = f
    name = vk_name[2:]
    impl_name = "impl"
    takes_device = params[0][0] == "VkDevice"
    param_str = format_params(params[1:] if takes_device else params)
    args_str = None
    if takes_device:
        args_str = format_args(("{impl}->GetDevice()",) + tuple(n for t, n in params[1:]), impl_name)
    else:
        args_str = format_args((n for t, n in params), impl_name)
    return (
    "#if {cond}\n"
    "    {retval} {name} ({params}) noexcept {{\n"
    "        auto {impl} = static_cast<Derived*>(this);\n"
    "        return {impl}->GetDispatchTable().{name}({args});\n"
    "    }}\n"
    "#endif // {cond}"
    ).format(
        cond=format_cond(g),
        name=name,
        retval=retval,
        params=param_str,
        impl=impl_name,
        args=args_str
    )

def format_funcs_header(funcs):
    return "\n".join(format_func_header(f) for f in funcs)

def generate_header(funcs):
    return (
    "#pragma once\n"
    "#include <vulkan/vulkan.h>\n"
    "\n"
    "template<typename Derived>\n"
    "struct VulkanContextDispatcher {{\n"
    "{0}\n"
    "}};\n"
    ).format(
        format_funcs_header(funcs)
    )

def main():
    vk_xml = argv[1]
    header_out = Path(argv[2])

    tree = None
    with open(vk_xml, "rb") as f:
        tree = ET.parse(vk_xml)
    root = tree.getroot()

    funcs = parse_all(root)
    device_funcs = parse_device_commands(root)
    device_funcs = { k: (v, funcs[k]) for k, v in device_funcs.items() if k in funcs}

    header_name = header_out.name
    h = generate_header(device_funcs.items())

    header_out.parent.mkdir(exist_ok=True, parents=True)
    header_out.write_text(h)

if __name__ == "__main__":
    main()
