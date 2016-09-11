/***************************************************************************
                          modifyconstraintteachersintervalmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSINTERVALMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachersintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersIntervalMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersIntervalMaxDaysPerWeek* _ctr;

	ModifyConstraintTeachersIntervalMaxDaysPerWeekForm(QWidget* parent, ConstraintTeachersIntervalMaxDaysPerWeek* ctr);
	~ModifyConstraintTeachersIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
//	void updateTeachersComboBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
