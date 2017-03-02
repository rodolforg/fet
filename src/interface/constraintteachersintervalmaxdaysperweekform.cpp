/***************************************************************************
                          constraintteachersintervalmaxdaysperweekform.cpp  -  description
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

#include "constraintteachersintervalmaxdaysperweekform.h"
#include "addconstraintteachersintervalmaxdaysperweekform.h"
#include "modifyconstraintteachersintervalmaxdaysperweekform.h"

ConstraintTeachersIntervalMaxDaysPerWeekForm::ConstraintTeachersIntervalMaxDaysPerWeekForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersIntervalMaxDaysPerWeekForm_template", "Constraints teachers interval max days per week"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersIntervalMaxDaysPerWeekForm::~ConstraintTeachersIntervalMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersIntervalMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
		return true;
	}
	else{
		return false;
	}
}

QDialog * ConstraintTeachersIntervalMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintTeachersIntervalMaxDaysPerWeekForm(this);
}

QDialog * ConstraintTeachersIntervalMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersIntervalMaxDaysPerWeekForm(this, (ConstraintTeachersIntervalMaxDaysPerWeek*)ctr);
}
