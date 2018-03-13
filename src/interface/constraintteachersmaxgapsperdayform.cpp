/***************************************************************************
                          constraintteachersmaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : Jan 21, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "constraintteachersmaxgapsperdayform.h"
#include "addconstraintteachersmaxgapsperdayform.h"
#include "modifyconstraintteachersmaxgapsperdayform.h"

#include "centerwidgetonscreen.h"

ConstraintTeachersMaxGapsPerDayForm::ConstraintTeachersMaxGapsPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMaxGapsPerDayForm_template", "Constraints teachers max gaps per day"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMaxGapsPerDayForm::~ConstraintTeachersMaxGapsPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxGapsPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMaxGapsPerDayForm::createAddDialog()
{
	return new AddConstraintTeachersMaxGapsPerDayForm(this);
}

QDialog * ConstraintTeachersMaxGapsPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxGapsPerDayForm(this, (ConstraintTeachersMaxGapsPerDay*)ctr);
}
