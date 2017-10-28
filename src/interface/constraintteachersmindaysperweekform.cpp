/***************************************************************************
                          constraintteachersmindaysperweekform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintteachersmindaysperweekform.h"
#include "addconstraintteachersmindaysperweekform.h"
#include "modifyconstraintteachersmindaysperweekform.h"

ConstraintTeachersMinDaysPerWeekForm::ConstraintTeachersMinDaysPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMinDaysPerWeekForm_template", "Constraints teachers min days per week"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMinDaysPerWeekForm::~ConstraintTeachersMinDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMinDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMinDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintTeachersMinDaysPerWeekForm(this);
}

QDialog * ConstraintTeachersMinDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMinDaysPerWeekForm(this, (ConstraintTeachersMinDaysPerWeek*)ctr);
}

