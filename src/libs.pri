#nixos/systemd opengl for some shitty reason not in the default library path
LIBPATH += /run/opengl-driver/lib/

# qhttp
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/qhttpserver/src
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/qhttpserver/http-parser
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/qhttpserver
LIBPATH += $$_PRO_FILE_PWD_/libs/qhttpserver/lib
LIBS += -lqhttpserver

# libcurl
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/curl/include/
LIBPATH += $$_PRO_FILE_PWD_/libs/curl/lib/.libs/
LIBS += -lcurl

# noslib
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/noslib/src/
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/noslib/
LIBPATH += $$_PRO_FILE_PWD_/libs/noslib/
LIBS += -lN0Slib
