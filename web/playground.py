import threading
import multiprocessing
import subprocess
import queue
import time


class T(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self.eventQueue = queue.Queue()

  def run(self):
    for i in range(3):
      # time.sleep(1)
      # print('post T' + str(i))
      self.eventQueue.put('T' + str(i))
      # time.sleep(0.3)
      # print('proc T' + str(i))

    self.eventQueue.put(None)

  def __iter__(self):
    while True:
      message = self.eventQueue.get(block=True)
      if message:
        # time.sleep(2)
        yield message
      else:
        break


'''
t = T()
t.start()
for s in t:
  print(s)
'''

'''
def count(n):
  while n > 0:
    n -= 1

start_time = time.time()
count(10000000)
# print("--- {} seconds ---".format(time.time() - start_time))
count(10000000)
print("--- {} seconds ---".format(time.time() - start_time))

start_time = time.time()
t1 = threading.Thread(target=count, args=(10000000,))
t1.start()
t2 = threading.Thread(target=count, args=(10000000,))
t2.start()
t1.join()
t2.join()
print("--- {} seconds ---".format(time.time() - start_time))

start_time = time.time()
p1 = multiprocessing.Process(target=count, args=(10000000,))
p1.start()
p2 = multiprocessing.Process(target=count, args=(10000000,))
p2.start()
p1.join()
p2.join()
print("--- {} seconds ---".format(time.time() - start_time))
'''

'''
def f(q):
  q.put('X ' * 10000)


if __name__ == '__main__':
  start_time = time.time()

  queue = multiprocessing.Queue()
  p = multiprocessing.Process(target=f, args=(queue,))
  p.start()
  # p.join()  # this deadlocks
  obj = queue.get()
  
  print(obj)

  print("--- {} seconds ---".format(time.time() - start_time))
'''


def f(e1, e2):
  with open('playground.py') as file:
    while True:
      e1.wait()
      e1.clear()
      line = file.readline()
      if not line:
        e2.set()
        break
      print(threading.current_thread(), line.rstrip())
      time.sleep(0.01)
      e2.set()


class AsynchronousFileReader(threading.Thread):
  def __init__(self, fd):
    assert callable(fd.readline)
    threading.Thread.__init__(self)
    self._fd = fd
    self.queue = queue.Queue()

  def __iter__(self):
    self.start()
    while True:
      message = self.queue.get(block=True)
      if message:
        yield message
      else:
        break

  def run(self):
    while True:
      chunk = self._fd.readline()
      if not chunk:
        self.queue.put('')
        break
      self.queue.put( '{} {}'.format( str(threading.current_thread()), chunk ) )


if __name__ == '__main__':
  print(threading.current_thread(), 'main')

  e = threading.Event()

  t1 = threading.Thread(target=f, args=(e, e))
  t2 = threading.Thread(target=f, args=(e, e))

  t1.start()
  t2.start()

  e.set()

  ##########################################

  proc = subprocess.Popen([
    'utils/bin/dummy'
  ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  stdout_reader = AsynchronousFileReader(proc.stdout)
  stderr_reader = AsynchronousFileReader(proc.stderr)

  def output(reader):
    for msg in reader:
      print(msg)

  o1 = threading.Thread(target=output, args=(stdout_reader,))
  o2 = threading.Thread(target=output, args=(stderr_reader,))

  o1.start()
  o2.start()
