#!/usr/bin/env python3

import os
import subprocess
import multiprocessing

import recolor_server_path as path


if __name__ == '__main__':
	os.chdir( path.build_dir )

	cmake_command = 'cmake', '-DCMAKE_BUILD_TYPE:STRING=Release', '-DBUILD_TESTING:BOOL=OFF', path.source_dir
	subprocess.Popen( cmake_command ).communicate()

	make_command = 'make', '-j{}'.format(multiprocessing.cpu_count())
	subprocess.Popen( make_command ).communicate()

	print('Done')
