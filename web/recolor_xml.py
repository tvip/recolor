#!/usr/bin/env python3

import os
import re
import xml.etree.ElementTree as ET


def color_from_string(string):
    match_hex_color = re.match('^#(([0-9a-f]){6}|([0-9a-f]){8})$', string, flags=re.IGNORECASE)
    if match_hex_color:
        hex_string = match_hex_color.groups()[0]
        result = []
        i = 0
        while True:
            component = hex_string[i:i+2]
            if not component: break
            result.append(int(component, 16))
            i += 2
        return result


def recolor_node(node):
    for key, value in node.attrib.items():
        color = color_from_string(value)
        if color:
            print('ATTR', color)
    if node.text:
        color = color_from_string(node.text)
        if color:
            print('TEXT', color)
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
