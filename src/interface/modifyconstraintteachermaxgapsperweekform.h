/***************************************************************************
                          modifyconstraintteachermaxgapsperweekform.h  -  description
                             -------------------
    begin                : July 15, 2007
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

#ifndef MODIFYCONSTRAINTTEACHERMAXGAPSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERMAXGAPSPERWEEKFORM_H

#include "ui_modifyconstraintteachermaxgapsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxGapsPerWeekForm : public QDialog, Ui::ModifyConstraintTeacherMaxGapsPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxGapsPerWeek* _ctr;

	ModifyConstraintTeacherMaxGapsPerWeekForm(QWidget* parent, ConstraintTeacherMaxGapsPerWeek* ctr);
	~ModifyConstraintTeacherMaxGapsPerWeekForm();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
