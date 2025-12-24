#!/usr/bin/env python3
import argparse
import os

dirs = []
included_files = set()
output = "#pragma once\n"

def find_file(file, from_dir):
    global dirs

    if os.path.isabs(file):
        if os.path.isfile(file):
            return file 
    else:
        for d in ([from_dir] + dirs):
            path = os.path.join(d, file)
            if os.path.isfile(path):
                return path
    raise FileNotFoundError(f"File {file} not found")

def include_file(file, from_dir):
    global output
    global included_files

    abs_file = os.path.normpath(find_file(file, from_dir))

    if abs_file in included_files:
        return
    included_files.add(abs_file)

    with open(abs_file, "r", encoding="utf-8") as f:
        for line in f:
            stripped = line.strip()
            if stripped.startswith('#pragma once'):
                continue
            if stripped.startswith('#include'):
                start = stripped.find('"')
                end = stripped.rfind('"')
                if start != -1 and end != -1 and end > start:
                    inc_name = stripped[start + 1 : end]
                    include_file(inc_name, os.path.dirname(abs_file))
                else:
                    output += line
            else:
                output += line

def main():
    global output
    global dirs

    p = argparse.ArgumentParser()
    p.add_argument('-o', '--output')
    p.add_argument('-I', dest='dirs', action='append', default=[])
    p.add_argument('headers', nargs='+')
    args = p.parse_args()

    dirs = [os.path.abspath(d) for d in args.dirs]

    for hdr in args.headers:
        include_file(hdr, os.path.abspath("."))

    if args.output:
        with open(args.output, "w", encoding="utf-8") as out_f:
            out_f.write(output)
    else:
        print(output)

if __name__ == '__main__':
    main()

