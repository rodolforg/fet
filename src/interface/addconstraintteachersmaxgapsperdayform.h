/***************************************************************************
                          addconstraintteachersmaxgapsperdayform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERSMAXGAPSPERDAYFORM_H
#define ADDCONSTRAINTTEACHERSMAXGAPSPERDAYFORM_H

#include "ui_addconstraintteachersmaxgapsperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxGapsPerDayForm : public QDialog, Ui::AddConstraintTeachersMaxGapsPerDayForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxGapsPerDayForm(QWidget* parent);
	~AddConstraintTeachersMaxGapsPerDayForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
