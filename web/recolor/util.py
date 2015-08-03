import os
import errno
import threading
import subprocess
import shutil
import re


def make_sure_path_exists(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise


# Удаляет все файлы в директории, подходящие по regexp
def purge(directory, pattern):
    for f in os.listdir(directory):

        if re.search(pattern, f):
            path = os.path.join(directory, f)

            if os.path.isdir(path):
                shutil.rmtree(path)

            elif os.path.isfile(path):
                os.remove(path)


class AsynchronousFileReader(threading.Thread):

    def __init__(self, fd):
        assert callable(fd.readline)
        threading.Thread.__init__(self)
        self._fd = fd
        self.log = list()
        self.event = threading.Condition()

    def __iter__(self):
        self.event.acquire()
        message = True

        i = 0
        while i < len(self.log):
            message = self.log[i]

            if message:
                yield self.log[i]

            i = i + 1

        self.event.release()

        while message:
            self.event.acquire()
            self.event.wait()

            while i < len(self.log):
                message = self.log[i]

                if message:
                    yield self.log[i]

                i = i + 1

            self.event.release()

    def run(self):
        while True:
            chunk = self._fd.readline()

            self.event.acquire()

            self.log.append(chunk)
            print(type(chunk), len(chunk))

            self.event.notify_all()
            self.event.release()

            if not chunk:
                break


class ProcLogger(threading.Thread):

    def __init__(self, args):
        threading.Thread.__init__(self)

        self._proc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self._onfinish = None

        self.threads = (
            AsynchronousFileReader(self._proc.stdout),
            AsynchronousFileReader(self._proc.stderr)
        )

    def run(self):
        for thread in self.threads:
            thread.start()

        for thread in self.threads:
            thread.join()

        self._proc.stdout.close()
        self._proc.stderr.close()

        if self._onfinish is not None:
            self._onfinish()

    def onFinish(self, on_finish):
        # FIXME: по-идее здесь нужен lock
        self._onfinish = on_finish
        if self._proc.poll() is not None:  # Нет returncode, значит ещё работает
            on_finish()
