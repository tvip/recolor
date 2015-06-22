#!/usr/bin/env python3

import cherrypy


class Root(object):
  @cherrypy.expose
  def index(self):
    return "Hello World!"

import json
with open('myapp-config.json', 'w') as outfile:
  json.dump(
    {'/': {'tools.gzip.on': True}},
    outfile, sort_keys=True, indent=2
  )
with open('myapp-config.json') as infile:
  myapp_conf = json.load(infile)

if __name__ == '__main__':
  cherrypy.quickstart(Root(), '/myapp', myapp_conf)

