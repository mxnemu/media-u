#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T18:40:30
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaU
TEMPLATE = app

OTHER_FILES += libs.pri

include(libs.pri)
include(common.pri)
include(gui.pri)
include(online.pri)

