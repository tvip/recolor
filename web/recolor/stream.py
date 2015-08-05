import cherrypy
import base64
import threading
import queue
import subprocess


class AsynchronousFileReader(threading.Thread):

    def __init__(self, fd):
        assert callable(fd.readline)
        threading.Thread.__init__(self)
        self._fd = fd
        self.log = queue.Queue()

    def __iter__(self):
        self.start()
        while True:
            message = self.log.get(block=True)
            if message:
                yield message
            else:
                break

    def run(self):
        while True:
            chunk = self._fd.readline()
            if not chunk:
                self.log.put('')
                break
            self.log.put(
                '{} {}'.format(str(threading.current_thread()), chunk))


class Stream(object):

    @cherrypy.expose()
    def stdout(self):
        proc = subprocess.Popen([
            'utils/bin/dummy'
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        stdout_reader = AsynchronousFileReader(proc.stdout)

        def content(reader):
            for line in reader:
                chunk = '{thread} {session} {message}'.format(
                    thread=str(threading.current_thread()),
                    session=cherrypy.session.id,
                    message=line
                )

                encoded = base64.b64encode(chunk.encode())
                yield 'data: ' + encoded.decode() + '\n\n'

        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return content(stdout_reader)

    stdout._cp_config = {'response.stream': True}

    @cherrypy.expose()
    def stderr(self):
        proc = subprocess.Popen([
            'utils/bin/dummy'
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        stderr_reader = AsynchronousFileReader(proc.stderr)

        def content(reader):
            for line in reader:
                chunk = '{thread} {session} {message}'.format(
                    thread=str(threading.current_thread()),
                    session=cherrypy.session.id,
                    message=line
                )

                encoded = base64.b64encode(chunk.encode())
                yield 'data: ' + encoded.decode() + '\n\n'

        cherrypy.response.headers['Content-Type'] = 'text/event-stream'
        cherrypy.response.headers['Cache-Control'] = 'no-cache'
        return content(stderr_reader)

    stderr._cp_config = {'response.stream': True}
