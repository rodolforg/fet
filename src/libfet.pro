#-------------------------------------------------
#
# Project created by QtCreator 2019-03-15T22:37:07
#
#-------------------------------------------------

QT       -= gui

TARGET = fet
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
	QT_NO_FOREACH

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = ..

OBJECTS_DIR = ../tmp/library
UI_DIR      = ../tmp/library
MOC_DIR     = ../tmp/library
RCC_DIR     = ../tmp/library

INCLUDEPATH += engine interface
DEPENDPATH  += engine interface


SOURCES += \
	engine/timetableexport.cpp \
	engine/activity.cpp \
	engine/groupactivitiesininitialorderitem.cpp \
	engine/solution.cpp \
	engine/timetable.cpp \
	engine/rules.cpp \
	engine/errorcode.cpp \
#	engine/import.cpp \
#	engine/export.cpp \
#	engine/generate_pre.cpp \
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
#	engine/messageboxes.cpp \
	engine/xmllog.cpp \
	engine/conflictinfo.cpp \
#	engine/statisticsexport.cpp \
	engine/lockunlock.cpp \
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
	engine/constraints/paradoxmingapsvsmindaysconsecutive.cpp \
	engine/constraints/paradoxmingapsvsconsecutive.cpp \
	engine/constraints/paradoxmingapsvs2grouped.cpp \
	engine/constraints/paradoxmingapsvs3grouped.cpp \
	engine/constraints/paradoxorderedvsreversedconsecutive.cpp \
	engine/constraints/paradoxconsecutivevsconsecutive.cpp \
	engine/constraints/paradoxorderedifsamedayvsconsecutive.cpp \
	engine/constraints/paradoxorderedifsamedayvsreversed.cpp \

HEADERS += \
	engine/timetableexport.h \
	engine/activity.h \
	engine/groupactivitiesininitialorderitem.h \
	engine/solution.h \
	engine/timetable.h \
	engine/rules.h \
	engine/errorcode.h \
#	engine/import.h \
#	engine/export.h \
#	engine/generate_pre.h \
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
#	engine/statisticsexport.h \
	engine/lockunlock.h \
	engine/matrix.h \
	engine/randomknuth.h \
	engine/messageboxes.h \
	engine/xmllog.h \
	engine/conflictinfo.h \
	engine/constraints/constraintpre.h \
	engine/constraints/mindaysbetweenactivities.h \
	engine/constraints/maxdaysbetweenactivities.h \
	engine/constraints/mingapsbetweenactivities.h \
	engine/constraints/teachersmincontinuousgapininterval.h \
	engine/constraints/studentsmincontinuousgapininterval.h \
	engine/constraints/paradoxmindaysvsconsecutive.h \
	engine/constraints/paradoxmindaysvstwogrouped.h \
	engine/constraints/paradoxmindaysvsthreegrouped.h \
	engine/constraints/paradoxmindaysvssameday.h \
	engine/constraints/paradoxmingapsvsmindaysconsecutive.h \
	engine/constraints/paradoxmingapsvsconsecutive.h \
	engine/constraints/paradoxmingapsvs2grouped.h \
	engine/constraints/paradoxmingapsvs3grouped.h \
	engine/constraints/paradoxorderedvsreversedconsecutive.h \
	engine/constraints/paradoxconsecutivevsconsecutive.h \
	engine/constraints/paradoxorderedifsamedayvsconsecutive.h \
	engine/constraints/paradoxorderedifsamedayvsreversed.h \

CONFIG += warn_on c++11

unix {
    target.path = /usr/lib
    INSTALLS += target
}
