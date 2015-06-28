import cherrypy
import subprocess
from . import util
import os
import time
import base64
import queue
import random


class Pocessing:
  def __init__(self):
    self.proc = None
    self.stdout_queue = queue.Queue()
    self.stderr_queue = queue.Queue()


_proc_session = {}


class Recolor(object):
  def __init__(self):
    object.__init__(self)
    self.clean_tmp_dir()

  @cherrypy.expose
  def thing(self):
    orig_color = 'orig'
    res_color = 'green'
    cherrypy.request.app.log('SESSION: ' + cherrypy.session.id)

    if cherrypy.session.id in _proc_session:
      p = _proc_session[cherrypy.session.id]
      cherrypy.request.app.log('PID: {} POLL: {}'.format(str(p.proc.pid), str(p.proc.poll())))
      if p.proc.poll() is None:  # Нет returncode, значит ещё работает
        cherrypy.request.app.log('interrupt processing')
        p.proc.terminate()
        # proc.stdout.close()
        # proc.stderr.close()

    proc = subprocess.Popen([
      'utils/bin/recolor-tool',
      os.path.join('test-data', res_color, 'matrix.txt'),
      '-in', os.path.join('test-data', orig_color, 'tvip_light/resources'),
      '-out', os.path.join('tmp', cherrypy.session.id, 'tvip_light/resources'),
      '-xpath', '//image[@file]', '-xattr', 'file',
      '-xml', os.path.join('test-data', orig_color, 'tvip_light/resources.xml')
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # proc = subprocess.Popen(['utils/bin/dummy'])  # , stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    p = Pocessing()
    p.proc = proc
    _proc_session[cherrypy.session.id] = p
    cherrypy.session['processing'] = p

    stdout_reader = util.AsynchronousFileReader(proc.stdout, p.stdout_queue)
    stdout_reader.start()

    stderr_reader = util.AsynchronousFileReader(proc.stderr, p.stderr_queue)
    stderr_reader.start()

    # stderr_reader.join()
    # proc.stderr.close()
    '''
    def content():
      for msg in stdout_reader:
        yield 'data: ' + str(msg) + '\n\n'
        proc.stdout.close()
    '''
    # proc.communicate()
    # cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    # cherrypy.response.headers['Cache-Control'] = 'no-cache'
    # proc.wait()
    # proc.stdout.close()
    # proc.stderr.close()
    return 'data: oki\n\n'

  # thing._cp_config = {'response.stream': True}

  @cherrypy.expose()
  def stdout(self):

    def content():

      while cherrypy.session.id in _proc_session:
        proc = _proc_session[cherrypy.session.id]
        eof = False

        while not proc.stdout_queue.empty():
          message = proc.stdout_queue.get()

          if message is not None:
            encoded = base64.b64encode((cherrypy.session.id + message.decode()).rstrip('\r\n').encode())
            yield 'data: ' + encoded.decode() + '\n\n'
            time.sleep(.1)
          else:
            eof = True
            break

        if eof:
          break

        time.sleep(.1)

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()

  stdout._cp_config = {'response.stream': True}

  @cherrypy.expose()
  def stderr(self):
    '''
    def content():
      if 'stderr_reader' in cherrypy.session:
        stderr_reader = cherrypy.session['stderr_reader']
        for msg in stderr_reader:
          encoded = base64.b64encode(msg.decode().rstrip('\r\n').encode())
          yield 'data: ' + encoded.decode() + '\n\n'
    '''

    def content():
      i = 0
      while True:
        i = i + 1
        yield 'data: stderr' + str(i) + '\n\n'
        time.sleep(.2)

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()

  stderr._cp_config = {'response.stream': True}

  @cherrypy.expose()
  def upload(self):
    cherrypy.request.app.log('UPLOAD')
    cherrypy.session['upload'] = True  # почему-то сессии сохраняются, только если что-нибудь у неё записать
    util.make_sure_path_exists(os.path.join('tmp', 'orig', cherrypy.session.id))

    data = cherrypy.request.body.read()

    fname = cherrypy.request.headers['X-FILE-NAME']
    cherrypy.request.app.log('data type: {} len: {} name: {}'.format(type(data), len(data), fname))

    path = os.path.join('tmp', 'orig', cherrypy.session.id, fname)
    with open(path, 'wb') as file:
      file.write(data)

  def clean_tmp_dir(self):
    cherrypy.log('Cleanup tmp dir')
    util.purge('tmp/', '^(?!\.)')
