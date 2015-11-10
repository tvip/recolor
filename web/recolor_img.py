#!/usr/bin/env python3

import sys
import os
import subprocess
import recolor_server_path as path


def recolor_img(fname):
    stdout, stderr = subprocess.Popen([
        path.recolor_tool,
        os.path.join(matrix_fname),
        '-img', fname, fname
    ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    ).communicate()


if __name__ == '__main__':
    if len(sys.argv) is not 3:
        print('Usage:', sys.argv[0], '<matrix>', '<themes_dir>')
        print(
            'Этот скрипт перекрашивает все *.png картинки в указанной дирректории'
            ' в соответствии файлом с матрицы.'
        )
        exit()

    print(' '.join(sys.argv))
    matrix_fname = sys.argv[1]
    working_dir = sys.argv[2]

    for root, dirs, files in os.walk(working_dir):
        for file in files:
            if '.png'.upper() == os.path.splitext(file)[-1].upper():
                if file == 'button_helper_icons.png':
                    continue
                recolor_img(os.path.join(root, file))
