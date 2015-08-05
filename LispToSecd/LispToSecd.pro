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

TEMPLATE = app


SOURCES += main.cpp \
    Simplify/atom.cpp \
    Simplify/function.cpp \
    Tree/Function.cpp \
    Tree/leaf.cpp \
    Tree/sexpression.cpp \
    parser.cpp

HEADERS += \
    Simplify/atom.h \
    Simplify/buffer.h \
    Simplify/function.h \
    Tree/Function.h \
    Tree/leaf.h \
    Tree/sexpression.h \
    exception.h \
    parser.h
