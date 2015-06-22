import cherrypy


class Root(object):
  @cherrypy.expose(['abc'])
  def index(self):
    cherrypy.log('global log')
    cherrypy.request.app.log('app log')
    return "Hello World!"
