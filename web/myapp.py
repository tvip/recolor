#!/usr/bin/env python3

import cherrypy


class Root(object):
  @cherrypy.expose
  def index(self):
    return "Hello World!"

#######################################################################

import json
with open('myapp-config.json', 'w') as outfile:
  json.dump(
    {'/': {'tools.gzip.on': True}},
    outfile, sort_keys=True, indent=2
  )
with open('myapp-config.json') as infile:
  myapp_conf = json.load(infile)

#######################################################################

import pickle
with open('myapp-config.p', 'wb') as outfile:
  print(pickle.dumps(myapp_conf))
  pickle.dump(myapp_conf, outfile)

#######################################################################


if __name__ == '__main__':
  cherrypy.quickstart(Root(), '/myapp', 'myapp.conf')

