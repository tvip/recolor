#!/usr/bin/env python3

import sys
import os
import re
import subprocess
import xml.etree.ElementTree as ET
import recolor_server_path as path


def color_from_string(string):
    match_hex_color = re.match('^#(([0-9a-f]){6}|([0-9a-f]){8})$', string, flags=re.IGNORECASE)
    if match_hex_color:
        hex_string = match_hex_color.groups()[0]
        result = []
        i = 0
        while True:
            component = hex_string[i:i+2]
            if not component: break
            result.append(int(component,16) / 255.0)
            i += 2
        return result


def color_to_string(color):
    result = ['#']
    for component in color:
        result.append('{:02x}'.format(round(255*component)))
    return ''.join(result)


def recolor(color):
    alpha, red, green, blue = color

    stdout, stderr = subprocess.Popen([
        path.recolor_tool,
        os.path.join(matrix_fname),
        str(red), str(green), str(blue)
    ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    ).communicate()

    result = []

    if alpha is not None:
        result.append(alpha)

    result.extend([float(token) for token in stdout.decode().split()])

    return result


def recolor_node(node):
    for key, value in node.attrib.items():
        color = color_from_string(value)
        if color:
            new_color = color_to_string(recolor(color))
            print('ATTR', value, '->', new_color)
            node.attrib[key] = new_color
    if node.text:
        color = color_from_string(node.text)
        if color:
            new_color = color_to_string(recolor(color))
            print('TEXT', node.text, '->', new_color)
            node.text = new_color
    for child in node:
        recolor_node(child)


def recolor_xml(fname):
    print(fname)
    tree = ET.parse(fname)
    recolor_node(tree.getroot())
    tree.write(fname)


if __name__ == '__main__':
    if len(sys.argv) is not 3:
        print('Usage:', sys.argv[0], '<matrix>', '<themes_dir>')
        print(
            'Этот скрипт рекурсивно сканирует xml файлы в указанной дирректории.'
            ' И если находит цвет записанный в хексовом формате заменяет его в'
            ' в соответствии файлом с матрицы. Цвета ищются в заначениях xml'
            ' атрибутов и теле узлов.'
        )
        exit()

    print(' '.join(sys.argv))
    matrix_fname = sys.argv[1]
    working_dir = sys.argv[2]

    for root, dirs, files in os.walk(working_dir):
        for file in files:
            if '.xml'.upper() == os.path.splitext(file)[-1].upper():
                recolor_xml(os.path.join(root, file))
