SOURCES += \
	../../../src/engine/generate_pre.cpp \
	../../../src/engine/messageboxes.cpp \
	\
	../../../src/interface/longtextmessagebox.cpp

HEADERS += \
	../../../src/engine/activity.h \
	../../../src/engine/solution.h \
	../../../src/engine/rules.h \
	../../../src/engine/generate_pre.h \
	../../../src/engine/timeconstraint.h \
	../../../src/engine/timetable_defs.h \
	../../../src/engine/studentsset.h \
	../../../src/engine/teacher.h \
	../../../src/engine/subject.h \
	../../../src/engine/activitytag.h \
	../../../src/engine/room.h \
	../../../src/engine/building.h \
	../../../src/engine/messageboxes.h \
	\
	../../../src/interface/longtextmessagebox.h

LIBS += -L../../../ -lfet
