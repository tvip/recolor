import cherrypy
import subprocess
from . import util
import os
import time


class Recolor(object):
  def __init__(self):
    object.__init__(self)
    self.clean_tmp_dir()

  @cherrypy.expose
  def thing(self):
    orig_color = 'orig'
    res_color = 'green'
    cherrypy.request.app.log('SESSION: ' + cherrypy.session.id)

    proc = subprocess.Popen([
      'utils/bin/recolor-tool',
      os.path.join('test-data', res_color, 'matrix.txt'),
      '-in', os.path.join('test-data', orig_color, 'tvip_light/resources'),
      '-out', os.path.join('tmp', cherrypy.session.id, 'tvip_light/resources'),
      '-xpath', '//image[@file]', '-xattr', 'file',
      '-xml', os.path.join('test-data', orig_color, 'tvip_light/resources.xml')
    ], stdout=subprocess.PIPE)  # , stderr=subprocess.PIPE)

    stdout_reader = util.AsynchronousFileReader(proc.stdout)
    stdout_reader.start()

    proc.terminate()

    # stderr_reader = util.AsynchronousFileReader(proc.stderr)
    # stderr_reader.start()

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

  @cherrypy.expose()
  def stdout(self):
    def content():
      i = 0
      while True:
        i = i + 1
        yield 'data: stdout' + str(i) + '\n\n'
        time.sleep(1)
        # for msg in stdout_reader:
        #   yield 'data: ' + str(msg) + '\n\n'
        # proc.stdout.close()

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()

  stdout._cp_config = {'response.stream': True}

  @cherrypy.expose()
  def stderr(self):
    def content():
      i = 0
      while True:
        i = i + 1
        yield 'data: stderr' + str(i) + '\n\n'
        time.sleep(1)

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
