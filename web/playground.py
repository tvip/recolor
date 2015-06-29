import threading
import multiprocessing
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
