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
    marker.cpp \
    cursor.cpp \
    constants.cpp \
    Spline/bezier.cpp \
    Spline/segment.cpp \
    Spline/curveC2.cpp \
    Spline/cadmarkerobject.cpp \
    Spline/bsinterpolation.cpp \
    Patch/bezierplane.cpp \
    Patch/bicubicsegment.cpp \
    Patch/cadsplinepatch.cpp \
    Patch/bsplineplane.cpp \
    cadloader.cpp \
    Patch/gapfilling.cpp \
    Patch/gregorypatch.cpp \
    Spline/intersection.cpp

HEADERS  += mainwindow.h \
    widget.h \
    cadobject.h \
    torus.h \
    marker.h \
    cursor.h \
    constants.h \
    Spline/bezier.h \
    Spline/segment.h \
    Spline/curveC2.h \
    Spline/cadmarkerobject.h \
    Spline/bsinterpolation.h \
    Patch/bezierplane.h \
    Patch/bicubicsegment.h \
    Patch/cadsplinepatch.h \
    Patch/bsplineplane.h \
    cadloader.h \
    Patch/gapfilling.h \
    Patch/gregorypatch.h \
    Spline/intersection.h

FORMS    += mainwindow.ui
