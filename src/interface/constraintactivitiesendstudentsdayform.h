/***************************************************************************
                          constraintactivitiesendstudentsdayform.h  -  description
                             -------------------
    begin                : 15 May 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H
#define CONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H

#include "ui_constraintactivitiesendstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesEndStudentsDayForm : public QDialog, Ui::ConstraintActivitiesEndStudentsDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesEndStudentsDayForm(QWidget* parent);
	~ConstraintActivitiesEndStudentsDayForm();

	void refreshConstraintsListWidget();

	bool filterOk(TimeConstraint* ctr);
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	
};

#endif
