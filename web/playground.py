import threading
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

t = T()
t.start()
for s in t:
  print(s)
