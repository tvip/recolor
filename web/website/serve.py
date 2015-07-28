#!/usr/bin/env python
# -*- coding: utf-8 -*-


from application import bootstrap

bootstrap()


# debugging purpose, e.g. run with PyDev debugger
if __name__ == '__main__':
    import cherrypy

    cherrypy.engine.signals.subscribe()
    cherrypy.engine.start()
    cherrypy.engine.block()
