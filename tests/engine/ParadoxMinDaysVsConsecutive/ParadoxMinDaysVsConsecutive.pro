QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include( ../GeneratePre/GeneratePre.pri )

SOURCES +=  tst_paradox_mindays_consecutive.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\" FET_COMMAND_LINE

INCLUDEPATH += $$PWD/../../../src/engine $$PWD/../../../src/interface
