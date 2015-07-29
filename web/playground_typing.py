import typing
import typecheck as tc
import inspect


def greeting(name) -> str:
    return 'Hello, {}'.format(name)

print(greeting(12))


import enum


class Color(enum.IntEnum):
    green = 2
    blue = 3

print(list(Color))

print(type(Color.__members__))

print(Color.green == 2)

Animal = enum.Enum('Animal', 'ant bee cat dog')
print(list(Animal))


@tc.typecheck
def with_color(c: Color):
    print(c)

with_color(Color(2))


class TestCase:
    pass
class MyCheckers(TestCase):
    pass
class AddressTest:
    pass


def classname_contains_Test(arg):
    print("{}({}:{})".format(inspect.stack()[0][3], repr(arg), type(arg).__name__))
    return type(arg).__name__.find("Test") >= 0


@tc.typecheck
def no_tests_please(arg: tc.none(classname_contains_Test)): pass


no_tests_please("stuff")        # OK
#no_tests_please(TestCase())     # Wrong: not wanted here
#no_tests_please(MyCheckers())   # Wrong: superclass not wanted here
#no_tests_please(AddressTest())  # Wrong: suspicious class name



print(type(MyCheckers()).__name__)


@tc.typecheck
def foo_callable(func: callable, msg: str):
    func(msg)

foo_callable(print, 'abc')






