mount -t nfs -O uid=1000,iocharset=utf-8 10.170.253.147:/home/ik/workspace/tvip/tvip_stb/tvip/themes /home/tvip/themes

cd /tvip/tvip_stb && git checkout tvip && /home/ik/workspace/recolor/web/recolor_xml.py /home/ik/workspace/recolor/src/app/res/green.txt /tvip/tvip_stb/tvip/themes && /home/ik/workspace/recolor/web/build/Staging/bin/recolor-tool /home/ik/workspace/recolor/src/app/res/green.txt -in /tvip/tvip_stb/tvip/themes/tvip_light/resources -out /tvip/tvip_stb/tvip/themes/tvip_light/resources -xpath //image[@file] -xattr file -xml /tvip/tvip_stb/tvip/themes/tvip_light/resources.xml

cd /tvip/tvip_stb && git checkout tvip && /home/ik/workspace/recolor/web/recolor_xml.py /home/ik/workspace/recolor/src/app/res/green.txt /tvip/tvip_stb/tvip/themes && /home/ik/workspace/recolor/web/recolor_img.py /home/ik/workspace/recolor/src/app/res/green.txt /tvip/tvip_stb/tvip/themes

/home/ik/workspace/recolor/web/build/Staging/bin/recolor-tool /home/ik/workspace/recolor/src/app/res/green.txt -img /tvip/tvip_stb/tvip/themes/tvip_light/resources/tvip_logo.png /tvip/tvip_stb/tvip/themes/tvip_light/resources/tvip_logo.png

cd /tvip/tvip_stb && git checkout tvip && /home/ik/workspace/recolor/web/recolor_xml.py /tvip/tvip_stb/tvip/themes/themes/green/matrix.txt /tvip/tvip_stb/tvip/themes/themes/green && /home/ik/workspace/recolor/web/recolor_img.py /tvip/tvip_stb/tvip/themes/themes/green/matrix.txt /tvip/tvip_stb/tvip/themes/themes/green

cd /tvip/tvip_stb && git checkout tvip && /home/ik/workspace/recolor/web/recolor_xml.py /tvip/tvip_stb/tvip/themes/themes/coffee/matrix.txt /tvip/tvip_stb/tvip/themes/themes/coffee && /home/ik/workspace/recolor/web/recolor_img.py /tvip/tvip_stb/tvip/themes/themes/coffee/matrix.txt /tvip/tvip_stb/tvip/themes/themes/coffee
