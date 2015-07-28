import cherrypy
import subprocess
from . import util
import os
import time
import base64
import queue
import random
import threading


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
        cherrypy.request.app.log('UPLOAD {}'.format(str(threading.current_thread())))
        # почему-то сессии сохраняются, только если что-нибудь у неё записать
        cherrypy.session['upload'] = True
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id, 'orig'))
        util.make_sure_path_exists(os.path.join('tmp', cherrypy.session.id, 'res'))

        data = cherrypy.request.body.read()

        fname = cherrypy.request.headers['X-FILE-NAME']
        cherrypy.request.app.log('data type: {} len: {} name: {}'.format(type(data), len(data), fname))
        fname = 'img' + os.path.splitext(fname)[1]

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

        # print(encoded)
        return encoded

    def clean_tmp_dir(self):
        cherrypy.log('Cleanup tmp dir')
        util.purge('tmp/', '^(?!\.)')
