/***************************************************************************
                          addconstraintteachermaxgapsperweekform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERMAXGAPSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERMAXGAPSPERWEEKFORM_H

#include "ui_addconstraintteachermaxgapsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxGapsPerWeekForm : public QDialog, Ui::AddConstraintTeacherMaxGapsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxGapsPerWeekForm(QWidget* parent);
	~AddConstraintTeacherMaxGapsPerWeekForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
