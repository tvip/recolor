#!/usr/bin/env python3

import cherrypy
import jinja2
import jinja2tool

import os

import recolor_server_path as path
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
    cherrypy.config.update(os.path.join(path.server_dir, 'server.conf'))

    cherrypy.tree.mount(
        recolor.Recolor(), '/recolor',
        setup_config(
            template_fname='recolor/root.conf',
            result_fname='var/recolor.conf',
            env={
                'staticdir': os.path.join(path.server_dir, 'recolor'),
                'access_file': os.path.join(path.server_dir, 'var', 'recolor.access'),
                'error_file': os.path.join(path.server_dir, 'var', 'recolor.error')
            }
        ))

    cherrypy.engine.start()
    cherrypy.engine.block()
