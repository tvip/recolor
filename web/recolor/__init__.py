import cherrypy
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


_proc_session = {}
_logger = ProcLogger(['utils/bin/dummy'])


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


class Image(object):
    @cherrypy.expose
    def index(self, image):
        logger = _images()[image]
        print(logger._proc)

        if logger._proc.poll() is None:
            logger._proc.communicate()

        with open(os.path.join('tmp', cherrypy.session.id, 'res', image), 'rb') as res_file:
            # print(str(type(res_file)))
            encoded = base64.b64encode(res_file.read())

        #cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        #cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return encoded

    #index._cp_config = {'response.stream': True}


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
        self.images = Image()
        self._stdout = Stdout()
        self._stderr = Stderr()

    def _cp_dispatch(self, vpath):
        print('DISPATCH', type(vpath), vpath)

        if len(vpath) == 1:
            cherrypy.request.params['image'] = vpath.pop(0)
            return self.images

        if len(vpath) == 2:
            stream = vpath.pop(0)
            if stream == 'stdout':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self._stdout
            if stream == 'stderr':
                cherrypy.request.params['image'] = vpath.pop(0)
                return self._stderr

        return vpath

    @cherrypy.expose
    def thing(self):
        '''
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
        
        # proc = subprocess.Popen(['utils/bin/dummy'])  # ,
        # stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        p = Pocessing()
        p.proc = proc
        _proc_session[cherrypy.session.id] = p
        cherrypy.session['processing'] = p

        stdout_reader = util.AsynchronousFileReader(proc.stdout, p.stdout_queue)
        stdout_reader.start()

        stderr_reader = util.AsynchronousFileReader(proc.stderr, p.stderr_queue)
        stderr_reader.start()
        '''

        # stderr_reader.join()
        # proc.stderr.close()
        '''
    def content():
      for msg in stdout_reader:
        yield 'data: ' + str(msg) + '\n\n'
        proc.stdout.close()
    '''
        # proc.communicate()
        # proc.wait()
        # proc.stdout.close()
        # proc.stderr.close()
        
        logger = ProcLogger(['utils/bin/dummy'])
        #logger.start()
        
        cherrypy.session['dummy_logger'] = logger
        cherrypy.session['dummy_logger'].start()
        
        _logger.start()
        
        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return 'data: {} oki\n\n'.format(cherrypy.session.id)

    # thing._cp_config = {'response.stream': True}
    '''
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
                    else:
                        eof = True
                        break

                if eof:
                    break

                time.sleep(.1)

        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return content()
    '''

    '''
    @cherrypy.expose()
    def stdout(self):
        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return self._stream(_logger.threads[0])
    
    stdout._cp_config = {'response.stream': True}

    @cherrypy.expose()
    def stderr(self):
        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return self._stream(_logger.threads[0])

    stderr._cp_config = {'response.stream': True}
    '''

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
        # fname = 'img' + os.path.splitext(fname)[1]
        # print('type', type(cherrypy.session['images']))
        # print(dir(cherrypy.session))
        # print('SESSION', _images())
        _images()[fname] = None

        path = os.path.join('tmp', cherrypy.session.id, 'orig', fname)
        with open(path, 'wb') as file:
            file.write(data)

    @cherrypy.expose()
    def matrix(self, matrix):
        cherrypy.request.app.log('MATRIX {} {}'.format(str(threading.current_thread()), matrix))
        cherrypy.session['matrix'] = True
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id))

        matrix_path = os.path.join('tmp', cherrypy.session.id, 'matrix.txt')
        with open(matrix_path, 'w') as file:
            file.write(matrix)

        for fname in _images():
            '''
            logger = ProcLogger(['utils/bin/dummy'])
            '''
            logger = ProcLogger([
                'utils/bin/recolor-tool',
                os.path.join('tmp', cherrypy.session.id, 'matrix.txt'),
                '-img',
                os.path.join('tmp', cherrypy.session.id, 'orig', fname),
                os.path.join('tmp', cherrypy.session.id, 'res', fname)
            ])
            logger.start()
            _images()[fname] = logger
        '''
        fname = 'img.png'

        proc = subprocess.Popen([
            'utils/bin/recolor-tool',
            os.path.join('tmp', cherrypy.session.id, 'matrix.txt'),
            '-img',
            os.path.join('tmp', cherrypy.session.id, 'orig', fname),
            os.path.join('tmp', cherrypy.session.id, 'res', fname)
        ])

        proc.communicate()

        with open(os.path.join('tmp', cherrypy.session.id, 'res', fname), 'rb') as res_file:
            # print(str(type(res_file)))
            encoded = base64.b64encode(res_file.read())
        '''

        # print(encoded)
        # return encoded
        return 'ok'

    def clean_tmp_dir(self):
        cherrypy.log('Cleanup tmp dir')
        util.purge('tmp/', '^(?!\.)')
