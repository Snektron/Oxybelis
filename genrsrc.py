import os
import argparse

incbin_template = """
    .global {name}
    .type {name}, @object
    .align {align}
{name}:
    .incbin "{file}"
{name}_end:
    .global {name}_size
    .type {name}, @object
    .align 4
{name}_size:
    .quad {name}_end - {name}
"""

def write_asm(f, namespace, resources):
    f.write(b".section .rodata")
    for name in resources:
        mangled_name = namespace + name.replace('/', '_').replace('.', '_')
        f.write(incbin_template.format(name = mangled_name, align = 4, file = name).encode())

parser = argparse.ArgumentParser(description = 'Generate asm file from resources')
parser.add_argument('resources', metavar = 'resource', nargs = '*')
parser.add_argument('-o', '--output', metavar = 'output', required = True)
parser.add_argument('-n', '--namespace', metavar = 'namespace', default = '_')
args = parser.parse_args()

with open(args.output, 'wb') as f:
    write_asm(f, args.namespace, args.resources)
