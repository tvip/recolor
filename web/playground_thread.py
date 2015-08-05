#!/usr/bin/env python3

import typing
import numbers
import decimal
import fractions

import typecheck as tc
import warnings
import threading
import time
from builtins import int


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


@tc.typecheck
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

'''
@tc.typecheck
def type_experiment(x: (lambda x: isinstance(x, numbers.Number))):
    pass
'''


@tc.typecheck
def type_experiment(x: tc.optional(int)):
    pass


def p() -> None:
    print('hello')

if __name__ == '__main__':
    # events()

    a = p()
    print(a)

    type_experiment(None)

    print(type_experiment)
    print(tc.typecheck)

    '''
    type_experiment(12)
    type_experiment(12.4)
    type_experiment(1+2j)
    type_experiment(decimal.Decimal('12.3'))
    type_experiment(fractions.Fraction(1, 3))
    '''
    #warnings.warn("deprecated", DeprecationWarning)
    print(f2.__annotations__)
    conditions()
