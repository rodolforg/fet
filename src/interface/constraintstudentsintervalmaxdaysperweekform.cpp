/***************************************************************************
                          constraintstudentsintervalmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2008
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

#include "constraintstudentsintervalmaxdaysperweekform.h"
#include "addconstraintstudentsintervalmaxdaysperweekform.h"
#include "modifyconstraintstudentsintervalmaxdaysperweekform.h"

ConstraintStudentsIntervalMaxDaysPerWeekForm::ConstraintStudentsIntervalMaxDaysPerWeekForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsIntervalMaxDaysPerWeekForm_template", "Constraints students interval max days per week"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsIntervalMaxDaysPerWeekForm::~ConstraintStudentsIntervalMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsIntervalMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsIntervalMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsIntervalMaxDaysPerWeekForm(this);
}

QDialog * ConstraintStudentsIntervalMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsIntervalMaxDaysPerWeekForm(this, (ConstraintStudentsIntervalMaxDaysPerWeek*)ctr);
}
