import cherrypy
import subprocess
from . import util

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

    stdout_reader = util.AsynchronousFileReader(proc.stdout)
    stdout_reader.start()

    stderr_reader = util.AsynchronousFileReader(proc.stderr)
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

  @cherrypy.expose()
  def upload(self):
    cherrypy.request.app.log('UPLOAD')
    util.make_sure_path_exists('tmp/' + cherrypy.session.id)

    data = cherrypy.request.body.read()
    fname = cherrypy.request.headers['X-FILE-NAME']
    cherrypy.request.app.log('data type: {} len: {} name: {}'.format(type(data), len(data), fname))

    path = 'tmp/{}/{}'.format(cherrypy.session.id, fname)
    with open(path, 'wb') as file:
      file.write(data)
