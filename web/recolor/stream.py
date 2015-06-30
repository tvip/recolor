import cherrypy
import base64
import threading


class Stream(object):
  @cherrypy.expose()
  def stdout(self):
    def content():
      message = '{} {} Hello World'.format(str(threading.current_thread()), cherrypy.session.id)
      encoded = base64.b64encode(message.encode())
      yield 'data: ' + encoded.decode() + '\n\n'

    cherrypy.response.headers['Content-Type'] = 'text/event-stream'
    cherrypy.response.headers['Cache-Control'] = 'no-cache'
    return content()

  stdout._cp_config = {'response.stream': True}
