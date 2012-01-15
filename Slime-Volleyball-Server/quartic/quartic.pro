#-------------------------------------------------
#
# Project created by QtCreator 2012-01-15T15:57:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = quartic
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    quartic_yacfraid.hpp \
    quartic_neumark.hpp \
    quartic_ferrari.hpp \
    quartic_error.hpp \
    quartic_descartes.hpp \
    quartic.hpp \
    quadratic.hpp \
    cubic.hpp

SOURCES += \
    quartic.cpp
