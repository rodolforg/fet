/***************************************************************************
                          constraintstudentssetearlymaxbeginningsatsecondhourform.h  -  description
                             -------------------
    begin                : July 18, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef CONSTRAINTSTUDENTSSETEARLYMAXBEGINNINGSATSECONDHOURFORM_H
#define CONSTRAINTSTUDENTSSETEARLYMAXBEGINNINGSATSECONDHOURFORM_H

#include "ui_constraintstudentssetearlymaxbeginningsatsecondhourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm : public QDialog, Ui::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(QWidget* parent);
	~ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
