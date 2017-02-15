/***************************************************************************
                          constraintstudentsmaxspanperdayform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#ifndef CONSTRAINTSTUDENTSMAXSPANPERDAYFORM_H
#define CONSTRAINTSTUDENTSMAXSPANPERDAYFORM_H

#include "ui_constraintstudentsmaxspanperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintStudentsMaxSpanPerDayForm : public QDialog, Ui::ConstraintStudentsMaxSpanPerDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintStudentsMaxSpanPerDayForm(QWidget* parent);
	~ConstraintStudentsMaxSpanPerDayForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	
	void filterChanged();
};

#endif
