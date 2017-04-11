/***************************************************************************
                          constraintstudentsmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#include "constraintstudentsmaxdaysperweekform.h"
#include "addconstraintstudentsmaxdaysperweekform.h"
#include "modifyconstraintstudentsmaxdaysperweekform.h"

ConstraintStudentsMaxDaysPerWeekForm::ConstraintStudentsMaxDaysPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxDaysPerWeekForm_template", "Constraints students max days per week"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsMaxDaysPerWeekForm::~ConstraintStudentsMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsMaxDaysPerWeekForm(this);
}

QDialog * ConstraintStudentsMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxDaysPerWeekForm(this, (ConstraintStudentsMaxDaysPerWeek*)ctr);
}

