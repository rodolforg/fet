/***************************************************************************
                          modifyconstraintstudentsmaxhoursdailyform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYFORM_H

#include "ui_modifyconstraintstudentsmaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxHoursDailyForm : public QDialog, Ui::ModifyConstraintStudentsMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxHoursDaily* _ctr;

	ModifyConstraintStudentsMaxHoursDailyForm(QWidget* parent, ConstraintStudentsMaxHoursDaily* ctr);
	~ModifyConstraintStudentsMaxHoursDailyForm();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
