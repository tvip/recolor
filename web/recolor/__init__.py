import cherrypy
import subprocess

import sys, os
import contextlib


@contextlib.contextmanager
def monitor(stream, callback):
  read, write = os.pipe()
  yield write
  os.close(write)
  with os.fdopen(read) as f:
    for line in f:
      callback(line)
      stream.write(line)


def f(s):
  print("Write", s)


with monitor(sys.stdout, f) as stream:
  p = subprocess.Popen(["ls"], stdout=stream)
  p.communicate()


class Recolor(object):

  def recolor_tool_output(self, string):
    cherrypy.request.app.log(string)

  @cherrypy.expose
  def index(self):
    orig_color = 'orig'
    res_color = 'green'

    with monitor(sys.stdout, self.recolor_tool_output) as output_log, monitor(sys.stderr, self.recolor_tool_output) as error_log:
      print(type(output_log), type(error_log))

      proc = subprocess.Popen([
        'utils/bin/recolor-tool',
        'test-data/' + res_color + '/matrix.txt',
        '-in', 'test-data/' + orig_color + '/tvip_light/resources',
        '-out', 'test-data/' + res_color + '/tvip_light/resources',
        '-xpath', '//image[@file]', '-xattr', 'file',
        '-xml', 'test-data/' + orig_color + '/tvip_light/resources.xml'
      ], stdout=output_log, stderr=error_log)
      proc.communicate()
      exitcode = proc.returncode

      cherrypy.request.app.log(str(exitcode))

    return 'Done'
