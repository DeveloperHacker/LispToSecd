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
    Tree/tree.cpp \
    Simplify/simplify.cpp \
    Parser/parser.cpp

HEADERS += \
    exception.h \
    parser.h \
    Tree/tree.h \
    Simplify/simplify.h \
    Parser/parser.h

DISTFILES += \
    ../Source/source.lisp \
    ../Source/source.secd \
    ../Source/source.log
