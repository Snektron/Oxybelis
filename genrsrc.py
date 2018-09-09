# Made by Robin Voetter (s1835130)
import os
import argparse
from collections import defaultdict

incbin_template = """\
    .global {name}
    .type {name}, @object
    .align {align}
{name}:
    .incbin "{file}"
"""

header_template = """\
#ifndef {guard}
#define {guard}

#include <cstddef>

namespace {namespace} {{
    namespace externals {{
{externals}
    }}

    struct Resource {{
        const char* const data;
        const size_t size;
    }};

{resources}
}}

#endif
"""

extern_template = '\t\textern "C" const char {mangled_name}[];'

resource_template = '{indent}constexpr const Resource {name} = {{externals::{extern}, {size}}};'

resource_namespace_template = """\
{indent}namespace {namespace} {{
{nested}
{indent}}}"""

def mangle(prefix, name):
    return prefix + name.replace('/', '_').replace('.', '_')

def splitdirpath(path):
    return os.path.normpath(path).split(os.sep)[:-1]

class Directory:
    def __init__(self):
        self.subdirs = defaultdict(lambda: Directory())
        self.resources = []

    def append_subdir(self, child):
        return self.subdirs[child]

    def append_resource(self, child):
        self.resources.append(child)

    def insert_resource(self, path, resource):
        if not path:
            self.append_resource(resource)
        else:
            self.append_subdir(path[0]).insert_resource(path[1:], resource)

    def serialize_resources(self, depth = 1):
        indent = ' ' * depth * 4

        items = []
        for name, subdir in self.subdirs.items():
            code = resource_namespace_template.format(
                indent = indent,
                namespace = name,
                nested = subdir.serialize_resources(depth + 1)
            )
            items.append(code)

        for res in self.resources:
            items.append(res.resource(depth))

        return '\n'.join(items)

class Resource:
    def __init__(self, prefix, name, path):
        self.prefix = prefix
        self.name = name
        self.path = path
        self.size = os.path.getsize(path)
        self.mangled_name = mangle(prefix, name)

    def __repr__(self):
        return '{{{} {} {} {}}}'.format(self.prefix, self.name, self.path, self.size)

    def write_asm(self, f):
        f.write(incbin_template.format(name = self.mangled_name, align = 4, file = self.name).encode())

    def extern(self):
        return extern_template.format(mangled_name = self.mangled_name)

    def resource(self, depth):
        basename = os.path.basename(self.path).replace('.', '_')
        return resource_template.format(indent = ' ' * depth * 4, name = basename, size = self.size, extern = self.mangled_name)

def write_asm(output, resources):
    with open(output, 'wb') as f:
        f.write(b'.section .rodata\n')
        for resource in resources:
            resource.write_asm(f)

def write_header(output, namespace, guard, resources, resource_tree):
    externals = '\n'.join([resource.extern() for resource in resources])
    header = header_template.format(
        guard = guard,
        namespace = namespace,
        externals = externals,
        resources = resource_tree.serialize_resources()
    )

    with open(output, 'wb') as f:
        f.write(header.encode())

def find_file(name, directories):
    for dir in directories:
        path = os.path.join(dir, name)
        if os.path.isfile(path):
            return path
    raise FileNotFoundError("Resource '{}' not found".format(name))

parser = argparse.ArgumentParser(description = 'Generate asm file and header from resources')
parser.add_argument('resources', metavar = '<resources>', nargs = '*', help = 'List of resources to compile')
parser.add_argument('-o', '--output', metavar = ('<asm output>', '<header output>'), nargs = 2, required = True, help = 'Output of generated assembly and header files')
parser.add_argument('-p', '--prefix', metavar = '<prefix>', default = '', help = 'Prefix of assembly labels/extern variable names')
parser.add_argument('-I', '--include', metavar = '<path>', action = 'append', required = True, help = 'Search directories for resources')
parser.add_argument('-n', '--namespace', metavar = '<namespace>', default = 'resource', help = 'Namespace to place externals in, defaults to "resource"')
parser.add_argument('-g', '--guard', metavar = '<guard>', default = '_RESOURCES_H', help = 'Header guard, defaults to _RESOURCES_H')
args = parser.parse_args()

resource_tree = Directory()
resources = []

for name in args.resources:
    path = find_file(name, args.include)
    res = Resource(args.prefix, name, path)
    resource_tree.insert_resource(splitdirpath(name), res)
    resources.append(res)

write_asm(args.output[0], resources)
write_header(args.output[1], args.namespace, args.guard, resources, resource_tree)
