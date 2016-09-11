/***************************************************************************
                          addactivityform.h  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDACTIVITYFORM_H
#define ADDACTIVITYFORM_H

#include "ui_addactivityform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QSet>
//#include <QHash>
#include <QList>

class AddActivityForm : public QDialog, Ui::AddActivityForm_template  {
	Q_OBJECT
	
private:
	QList<QSpinBox*> durList;
	QList<QCheckBox*> activList;

	QSpinBox* dur(int i);
	QCheckBox* activ(int i);
	
	QSet<QString> teacherNamesSet;
	QSet<QString> subjectNamesSet;
	QSet<QString> activityTagNamesSet;
	//QHash<QString, int> numberOfStudentsHash;

public:
	QList<QString> canonicalStudentsSetsNames;

	AddActivityForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName);
	~AddActivityForm();

	void updateStudentsListWidget();
	void updateTeachersListWidget();
	void updateSubjectsComboBox();
	void updateActivityTagsListWidget();
	void updatePreferredDaysComboBox();
	void updatePreferredHoursComboBox();

public slots:
	void addTeacher();
	void removeTeacher();
	void addStudents();
	void removeStudents();

	void addActivityTag();
	void removeActivityTag();

	void splitChanged();
	
	void clearTeachers();
	void clearStudents();
	void clearActivityTags();
	
	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();

	void addActivity();
	void help();
	
	void minDaysChanged();
};

class SecondMinDaysDialog: public QDialog
{
	Q_OBJECT
public:
	SecondMinDaysDialog(QWidget* p, int minD, double weight);
	~SecondMinDaysDialog();
	
	double weight;
	QLineEdit* percText;

public slots:
	void yesPressed();
};

#endif
