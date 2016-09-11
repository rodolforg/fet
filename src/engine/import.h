/*
File import.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          import.h  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>

#include "timetable.h"
#include "timetable_defs.h"

class QGroupBox;
class QLineEdit;
class QRadioButton;
class QComboBox;
class QPushButton;
class QSpinBox;
class QHBoxLayout;

static const int DO_NOT_IMPORT=-2;
static const int IMPORT_DEFAULT_ITEM=-1;

static const int FIELD_LINE_NUMBER=0;

static const int FIELD_YEAR_NAME=1;
static const int FIELD_YEAR_NUMBER_OF_STUDENTS=2;
static const int FIELD_GROUP_NAME=3;
static const int FIELD_GROUP_NUMBER_OF_STUDENTS=4;
static const int FIELD_SUBGROUP_NAME=5;
static const int FIELD_SUBGROUP_NUMBER_OF_STUDENTS=6;

static const int FIELD_SUBJECT_NAME=7;

static const int FIELD_ACTIVITY_TAG_NAME=8;

static const int FIELD_TEACHER_NAME=9;

static const int FIELD_BUILDING_NAME=10;
static const int FIELD_ROOM_NAME=11;
static const int FIELD_ROOM_CAPACITY=12;

static const int FIELD_ACTIVITY_TAGS_SET=13;

static const int FIELD_STUDENTS_SET=14;
static const int FIELD_TEACHERS_SET=15;

static const int FIELD_TOTAL_DURATION=16;
static const int FIELD_SPLIT_DURATION=17;
static const int FIELD_MIN_DAYS=18;
static const int FIELD_MIN_DAYS_WEIGHT=19;
static const int FIELD_MIN_DAYS_CONSECUTIVE=20;

static const int NUMBER_OF_FIELDS=21;

class Import: public QObject{
	Q_OBJECT

	static int chooseWidth(int w);
	static int chooseHeight(int h);

public:
	Import();
	~Import();

	static void importCSVActivities(QWidget* parent);
	static void importCSVActivityTags(QWidget* parent);
	static void importCSVRoomsAndBuildings(QWidget* parent);
	static void importCSVSubjects(QWidget* parent);
	static void importCSVTeachers(QWidget* parent);
	static void importCSVStudents(QWidget* parent);

private:
	static void prearrangement();
	static int getFileSeparatorFieldsAndHead(QWidget* parent, QDialog* &newParent);
	static int readFields(QWidget* parent);
	static int showFieldsAndWarnings(QWidget* parent, QDialog* &newParent);
};

class ChooseFieldsDialog: public QDialog{
	Q_OBJECT
	
public:							//can be this done privat, too?
	ChooseFieldsDialog(QWidget *parent);
	~ChooseFieldsDialog();

private:
	QGroupBox* fieldGroupBox[NUMBER_OF_FIELDS];
	QRadioButton* fieldRadio1[NUMBER_OF_FIELDS];
	QRadioButton* fieldRadio2[NUMBER_OF_FIELDS];
	QRadioButton* fieldRadio3[NUMBER_OF_FIELDS];
	QRadioButton* fieldRadio3b[NUMBER_OF_FIELDS];
	QComboBox* fieldLine2CB[NUMBER_OF_FIELDS];
	QLineEdit* fieldLine3Text[NUMBER_OF_FIELDS];
	QSpinBox* fieldLine3bSpinBox[NUMBER_OF_FIELDS];
	QPushButton* pb;
	QPushButton* cancelpb;
	QHBoxLayout* buttonsLayout;
	
	QString _settingsName;
	
private slots:
	void chooseFieldsDialogClose();
	void chooseFieldsDialogUpdateRadio1();
	void chooseFieldsDialogUpdateRadio2();
	void chooseFieldsDialogUpdateRadio3();
	void chooseFieldsDialogUpdateRadio3b();
	void chooseFieldsDialogUpdateLine3Text();
};

class LastWarningsDialog: public QDialog{
	Q_OBJECT
	
public:							//can be this done privat, too?
	LastWarningsDialog(QWidget *parent);
	~LastWarningsDialog();
};

#endif
