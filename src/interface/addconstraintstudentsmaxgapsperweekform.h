/***************************************************************************
                          addconstraintstudentsmaxgapsperweekform.h  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#ifndef ADDCONSTRAINTSTUDENTSMAXGAPSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSMAXGAPSPERWEEKFORM_H

#include "ui_addconstraintstudentsmaxgapsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMaxGapsPerWeekForm : public QDialog, Ui::AddConstraintStudentsMaxGapsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMaxGapsPerWeekForm(QWidget* parent);
	~AddConstraintStudentsMaxGapsPerWeekForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
