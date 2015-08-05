import cherrypy
import jinja2
import jinja2plugin

import subprocess
from . import util
import os
import time
import base64
import queue
import random
import threading
from recolor.util import ProcLogger


class Pocessing:

    def __init__(self):
        self.proc = None
        self.stdout_queue = queue.Queue()
        self.stderr_queue = queue.Queue()


def _images() -> map:
    try:
        images = cherrypy.session['images']
    except:
        images = {}
        cherrypy.session['images'] = images
    finally:
        return images


def _stream(content):
    for msg in content:
        encoded = base64.b64encode('{} '.format(cherrypy.session.id).encode() + msg.decode().rstrip().encode())
        yield 'data: ' + encoded.decode() + '\n\n'


class ImageBase64(object):

    @cherrypy.expose
    def index(self, image, timestamp=0):
        logger = _images()[image]
        print(logger._proc)

        if logger._proc.poll() is None:
            logger._proc.communicate()

        with open(os.path.join('tmp', cherrypy.session.id, 'res', image), 'rb') as res_file:
            encoded = base64.b64encode(res_file.read())

        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return encoded


class Image(object):

    @cherrypy.expose
    def index(self, image, timestamp=0):
        logger = _images()[image]
        print(logger._proc)

        if logger._proc.poll() is None:
            logger._proc.communicate()

        with open(os.path.join('tmp', cherrypy.session.id, 'res', image), 'rb') as res_file:
            data = res_file.read()

        cherrypy.response.headers['Content-Type'] = "image/png"
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return data


class Stdout(object):

    @cherrypy.expose
    def index(self, image):
        def content():
            logger = _images()[image]
            return _stream(logger.threads[0])

        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return content()

    index._cp_config = {'response.stream': True}


class Stderr(object):

    @cherrypy.expose
    def index(self, image):
        def content():
            logger = _images()[image]
            return _stream(logger.threads[1])

        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return content()

    index._cp_config = {'response.stream': True}


class Recolor(object):

    def __init__(self):
        object.__init__(self)
        self.clean_tmp_dir()

        package_dir = os.path.dirname(os.path.abspath(__file__))
        templates_dir = os.path.join(package_dir, 'templates')
        print('PATH:', templates_dir)

        env = jinja2.Environment(loader=jinja2.FileSystemLoader(templates_dir))
        jinja2plugin.Jinja2TemplatePlugin(cherrypy.engine, env=env).subscribe()

        self.images = Image()
        self.images_base64 = ImageBase64()
        self._stdout = Stdout()
        self._stderr = Stderr()

    @cherrypy.expose
    @cherrypy.tools.template(template='index.html')
    def index(self):
        '''
        TODO: Попробывать сделать свою сессию для каждого таба в браузере
        http://stackoverflow.com/questions/368653/how-to-differ-sessions-in-browser-tabs
        '''
        cherrypy.session.update([])
        return {'session_id': cherrypy.session.id}

    def _cp_dispatch(self, vpath):
        print('DISPATCH', type(vpath), vpath)

        if len(vpath) == 2:
            stream = vpath.pop(0)
            if stream == 'image':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self.images
            if stream == 'base64':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self.images_base64
            if stream == 'stdout':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self._stdout
            if stream == 'stderr':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self._stderr

        return vpath

    @cherrypy.expose()
    def upload(self):
        cherrypy.request.app.log('UPLOAD {}'.format(str(threading.current_thread())))
        # почему-то сессии сохраняются, только если что-нибудь у неё записать
        cherrypy.session['upload'] = True
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id, 'orig'))
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id, 'res'))

        data = cherrypy.request.body.read()

        fname = cherrypy.request.headers['X-FILE-NAME']
        cherrypy.request.app.log('data type: {} len: {} name: {}'.format(type(data), len(data), fname))

        _images()[fname] = None

        path = os.path.join('tmp', cherrypy.session.id, 'orig', fname)
        with open(path, 'wb') as file:
            file.write(data)

        return {}

    @cherrypy.expose()
    def matrix(self, matrix):
        cherrypy.request.app.log('MATRIX {} {}'.format(str(threading.current_thread()), matrix))
        cherrypy.session['matrix'] = True
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id))

        matrix_path = os.path.join('tmp', cherrypy.session.id, 'matrix.txt')
        with open(matrix_path, 'w') as file:
            file.write(matrix)

        for fname in _images():
            logger = ProcLogger([
                'utils/bin/recolor-tool',
                os.path.join('tmp', cherrypy.session.id, 'matrix.txt'),
                '-img',
                os.path.join('tmp', cherrypy.session.id, 'orig', fname),
                os.path.join('tmp', cherrypy.session.id, 'res', fname)
            ])
            logger.start()
            _images()[fname] = logger

        return 'ok'.encode()

    def clean_tmp_dir(self):
        cherrypy.log('Cleanup tmp dir')
        util.purge('tmp/', '^(?!\.)')
