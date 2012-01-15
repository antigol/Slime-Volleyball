#-------------------------------------------------
#
# Project created by QtCreator 2012-01-09T20:53:25
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui


TARGET = slimevolleyball-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cmdlineparser.cpp \
    server.cpp \
    world.cpp

HEADERS += \
    cmdlineparser.h \
    server.h \
    world.h \
    quartic/quartic_yacfraid.hpp \
    quartic/quartic_neumark.hpp \
    quartic/quartic_ferrari.hpp \
    quartic/quartic_error.hpp \
    quartic/quartic_descartes.hpp \
    quartic/quartic.hpp \
    quartic/quadratic.hpp \
    quartic/cubic.hpp
