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
	engine/randomknuth.cpp \
	engine/messageboxes.cpp \
	engine/export.cpp \
	engine/xmllog.cpp \
	engine/constraints/constraintpre.cpp \
	engine/constraints/mindaysbetweenactivities.cpp \
	engine/constraints/maxdaysbetweenactivities.cpp \
	engine/constraints/mingapsbetweenactivities.cpp \
	engine/constraints/teachersmincontinuousgapininterval.cpp \
	engine/constraints/studentsmincontinuousgapininterval.cpp \
	engine/constraints/paradoxmindaysvsconsecutive.cpp \
	engine/constraints/paradoxmindaysvstwogrouped.cpp \
	engine/constraints/paradoxmindaysvsthreegrouped.cpp \
	engine/constraints/paradoxmindaysvssameday.cpp \
	\
	interface/fet.cpp \
	interface/longtextmessagebox.cpp \

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
	engine/matrix.h \
	engine/randomknuth.h \
	engine/messageboxes.h \
	engine/export.h \
	engine/xmllog.h \
	engine/constraints/constraintpre.h \
	engine/constraints/mindaysbetweenactivities.h \
	engine/constraints/maxdaysbetweenactivities.h \
	engine/constraints/mingapsbetweenactivities.h \
	engine/constraints/teachersmincontinuousgapininterval.h \
	engine/constraints/studentsmincontinuousgapininterval.h \
	engine/constraints/paradoxmindaysvsconsecutive.h \
	engine/constraints/paradoxmindaysvstwogrouped.h \
	engine/constraints/paradoxmindaysvsthreegrouped.h \
	\
	interface/fet.h \
	interface/longtextmessagebox.h \

TEMPLATE = app

DEFINES += \
	FET_COMMAND_LINE \
	QT_DEPRECATED_WARNINGS

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
