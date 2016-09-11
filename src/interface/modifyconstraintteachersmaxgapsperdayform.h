/***************************************************************************
                          modifyconstraintteachersmaxgapsperdayform.h  -  description
                             -------------------
    begin                : Jan 21, 2008
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXGAPSPERDAYFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXGAPSPERDAYFORM_H

#include "ui_modifyconstraintteachersmaxgapsperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxGapsPerDayForm : public QDialog, Ui::ModifyConstraintTeachersMaxGapsPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMaxGapsPerDay* _ctr;

	ModifyConstraintTeachersMaxGapsPerDayForm(QWidget* parent, ConstraintTeachersMaxGapsPerDay* ctr);
	~ModifyConstraintTeachersMaxGapsPerDayForm();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
