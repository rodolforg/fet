QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_paradox_mindays_consecutive.cpp \
    ../../../src/engine/activity.cpp \
    ../../../src/engine/activitytag.cpp \
    ../../../src/engine/building.cpp \
    ../../../src/engine/lockunlock.cpp \
    ../../../src/engine/room.cpp \
    ../../../src/engine/rules.cpp \
    ../../../src/engine/solution.cpp \
    ../../../src/engine/studentsset.cpp \
    ../../../src/engine/subject.cpp \
    ../../../src/engine/teacher.cpp \
    ../../../src/engine/timeconstraint.cpp \
    ../../../src/engine/xmllog.cpp \
    ../../../src/engine/messageboxes.cpp \
    ../../../src/engine/randomknuth.cpp \
    ../../../src/engine/timetable_defs.cpp \
    ../../../src/engine/generate_pre.cpp \
    ../../../src/interface/longtextmessagebox.cpp \
    ../../../src/engine/constraints/constraintpre.cpp \
    ../../../src/engine/constraints/mindaysbetweenactivities.cpp \
    ../../../src/engine/constraints/maxdaysbetweenactivities.cpp \
    ../../../src/engine/constraints/mingapsbetweenactivities.cpp \
    ../../../src/engine/constraints/teachersmincontinuousgapininterval.cpp \
    ../../../src/engine/constraints/studentsmincontinuousgapininterval.cpp \
    ../../../src/engine/constraints/paradoxmindaysvsconsecutive.cpp \
    ../../../src/engine/constraints/paradoxmindaysvstwogrouped.cpp \
    ../../../src/engine/constraints/paradoxmindaysvsthreegrouped.cpp \
    ../../../src/engine/spaceconstraint.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\" FET_COMMAND_LINE

HEADERS += \
	../../../src/engine/activity.h \
	../../../src/engine/solution.h \
	../../../src/engine/rules.h \
	../../../src/engine/timeconstraint.h \
	../../../src/engine/timetable_defs.h \
	../../../src/engine/studentsset.h \
	../../../src/engine/teacher.h \
	../../../src/engine/subject.h \
	../../../src/engine/activitytag.h \
	../../../src/engine/room.h \
	../../../src/engine/building.h \
	../../../src/engine/lockunlock.h \
	../../../src/engine/messageboxes.h \
	../../../src/engine/generate_pre.h \
	\
	../../../src/interface/longtextmessagebox.h

INCLUDEPATH += $$PWD/../../../src/engine $$PWD/../../../src/interface
