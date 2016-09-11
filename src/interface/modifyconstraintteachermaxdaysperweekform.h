/***************************************************************************
                          modifyconstraintteachermaxdaysperweekform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachermaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeacherMaxDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeacherMaxDaysPerWeek* _ctr;

	ModifyConstraintTeacherMaxDaysPerWeekForm(QWidget* parent, ConstraintTeacherMaxDaysPerWeek* ctr);
	~ModifyConstraintTeacherMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateTeachersComboBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
