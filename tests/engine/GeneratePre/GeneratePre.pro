#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T22:04:11
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_generatepretest
CONFIG   += console testcase
CONFIG   -= app_bundle
CONFIG   += debug

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += tst_generatepretest.cpp \
    ../../../src/engine/generate_pre.cpp \
    ../../../src/engine/activity.cpp \
    ../../../src/engine/activitytag.cpp \
    ../../../src/engine/building.cpp \
    ../../../src/engine/lockunlock.cpp \
    ../../../src/engine/room.cpp \
    ../../../src/engine/rules.cpp \
    ../../../src/engine/solution.cpp \
    ../../../src/engine/spaceconstraint.cpp \
    ../../../src/engine/studentsset.cpp \
    ../../../src/engine/subject.cpp \
    ../../../src/engine/teacher.cpp \
    ../../../src/engine/timeconstraint.cpp \
    ../../../src/engine/timetable_defs.cpp \
    ../../../src/engine/xmllog.cpp \
    ../../../src/engine/constraints/mindaysbetweenactivities.cpp \
    ../../../src/engine/constraints/maxdaysbetweenactivities.cpp \
    ../../../src/engine/constraints/mingapsbetweenactivities.cpp \
    ../../../src/engine/messageboxes.cpp \
    ../../../src/engine/randomknuth.cpp \
    ../../../src/interface/longtextmessagebox.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\" FET_COMMAND_LINE

HEADERS += \
	../../../src/engine/activity.h \
	../../../src/engine/solution.h \
	../../../src/engine/timetable.h \
	../../../src/engine/rules.h \
	../../../src/engine/generate_pre.h \
	../../../src/engine/timeconstraint.h \
	../../../src/engine/spaceconstraint.h \
	../../../src/engine/timetable_defs.h \
	../../../src/engine/studentsset.h \
	../../../src/engine/teacher.h \
	../../../src/engine/subject.h \
	../../../src/engine/activitytag.h \
	../../../src/engine/room.h \
	../../../src/engine/building.h \
	../../../src/engine/lockunlock.h \
#	engine/matrix.h \
#	engine/randomknuth.h \
	../../../src/engine/messageboxes.h \
	../../../src/engine/xmllog.h \
	\
	../../../src/interface/longtextmessagebox.h

DESTDIR = ../../

OBJECTS_DIR = ../../../tmp/tests
UI_DIR = ../../../tmp/tests
MOC_DIR = ../../../tmp/tests
RCC_DIR = ../../../tmp/tests

INCLUDEPATH += $$PWD/../../../src/engine $$PWD/../../../src/interface
