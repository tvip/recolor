import cherrypy
import time


class Root(object):

    @cherrypy.expose(['abc'])
    def index(self):
        cherrypy.log('global log')
        cherrypy.request.app.log('app log')
        time.sleep(1)
        return "Hello World!"
