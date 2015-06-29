import os
import errno
import threading
import queue
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
  def __init__(self, fd, eventQueue):
    assert callable(fd.readline)
    threading.Thread.__init__(self)
    self._fd = fd
    self._eventQueue = eventQueue

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
