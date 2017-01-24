#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T17:59:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vis
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    CImg.h \
    colorDesc.h \
    struct.h \
    functions.h \
    types.h \
    typedef.h \
    node.h \
    Mtree.h \
    fun.h

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

PKGCONFIG += opencv

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11

LIBS    += -lX11 -lpthread -lpqxx -lpq

LIBS    += -L/usr/local/pgsql/lib
