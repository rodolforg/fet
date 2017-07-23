#-------------------------------------------------
#
# Project created by QtCreator 2016-09-18T01:50:00
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_randomknuthtest
CONFIG   += console testcase c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_randomknuthtest.cpp \
    ../../../src/engine/randomknuth.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

DESTDIR = ../../

OBJECTS_DIR = ../../../tmp/tests
UI_DIR = ../../../tmp/tests
MOC_DIR = ../../../tmp/tests
RCC_DIR = ../../../tmp/tests

INCLUDEPATH += ../../../src
