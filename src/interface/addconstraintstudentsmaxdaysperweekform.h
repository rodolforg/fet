/***************************************************************************
                          addconstraintstudentsmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintstudentsmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintStudentsMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintStudentsMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
