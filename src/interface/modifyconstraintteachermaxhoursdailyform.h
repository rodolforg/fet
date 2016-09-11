/***************************************************************************
                          modifyconstraintteachermaxhoursdailyform.h  -  description
                             -------------------
    begin                : July 19, 2007
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

#ifndef MODIFYCONSTRAINTTEACHERMAXHOURSDAILYFORM_H
#define MODIFYCONSTRAINTTEACHERMAXHOURSDAILYFORM_H

#include "ui_modifyconstraintteachermaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxHoursDailyForm : public QDialog, Ui::ModifyConstraintTeacherMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxHoursDaily* _ctr;

	ModifyConstraintTeacherMaxHoursDailyForm(QWidget* parent, ConstraintTeacherMaxHoursDaily* ctr);
	~ModifyConstraintTeacherMaxHoursDailyForm();

	void updateMaxHoursSpinBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
