Картинки читаются с помощью libdevil, проверял только с PNG
`sudo apt-get install libdevil-dev libglm-dev`


Запуск веб сервера
---------------------------

Требуется python3 с установленными cherrypy и jinja2
`sydo apt-get install python3 && sudo pip3 install cherrypy jinja2`

Для начала нужно собрать консольную тулзу recolor-tool
`./web/build.py`

После того как сборка завершена можно запустить сервер
`./web/server.py`

TODO: Сделать чтобы сервер можно было запускать из любой директории
