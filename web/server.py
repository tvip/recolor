#!/usr/bin/env python3

import cherrypy

import band
import myapp
import recolor


if __name__ == '__main__':
  cherrypy.config.update('server.conf')
  cherrypy.tree.mount(myapp.Root(), '/myapp', 'myapp.conf')
  cherrypy.tree.mount(band.Band(), '/band')
  cherrypy.tree.mount(recolor.Recolor(), '/recolor', 'recolor/root.conf')
  cherrypy.engine.start()
  cherrypy.engine.block()
