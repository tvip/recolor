import cherrypy
import subprocess
import threading
import queue


class AsynchronousFileReader(threading.Thread):
  def __init__(self, fd):
    assert callable(fd.readline)
    threading.Thread.__init__(self)
    self._fd = fd
    self._eventQueue = queue.Queue()

  def __iter__(self):
    while True:
      message = self._eventQueue.get(block=True)
      if message:
        yield message
      else:
        break

  def run(self):
    while True:
      chunk = self._fd.readline()
      if not chunk:
        break
      self._eventQueue.put(chunk)
    self._eventQueue.put(None)


class Recolor(object):
  @cherrypy.expose
  def thing(self):
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

    stdout_reader = AsynchronousFileReader(proc.stdout)
    stdout_reader.start()

    stderr_reader = AsynchronousFileReader(proc.stderr)
    stderr_reader.start()

    # stderr_reader.join()
    # proc.stderr.close()

    def content():
      for msg in stdout_reader:
        yield 'data: ' + str(msg) + '\n\n'
      proc.stdout.close()

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()

  thing._cp_config = {'response.stream': True}
