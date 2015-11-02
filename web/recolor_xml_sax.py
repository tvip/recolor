#!/usr/bin/env python3

import os
import xml.sax
import xml.sax.xmlreader
import xml.sax.saxutils


with open('/home/ik/test.xml', 'w') as output:
    writer = xml.sax.saxutils.XMLGenerator(out=output, encoding='utf-8')
    writer.startDocument()
    writer.endDocument()


with open('/home/ik/test.xml', 'rb') as input_stream:
    source = xml.sax.xmlreader.InputSource(input_stream)
    # source.setByteStream(input_stream)
    print('encoding', source.getEncoding())


class ContentHandler(xml.sax.ContentHandler):
    def startElement(self, name, attrs):
        for attr, value in attrs.items():
            # print(attr, value, sep=': ')
            pass


class ErrorHandler(xml.sax.ErrorHandler):

    def error(self, exception):
        print(exception)

    def fatalError(self, exception):
        print(exception)

    def warning(self, exception):
        print(exception)


parser = xml.sax.make_parser()
parser.setContentHandler(ContentHandler())
parser.setErrorHandler(ErrorHandler())


def recolor_xml(fname):
    print(fname)
    with open(fname, 'rb') as input_stream:
        source = xml.sax.xmlreader.InputSource()
        source.setByteStream(input_stream)
        xml.sax.parse(source, ContentHandler())
    pass

if __name__ == '__main__':

    for root, dirs, files in os.walk('/tvip/tvip_stb/tvip/themes'):
        for file in files:
            if '.xml' == os.path.splitext(file)[-1]:
                recolor_xml(os.path.join(root, file))
