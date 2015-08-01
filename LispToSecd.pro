#-------------------------------------------------
#
# Project created by QtCreator 2015-07-25T12:59:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LispToSecd
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CXXFLAGS      += -std=c++11

TEMPLATE = app


SOURCES += main.cpp \
    parser.cpp \
    Simplify/function.cpp \
    Simplify/atom.cpp

HEADERS += \
    parser.h \
    exception.h \
    Simplify/function.h \
    Simplify/atom.h \
    Simplify/buffer.h

DISTFILES += \
    source.lisp
