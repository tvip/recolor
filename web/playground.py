import threading
import queue
import time


class Message:
  def __init__(self, _type, _data=''):
    self.type = _type
    self.data = _data


class T(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self.eventQueue = queue.Queue()

  def run(self):
    for i in range(3):
      # time.sleep(1)
      # print('post T' + str(i))
      self.eventQueue.put(Message('message', 'T' + str(i)))
      # time.sleep(0.3)
      # print('proc T' + str(i))

    self.eventQueue.put(Message('eof'))


def gen():
  t = T()
  t.start()

  while True:
    message = t.eventQueue.get(block=True)

    if message.type == 'eof':
      break
    elif message.type == 'message':
      # time.sleep(2)
      yield message.data


for s in gen():
  print(s)
