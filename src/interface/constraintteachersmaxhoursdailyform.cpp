/***************************************************************************
                          constraintteachersmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "constraintteachersmaxhoursdailyform.h"
#include "addconstraintteachersmaxhoursdailyform.h"
#include "modifyconstraintteachersmaxhoursdailyform.h"

ConstraintTeachersMaxHoursDailyForm::ConstraintTeachersMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMaxHoursDailyForm_template", "Constraints teachers max hours daily"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMaxHoursDailyForm::~ConstraintTeachersMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeachersMaxHoursDailyForm(this);
}

QDialog * ConstraintTeachersMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxHoursDailyForm(this, (ConstraintTeachersMaxHoursDaily*)ctr);
}
