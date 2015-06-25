import os
import errno
import threading
import queue


def make_sure_path_exists(path):
  try:
    os.makedirs(path)
  except OSError as exception:
    if exception.errno != errno.EEXIST:
      raise


class AsynchronousFileReader(threading.Thread):
  def __init__(self, fd):
    assert callable(fd.readline)
    threading.Thread.__init__(self)
    self._fd = fd
    self._eventQueue = queue.Queue()

  def __iter__(self):
    while True:
      message = self._eventQueue.get(block=True)
      if message:
        yield message
      else:
        break

  def run(self):
    while True:
      chunk = self._fd.readline()
      if not chunk:
        break
      self._eventQueue.put(chunk)
    self._eventQueue.put(None)
