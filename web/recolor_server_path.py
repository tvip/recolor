import os

server_dir = os.path.dirname(os.path.realpath(__file__))
source_dir = os.path.realpath(os.path.join(server_dir, '..'))
build_dir = os.path.realpath(os.path.join(server_dir, 'build'))
recolor_tool = os.path.realpath(os.path.join(build_dir, 'Staging', 'bin', 'recolor-tool'))
