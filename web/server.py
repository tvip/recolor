#!/usr/bin/env python3

import cherrypy
import jinja2
import jinja2tool

import os

import recolor


def setup_config(template_fname, result_fname, env):
    with open(template_fname) as config:
        template = jinja2.Template(config.read())
        res_config = template.render(env)

    with open(result_fname, 'w') as config:
        config.write(res_config)

    return result_fname


if __name__ == '__main__':
    cherrypy.tools.template = jinja2tool.Jinja2Tool()
    cherrypy.config.update('server.conf')

    cherrypy.tree.mount(
        recolor.Recolor(), '/recolor',
        setup_config(
            template_fname='recolor/root.conf',
            result_fname='var/recolor.conf',
            env={
                'staticdir': os.path.join(os.path.abspath(os.getcwd()), 'recolor')
            }
        ))

    cherrypy.engine.start()
    cherrypy.engine.block()
