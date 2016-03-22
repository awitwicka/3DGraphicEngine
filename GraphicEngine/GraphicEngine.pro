#-------------------------------------------------
#
# Project created by QtCreator 2016-02-26T14:08:44
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphicEngine
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    widget.cpp \
    cadobject.cpp \
    torus.cpp \
    marker.cpp

HEADERS  += mainwindow.h \
    widget.h \
    cadobject.h \
    torus.h \
    marker.h

FORMS    += mainwindow.ui
