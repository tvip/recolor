import cherrypy
import subprocess
import threading
import time


class AsynchronousFileReader(threading.Thread):

  def __init__(self, fd, callback):
    assert callable(fd.readline)
    assert callable(callback)
    threading.Thread.__init__(self)
    self._fd = fd
    self._callback = callback

  def run(self):
    # print('RUN')

    while True:
      chunk = self._fd.readline()
      # print('READ', type(chunk), len(chunk), chunk.decode())
      if not chunk:
        break
      self._callback(str(chunk))


def recolor_tool_output(string):
  yield cherrypy.request.app.log('recolor-tool log: ' + string)


class Recolor(object):

  @cherrypy.expose
  def thing(self):
    '''
    orig_color = 'orig'
    res_color = 'green'
    cherrypy.request.app.log('SESSION: ' + cherrypy.session.id)

    proc = subprocess.Popen([
        'utils/bin/recolor-tool',
        'test-data/' + res_color + '/matrix.txt',
        '-in', 'test-data/' + orig_color + '/tvip_light/resources',
        '-out', 'tmp/' + cherrypy.session.id + '/tvip_light/resources',
        '-xpath', '//image[@file]', '-xattr', 'file',
        '-xml', 'test-data/' + orig_color + '/tvip_light/resources.xml'
      ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    stdout_reader = AsynchronousFileReader(proc.stdout, recolor_tool_output)
    stdout_reader.start()

    stderr_reader = AsynchronousFileReader(proc.stderr, recolor_tool_output)
    stderr_reader.start()

    stdout_reader.join()
    stderr_reader.join()

    proc.stdout.close()
    proc.stderr.close()
    '''

    def content():
      for i in range(10):
        yield 'data: ' + str(i) + '\n\n'
        #time.sleep(1)

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()
  thing._cp_config = {'response.stream': True}
