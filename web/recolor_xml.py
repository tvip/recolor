#!/usr/bin/env python3

import os
import re
import xml.etree.ElementTree as ET


def recolor_node(node):
    # print(node.tag, node.attrib)
    if node.tag == 'color':
        print('color', node.text)
    for child in node:
        recolor_node(child)


def recolor_xml(fname):
    print(fname)
    tree = ET.parse(fname)
    recolor_node(tree.getroot())


if __name__ == '__main__':

    for root, dirs, files in os.walk('/tvip/tvip_stb/tvip/themes'):
        for file in files:
            if '.xml' == os.path.splitext(file)[-1]:
                recolor_xml(os.path.join(root, file))

    match_hex_color = re.match('^#([0-9a-f]*)$', '#11223344')
    if match_hex_color:
        hex_string = match_hex_color.groups()[0]
        number = int(hex_string, 16)
        
        print('match', int(hex_string, 16))
        pass
