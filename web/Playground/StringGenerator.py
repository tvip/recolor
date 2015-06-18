__author__ = 'ikoznov'

import random
import string

import cherrypy

class StringGenerator(object):
  cherrypy.expose()
  def index(self):
    return "Hello World!"

  cherrypy.expose()
  def generate(self):
    return ''.join(random.sample(string.hexdigits, 8))
