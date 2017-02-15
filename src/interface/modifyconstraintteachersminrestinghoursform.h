/***************************************************************************
                          modifyconstraintteachersminrestinghoursform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINRESTINGHOURSFORM_H
#define MODIFYCONSTRAINTTEACHERSMINRESTINGHOURSFORM_H

#include "ui_modifyconstraintteachersminrestinghoursform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinRestingHoursForm : public QDialog, Ui::ModifyConstraintTeachersMinRestingHoursForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMinRestingHours* _ctr;

	ModifyConstraintTeachersMinRestingHoursForm(QWidget* parent, ConstraintTeachersMinRestingHours* ctr);
	~ModifyConstraintTeachersMinRestingHoursForm();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
