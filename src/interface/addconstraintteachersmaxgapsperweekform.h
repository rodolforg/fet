/***************************************************************************
                          addconstraintteachersmaxgapsperweekform.h  -  description
                             -------------------
    begin                : July 6, 2007
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

#ifndef ADDCONSTRAINTTEACHERSMAXGAPSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERSMAXGAPSPERWEEKFORM_H

#include "ui_addconstraintteachersmaxgapsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxGapsPerWeekForm : public QDialog, Ui::AddConstraintTeachersMaxGapsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxGapsPerWeekForm(QWidget* parent);
	~AddConstraintTeachersMaxGapsPerWeekForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
