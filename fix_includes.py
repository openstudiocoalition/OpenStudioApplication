# Meant to be run one level above OpenStudioApplication
import os
import re
import glob as gb

ROOT_DIR = './'
NEW_SRC = './src/'
OLD_SRC = './openstudio/src'

# Get a list of the openstudio SDK (core) folders
core_folders = next(os.walk(OLD_SRC))[1]

re_include = re.compile(r'#include\s+"(.*)"')

for p in gb.glob('{}/**/*'.format(NEW_SRC), recursive=True):

    has_change = False
    if not os.path.isfile(p):
        # print("Skipping directory {}".format(p))
        continue

    _, ext = os.path.splitext(p)
    if ext == '.png':
        # print("Skipping image {}".format(p))
        continue

    # print(p)

    this_folder = os.path.relpath(os.path.join(p, os.pardir), start=NEW_SRC)
    old_folder = os.path.join(OLD_SRC, this_folder)
    try:
        with open(p, 'r') as f:
            lines = f.read().splitlines()
    except ValueError:
        print("Can't open {}".format(p))*print(ext)
        continue

    for i, line in enumerate(lines):
        m = re_include.match(line)
        if m:
            this_include = m.groups()[0]

            # Split the path into a list of components
            # eg: '../model/Model.hpp'
            # => ['..', 'model']
            path_components = (os.path.dirname(os.path.normpath(this_include))
                                      .split(os.sep))

            # If we find an element present in both lists:
            if (set(path_components) & set(core_folders)):
                has_change = True
                old_include_abs_path = os.path.join(old_folder, this_include)
                new_include = os.path.relpath(old_include_abs_path,
                                              start=ROOT_DIR)
                new_include_str = "#include <{}>".format(new_include)
                # print(new_dir, this_include)
                # print(new_include_str)
                lines[i] = new_include_str

    if has_change:
        with open(p, 'w') as f:
            f.write('\n'.join(lines))
