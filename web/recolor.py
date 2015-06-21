import subprocess

# Simple command
subprocess.call(['ls', '-1'])

# Command with shell expansion
subprocess.call('echo $HOME', shell=True)

subprocess.call('pwd')

subprocess.call(['utils/bin/recolor-tool'])
