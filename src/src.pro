#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T18:40:30
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaU
TEMPLATE = app

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

include(libs.pri)
include(data.pri)
include(filesystem.pri)
include(utils.pri)
include(videoutils.pri)
include(gui.pri)
include(online.pri)
include(player.pri)
include(server.pri)

SOURCES += main.cpp
