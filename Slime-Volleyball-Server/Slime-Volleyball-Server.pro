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
    world.h
