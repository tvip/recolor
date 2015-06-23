import requests
import threading


class AsyncRequest(threading.Thread):

  def run(self):
    url = 'http://127.0.0.1:9090/recolor'
    print(url)

    s = requests.Session()
    r = s.get(url)
    print(r)


def parallel(n):
  pool = []
  for i in range(0, n):
    r = AsyncRequest()
    r.start()
    pool.append(r)

  for t in pool:
    t.join()
