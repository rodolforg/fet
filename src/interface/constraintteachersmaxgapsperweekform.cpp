/***************************************************************************
                          constraintteachersmaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : July 6, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constraintteachersmaxgapsperweekform.h"
#include "addconstraintteachersmaxgapsperweekform.h"
#include "modifyconstraintteachersmaxgapsperweekform.h"

ConstraintTeachersMaxGapsPerWeekForm::ConstraintTeachersMaxGapsPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMaxGapsPerWeekForm_template", "Constraints teachers max gaps per week"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMaxGapsPerWeekForm::~ConstraintTeachersMaxGapsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxGapsPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMaxGapsPerWeekForm::createAddDialog()
{
	return new AddConstraintTeachersMaxGapsPerWeekForm(this);
}

QDialog * ConstraintTeachersMaxGapsPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxGapsPerWeekForm(this, (ConstraintTeachersMaxGapsPerWeek*)ctr);
}
