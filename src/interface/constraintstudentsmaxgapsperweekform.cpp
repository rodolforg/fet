/***************************************************************************
                          constraintstudentsmaxgapsperweekform.cpp  -  description
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

#include "constraintstudentsmaxgapsperweekform.h"
#include "addconstraintstudentsmaxgapsperweekform.h"
#include "modifyconstraintstudentsmaxgapsperweekform.h"

ConstraintStudentsMaxGapsPerWeekForm::ConstraintStudentsMaxGapsPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxGapsPerWeekForm_template", "Constraints students max gaps per week"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMaxGapsPerWeekForm::~ConstraintStudentsMaxGapsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxGapsPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxGapsPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsMaxGapsPerWeekForm(this);
}

QDialog * ConstraintStudentsMaxGapsPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxGapsPerWeekForm(this, (ConstraintStudentsMaxGapsPerWeek*)ctr);
}
