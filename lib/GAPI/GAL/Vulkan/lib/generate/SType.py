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

def parse_require(req, base_feature, out: dict):
    type_requires = [base_feature]
    if feature := req.attrib.get("feature", None):
        type_requires.append(feature)
    if extension := req.attrib.get("extension", None):
        type_requires.append(extension)
    type_requires = tuple(type_requires)
    for field in req:
        if field.tag == "type":
            type_name = field.attrib["name"]
            if type_name not in out:
                out[type_name] = []
            out[type_name].append(type_requires)

def parse_core_version(node, out: dict):
    core_version = node.attrib["name"]
    for req in node:
        parse_require(req, core_version, out)

def parse_core(root, out: dict):
    for child in root:
        if child.tag == "feature":
            parse_core_version(child, out)

def parse_extension(node, out: dict):
    extension_name = node.attrib.get("name", None)
    for req in node:
        parse_require(req, extension_name, out)

def parse_extensions(root, out: dict):
    for extension in root.find("extensions"):
        parse_extension(extension, out)

def parse_all(root):
    types = {}
    parse_core(root, types)
    parse_extensions(root, types)
    return types

def parse_exts(root):
    return set(ext.attrib["name"] for ext in root.find("tags"))

def get_ext(stype, exts):
    for ext in exts:
        if stype.endswith(ext):
            return ext

def parse_stypes(root, exts):
    aliases = {}
    stypes = {}
    for tp in root.find("types"):
        if tp.attrib.get("category", None) != "struct":
            continue
        type_name = tp.attrib["name"]
        if alias_target := tp.attrib.get("alias", None):
            aliases[type_name] = alias_target
            continue
        if tp.find("member")[0].text != "VkStructureType":
            continue
        if stype_value := tp[0].attrib.get("values", None):
            stypes[type_name] = stype_value
    for alias, target in aliases.items():
        if target_stype := stypes.get(target, None):
            if ext := get_ext(target_stype, exts):
                target_stype = target_stype.removesuffix("_" + ext)
            if ext := get_ext(alias, exts):
                target_stype += "_" + ext
            stypes[alias] = target_stype
    return stypes

def format_cond(g):
    return " || ".join(" && ".join(feature for feature in conj) for conj in g)

def format_type_header(t, g, stype, template_var):
    cond = format_cond(g)
    return (
    "#if {cond}\n"
    "   if constexpr(std::same_as<{template_var}, {tp}>) {{\n"
    "       return {stype};\n"
    "   }}\n"
    "#endif // {cond}").format(
        cond=cond,
        template_var=template_var,
        tp=t,
        stype=stype)

def format_types_header(types, stypes, template_var):
    return "\n".join(format_type_header(t, g, stypes[t], template_var) for t, g in types.items())

def generate_header(types, stypes):
    template_var = "T"
    return (
    "#pragma once\n"
    "#include <concepts>\n"
    "\n"
    "#include <vulkan/vulkan.h>\n"
    "\n"
    "template<typename S>\n"
    "consteval VkStructureType SType() {{\n"
    "   using {template_var} = std::remove_cvref_t<S>;\n"
    "{0}\n"
    "   throw \"Unknown sType!\";\n"
    "}}\n"
    "\n"
    "template<typename S>\n"
    "constexpr VkStructureType SType(S&&) {{\n"
    "   return SType<S>();\n"
    "}}\n").format(
        format_types_header(types, stypes, template_var),
        template_var=template_var)

def main():
    vk_xml = argv[1]
    header_out = Path(argv[2])

    tree = ET.parse(vk_xml)
    root = tree.getroot()

    types = parse_all(root)
    exts = parse_exts(root)
    stypes = parse_stypes(root, exts)
    types = dict(
        filter(lambda kv: kv[0] in stypes, types.items()))

    h = generate_header(types, stypes)

    header_out.parent.mkdir(exist_ok=True, parents=True)
    header_out.write_text(h)

if __name__ == "__main__":
    main()
