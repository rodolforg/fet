/***************************************************************************
                          constraintteachersminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept. 21, 2007
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

#include "constraintteachersminhoursdailyform.h"
#include "addconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"

ConstraintTeachersMinHoursDailyForm::ConstraintTeachersMinHoursDailyForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMinHoursDailyForm_template", "Constraints teachers min hours daily"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMinHoursDailyForm::~ConstraintTeachersMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMinHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMinHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeachersMinHoursDailyForm(this);
}

QDialog * ConstraintTeachersMinHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMinHoursDailyForm(this, (ConstraintTeachersMinHoursDaily*)ctr);
}

