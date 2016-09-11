/***************************************************************************
                          modifyconstraintteachersmaxhoursdailyform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXHOURSDAILYFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXHOURSDAILYFORM_H

#include "ui_modifyconstraintteachersmaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxHoursDailyForm : public QDialog, Ui::ModifyConstraintTeachersMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMaxHoursDaily* _ctr;

	ModifyConstraintTeachersMaxHoursDailyForm(QWidget* parent, ConstraintTeachersMaxHoursDaily* ctr);
	~ModifyConstraintTeachersMaxHoursDailyForm();

	void updateMaxHoursSpinBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
