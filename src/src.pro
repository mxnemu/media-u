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

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

include(libs.pri)
include(common.pri)
include(gui.pri)
include(online.pri)

