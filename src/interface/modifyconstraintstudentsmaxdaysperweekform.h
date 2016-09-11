/***************************************************************************
                          modifyconstraintstudentsmaxdaysperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintstudentsmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxDaysPerWeek* _ctr;

	ModifyConstraintStudentsMaxDaysPerWeekForm(QWidget* parent, ConstraintStudentsMaxDaysPerWeek* ctr);
	~ModifyConstraintStudentsMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
