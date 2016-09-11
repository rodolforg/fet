/***************************************************************************
                          modifyconstraintteachersmindaysperweekform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMINDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachersmindaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMinDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMinDaysPerWeek* _ctr;

	ModifyConstraintTeachersMinDaysPerWeekForm(QWidget* parent, ConstraintTeachersMinDaysPerWeek* ctr);
	~ModifyConstraintTeachersMinDaysPerWeekForm();

	void updateMinDaysSpinBox();
	//void updateTeachersComboBox();

public slots:
	//void constraintChanged();
	void ok();
	void cancel();
};

#endif
