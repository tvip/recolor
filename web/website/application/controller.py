# -*- coding: utf-8 -*-


import datetime

import cherrypy


class Index:
  news = None
  user = None

  def __init__(self):
    self.news = News()
    self.user = User()

  @cherrypy.tools.template
  def index(self):
    pass

  @cherrypy.expose
  def broken(self):
    raise RuntimeError('Pretend something has broken')


class User:
  @cherrypy.tools.template
  def profile(self):
    pass


class News:
  _list = [
    {'id': 0, 'date': datetime.datetime(2014, 11, 16), 'title': 'Bar', 'text': 'Lorem ipsum'},
    {'id': 1, 'date': datetime.datetime(2014, 11, 17), 'title': 'Foo', 'text': 'Ipsum lorem'}
  ]

  @cherrypy.tools.template
  def list(self):
    return {'list': self._list}

  @cherrypy.tools.template
  def show(self, id):
    return {'item': self._list[int(id)]}


def errorPage(status, message, **kwargs):
  return cherrypy.tools.template._engine.get_template('page/error.html').render()
