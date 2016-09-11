/***************************************************************************
                          addconstraintteachersmaxdaysperweekform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERSMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERSMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintteachersmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintTeachersMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintTeachersMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	//void updateTeachersComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
