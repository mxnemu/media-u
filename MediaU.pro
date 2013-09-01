#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T18:40:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaU
TEMPLATE = app

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


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    library.cpp \
    moviefile.cpp \
    mainpage.cpp \
    config.cpp \
    pagefactory.cpp \
    directoryscanner.cpp \
    mediascanner.cpp \
    tvshowscanner.cpp \
    tvshow.cpp \
    season.cpp \
    systemutils.cpp \
    page.cpp \
    malclient.cpp \
    nwutils.cpp \
    filedownloadthread.cpp \
    curlresult.cpp \
    utils.cpp \
    tvshowlistwidget.cpp \
    libraryfilter.cpp \
    tvshowpage.cpp \
    moebooruclient.cpp \
    filefilterscanner.cpp \
    videoplayer.cpp \
    mplayer.cpp \
    omxplayer.cpp \
    mainbackgroundwidget.cpp \
    metadataparser.cpp \
    metadataparseravconv.cpp

HEADERS  += mainwindow.h \
    server.h \
    library.h \
    moviefile.h \
    mainpage.h \
    config.h \
    pagefactory.h \
    directoryscanner.h \
    mediascanner.h \
    tvshowscanner.h \
    tvshow.h \
    season.h \
    systemutils.h \
    page.h \
    malclient.h \
    nwutils.h \
    filedownloadthread.h \
    curlresult.h \
    utils.h \
    tvshowlistwidget.h \
    libraryfilter.h \
    tvshowpage.h \
    moebooruclient.h \
    filefilterscanner.h \
    videoplayer.h \
    mplayer.h \
    omxplayer.h \
    mainbackgroundwidget.h \
    metadataparser.h \
    metadataparseravconv.h

FORMS    += mainwindow.ui \
    mainpage.ui \
    tvshowlistwidget.ui \
    tvshowpage.ui
