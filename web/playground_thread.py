#!/usr/bin/env python3

import typecheck
import warnings
import threading
import time


def f1(e1, e2):
    with open('dummy.cpp') as file:
        while True:
            # time.sleep(0.1)
            e1.wait()
            line = file.readline()
            # if not line:
            #    e2.set()
            #    break
            print(threading.current_thread(), line.rstrip())
            # time.sleep(0.1)
            # e1.clear()
            # e2.set()
            # break


def events():
    e = threading.Event()

    t1 = threading.Thread(target=f1, args=(e, e))
    t2 = threading.Thread(target=f1, args=(e, e))
    t3 = threading.Thread(target=f1, args=(e, e))
    t4 = threading.Thread(target=f1, args=(e, e))

    t1.start()
    t2.start()
    t3.start()
    t4.start()

    e.set()
    # time.sleep(1)
    e.clear()


@typecheck.typecheck
def f2(c: threading.Condition):
    '''
        Exploit the workers by hanging on to outdated imperialist dogma which
        perpetuates the economic and social differences in our society.

        @type c: str
        @param c: str to repress.
    '''
    with open('dummy.cpp') as file:
        while True:
            c.acquire()
            line = file.readline()
            print(threading.current_thread(), line.rstrip())
            break


def conditions():
    c = threading.Condition()
    t1 = threading.Thread(target=f2, args=(c,))
    t1.start()


if __name__ == '__main__':
    # events()
    warnings.warn("deprecated", DeprecationWarning)
    print(f2.__annotations__)
    conditions()
