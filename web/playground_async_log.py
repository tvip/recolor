import subprocess
import threading
import datetime
import time


class AsynchronousFileReader(threading.Thread):

    def __init__(self, fd):
        assert callable(fd.readline)
        threading.Thread.__init__(self)
        self._fd = fd
        self._end = False
        self.log = list()
        self.event = threading.Condition()

    def __iter__(self):
        pass

    def run(self):
        while True:
            chunk = self._fd.readline()
            self.event.acquire()
            
            self.log.append(chunk)

            self.event.notify_all()
            self.event.release()

            if not chunk:
                break


class ProcLogger(threading.Thread):

    def _output(self, reader):
        for msg in reader:
            print(datetime.datetime.now(), msg)

    def __init__(self):
        threading.Thread.__init__(self)

        self._proc = subprocess.Popen([
            'utils/bin/dummy'
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        self._stdout_reader = AsynchronousFileReader(self._proc.stdout)
        self._stderr_reader = AsynchronousFileReader(self._proc.stderr)
        
        self._stdout_reader.start()
        self._stderr_reader.start()
        
        self._stdout_reader.join()
        self._stderr_reader.join()
        
        for line in self._stdout_reader.log:
            print(line)
            
        for line in self._stderr_reader.log:
            print(line)

    def run(self):
        self._proc.stdout.close()
        self._proc.stderr.close()

'''
def dummy():
    while True:
        pass
'''

if __name__ == '__main__':
    if None:
        print(__name__)
    '''
    l = [i for i in range(10)]
    c = threading.Condition()
    
    def f():
        i = 0
        while True:
            
            print(l[i])
            i = i + 1
    
    '''
    p = ProcLogger()
    p.start()
    p.join()
    
        
    '''
    proc = subprocess.Popen([
        'utils/bin/dummy'
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    stdout_reader = AsynchronousFileReader(proc.stdout)
    stderr_reader = AsynchronousFileReader(proc.stderr)

    def output(reader):
        for msg in range(10):
            print('{} {}'.format(str(threading.current_thread()), msg))

    o1 = threading.Thread(target=output, args=(stdout_reader,))
    o2 = threading.Thread(target=output, args=(stderr_reader,))

    o1.start()
    o2.start()

    o1.join()
    o2.join()

    proc.stdout.close()
    proc.stderr.close()
    '''

    '''
    threads = [threading.Thread(target=dummy) for i in range(200)]
    print(threads)
    
    for t in threads:
        t.start()
        
    for t in threads:
        t.join()
    
    '''
