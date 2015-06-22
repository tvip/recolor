import cherrypy
import subprocess


class Recolor(object):

  @cherrypy.expose
  def index(self):
    orig_color = 'orig'
    res_color = 'green'

    subprocess.call([
      'utils/bin/recolor-tool',
      'test-data/' + res_color + '/matrix.txt',
      '-in', 'test-data/' + orig_color + '/tvip_light/resources',
      '-out', 'test-data/' + res_color + '/tvip_light/resources',
      '-xpath', '//image[@file]', '-xattr', 'file',
      '-xml', 'test-data/' + orig_color + '/tvip_light/resources.xml'
    ])

    return "Done"
