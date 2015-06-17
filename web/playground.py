# -*- coding: utf-8 -*-

__author__ = 'ikoznov'



import sys

print("Module",  __name__)
print("sys.path", sys.path)



from jinja2 import Template

template = Template('Hello {{ name }}!')
print(template.render(name='John Doe'))



from jinja2 import Environment, PackageLoader

env = Environment(loader=PackageLoader('recolor', 'templates'))
template = env.get_template('mytemplate.html')
print(template.render(the='variables', go='here'))

import recolor.fibo

recolor.fibo.fib(1000)


