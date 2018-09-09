import os
import argparse

incbin_template = """\
    .global {name}
    .type {name}, @object
    .align {align}
{name}:
    .incbin "{file}"
"""

guard_header = """\
#ifndef {guard}
#define {guard}

"""

guard_footer = b'\n#endif'

extern_template = """\
extern const char {name}[];
long long unsigned int {name}_size = {size};
"""

def mangle(namespace, name):
    return namespace + name.replace('/', '_').replace('.', '_')

class Resource:
    def __init__(self, namespace, name, path):
        self.namespace = namespace
        self.name = name
        self.path = path
        self.size = os.path.getsize(path)
        self.mangled_name = mangle(namespace, name)

    def __repr__(self):
        return '{{{} {} {} {}}}'.format(self.namespace, self.name, self.path, self.size)

    def write_asm(self, f):
        f.write(incbin_template.format(name = self.mangled_name, align = 4, file = self.name).encode())

    def write_extern(self, f):
        f.write(extern_template.format(name = self.mangled_name, size = self.size).encode())

def write_asm(output, resources):
    with open(output, 'wb') as f:
        f.write(b'.section .rodata\n')
        for resource in resources:
            resource.write_asm(f)

def write_header(output, namespace, guard, resources):
    with open(output, 'wb') as f:
        f.write(guard_header.format(guard = guard).encode())
        if namespace is not None:
            f.write('namespace {} {{\n'.format(namespace).encode())
        for resource in resources:
            resource.write_extern(f)
        if namespace is not None:
            f.write('}}\n'.format(namespace).encode())
        f.write(guard_footer)

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
parser.add_argument('-n', '--namespace', metavar = '<namespace>', help = 'Namespace to optionally place externals in')
parser.add_argument('-g', '--guard', metavar = '<guard>', default = '_RESOURCES_H', help = 'Header guard, defaults to _RESOURCES_H')
args = parser.parse_args()

resources = [Resource(args.prefix, name, find_file(name, args.include)) for name in args.resources]
write_asm(args.output[0], resources)
write_header(args.output[1], args.namespace, args.guard, resources)
