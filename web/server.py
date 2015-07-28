#!/usr/bin/env python3

import cherrypy
import jinja2

import os

import band
import myapp
import recolor
import recolor.stream


def setup_config(template_fname, result_fname, env):
    with open(template_fname) as config:
        template = jinja2.Template(config.read())
        res_config = template.render(env)

    with open(result_fname, 'w') as config:
        config.write(res_config)

    return result_fname


if __name__ == '__main__':
    cherrypy.config.update('server.conf')
    cherrypy.tree.mount(band.Band(), '/band')

    cherrypy.tree.mount(
        myapp.Root(), '/myapp',
        setup_config(
            template_fname='myapp.conf',
            result_fname='var/myapp.conf',
            env={
                'staticdir': os.path.abspath(os.getcwd())
            }
        ))

    cherrypy.tree.mount(
        recolor.Recolor(), '/recolor',
        setup_config(
            template_fname='recolor/root.conf',
            result_fname='var/recolor.conf',
            env={
                'staticdir': os.path.join(os.path.abspath(os.getcwd()), 'recolor')
            }
        ))

    cherrypy.tree.mount(
        recolor.stream.Stream(), '/streaming',
        setup_config(
            template_fname='recolor/root.conf',
            result_fname='var/recolor.conf',
            env={
                'staticdir': os.path.join(os.path.abspath(os.getcwd()), 'recolor')
            }
        ))

    cherrypy.engine.start()
    cherrypy.engine.block()
