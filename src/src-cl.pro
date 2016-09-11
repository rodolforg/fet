SOURCES += \
	engine/timetableexport.cpp \
	engine/activity.cpp \
	engine/solution.cpp \
	engine/timetable.cpp \
	engine/rules.cpp \
	engine/generate_pre.cpp \
	engine/timeconstraint.cpp \
	engine/spaceconstraint.cpp \
	engine/studentsset.cpp \
	engine/teacher.cpp \
	engine/subject.cpp \
	engine/activitytag.cpp \
	engine/room.cpp \
	engine/building.cpp \
	engine/timetable_defs.cpp \
	engine/generate.cpp \
	engine/lockunlock.cpp \
	engine/messageboxes.cpp \
	\
	interface/centerwidgetonscreen.cpp \
	interface/fet.cpp \
	interface/fetmainform.cpp \
	interface/longtextmessagebox.cpp

HEADERS += \
	engine/timetableexport.h \
	engine/activity.h \
	engine/solution.h \
	engine/timetable.h \
	engine/rules.h \
	engine/generate_pre.h \
	engine/timeconstraint.h \
	engine/spaceconstraint.h \
	engine/timetable_defs.h \
	engine/studentsset.h \
	engine/teacher.h \
	engine/subject.h \
	engine/activitytag.h \
	engine/room.h \
	engine/building.h \
	engine/generate.h \
	engine/lockunlock.h \
	engine/matrix.h \
	engine/messageboxes.h \
	\
	interface/centerwidgetonscreen.h \
	interface/fet.h \
	interface/longtextmessagebox.h

TEMPLATE = app

DEFINES += FET_COMMAND_LINE

CONFIG += release warn_on
win32 {
	CONFIG += console
}
QT -= gui

DESTDIR = ..
TARGET = fet-cl

OBJECTS_DIR = ../tmp/commandline
UI_DIR = ../tmp/commandline
MOC_DIR = ../tmp/commandline
RCC_DIR = ../tmp/commandline

INCLUDEPATH += engine interface
DEPENDPATH += engine interface

unix {
	target.path = /usr/bin

	INSTALLS += target
}
