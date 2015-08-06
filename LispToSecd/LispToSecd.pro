#-------------------------------------------------
#
# Project created by QtCreator 2015-08-05T11:48:19
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LispToSecd
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CXXFLAGS      += -std=c++11
QMAKE_LFLAGS    += -static

TEMPLATE = app


SOURCES += main.cpp \
    Simplify/atom.cpp \
    parser.cpp \
    Simplify/function.cpp \
    Simplify/buffer.cpp \
    Tree/tree.cpp

HEADERS += \
    Simplify/atom.h \
    Simplify/buffer.h \
    Simplify/function.h \
    exception.h \
    parser.h \
    Tree/tree.h
