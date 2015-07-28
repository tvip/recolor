# -*- coding: utf-8 -*-

__author__ = 'ikoznov'


import sys

print("Module", __name__)
print("sys.path", sys.path)


from jinja2 import Template

template = Template('Hello {{ name }}!')
print(template.render(name='John Doe'))
print()


from jinja2 import Environment, PackageLoader

env = Environment(loader=PackageLoader('recolor', 'templates'))
template = env.get_template('mytemplate.html')

print(template.render(the='variables', go='here'))
print()

print(env.get_template('base.html'))
print()


import recolor.fibo

recolor.fibo.fib(1000)
print()


import cherrypy


class HelloWorld(object):

    @cherrypy.expose()
    def index(self):
        return "Hello World!"

if __name__ == '__main__':
    cherrypy.quickstart(HelloWorld())
