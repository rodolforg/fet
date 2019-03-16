SOURCES += \
	engine/export.cpp \
	engine/generate_pre.cpp \
	engine/messageboxes.cpp \
	\
	interface/fet.cpp \
	interface/longtextmessagebox.cpp \

HEADERS += \
	engine/generate_pre.h \
	engine/messageboxes.h \
	engine/export.h \
	\
	interface/fet.h \
	interface/longtextmessagebox.h \

TEMPLATE = app

DEFINES += \
	FET_COMMAND_LINE \
	QT_DEPRECATED_WARNINGS \
	QT_NO_FOREACH

CONFIG += release warn_on c++11
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

LIBS += -L../ -lfet

unix {
	target.path = /usr/bin

	INSTALLS += target
}
