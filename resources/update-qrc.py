#!/usr/bin/env python
# -*- coding: utf-8; -*-

"""
Updates the qrc files according to the contents of the subdirectories.
"""

import os
import sys
from xml.etree import ElementTree

RESOURCE_PATH = os.path.dirname(__file__)

def make_qrc(qrc, path):
    output = open(qrc, 'w')
    builder = ElementTree.TreeBuilder()
    builder.start('RCC', {'version': '1.0'})
    builder.start('qresource', {})
    for root, dirs, files in os.walk(path):
        for f in files:
            builder.start('file', {})
            builder.data(os.path.relpath(os.path.join(root, f), RESOURCE_PATH))
            builder.end('file')
    builder.end('qresource')
    builder.end('RCC')
    xml = ElementTree.ElementTree(builder.close())
    xml.write(output)
    output.close()

def main(args):
    for entry in os.listdir(RESOURCE_PATH):
        path = os.path.join(RESOURCE_PATH, entry)
        if os.path.isdir(path):
            qrc = os.path.join(RESOURCE_PATH, entry + '.qrc')
            make_qrc(qrc, path)
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
