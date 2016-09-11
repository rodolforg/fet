/***************************************************************************
                          teachersubjectsqualificationsform.h  -  description
                             -------------------
    begin                : 2016
    copyright            : (C) 2016 by Lalescu Liviu
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

#ifndef TEACHERSUBJECTSQUALIFICATIONSFORM_H
#define TEACHERSUBJECTSQUALIFICATIONSFORM_H

#include "ui_teachersubjectsqualificationsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class Teacher;

class TeacherSubjectsQualificationsForm : public QDialog, Ui::TeacherSubjectsQualificationsForm_template {
	Q_OBJECT
public:
	TeacherSubjectsQualificationsForm(QWidget* parent, Teacher* teacher);
	~TeacherSubjectsQualificationsForm();
	
	Teacher* _teacher;

public slots:
	void clear();

	void addSubject();
	void removeSubject();
	
	void addAllSubjects();

	void ok();
};

#endif
