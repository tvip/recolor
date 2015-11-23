#!/usr/bin/env python3

import sys
import os
import re
import subprocess
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


def replace_color(match):
    before = match.group()
    color = color_from_string(before)
    result = color_to_string(recolor(color))
    print('REPLACE', before, '->', result)
    return result


def recolor_xml(fname):
    print(fname)
    with open(fname, 'r+') as xml_file:
        result = re.sub('#(([0-9a-f]){8}|([0-9a-f]){6})', replace_color, xml_file.read(), flags=re.IGNORECASE)
        xml_file.seek(0)
        xml_file.write(result)
        xml_file.truncate()


if __name__ == '__main__':
    if len(sys.argv) is not 3:
        print('Usage:', sys.argv[0], '<matrix>', '<themes_dir>')
        print(
            'Этот скрипт рекурсивно сканирует xml файлы в указанной дирректории.'
            ' И если находит цвет записанный в хексовом формате заменяет его в'
            ' в соответствии с файлом матрицы. Цвета ищутся в значениях xml'
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
