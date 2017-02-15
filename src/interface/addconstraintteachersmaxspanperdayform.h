/***************************************************************************
                          addconstraintteachersmaxspanperdayform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERSMAXSPANPERDAYFORM_H
#define ADDCONSTRAINTTEACHERSMAXSPANPERDAYFORM_H

#include "ui_addconstraintteachersmaxspanperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxSpanPerDayForm : public QDialog, Ui::AddConstraintTeachersMaxSpanPerDayForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxSpanPerDayForm(QWidget* parent);
	~AddConstraintTeachersMaxSpanPerDayForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
