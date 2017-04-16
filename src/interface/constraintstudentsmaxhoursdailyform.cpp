/***************************************************************************
                          constraintstudentsmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
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

#include "constraintstudentsmaxhoursdailyform.h"
#include "addconstraintstudentsmaxhoursdailyform.h"
#include "modifyconstraintstudentsmaxhoursdailyform.h"

ConstraintStudentsMaxHoursDailyForm::ConstraintStudentsMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxHoursDailyForm_template", "Constraints students max hours daily"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMaxHoursDailyForm::~ConstraintStudentsMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsMaxHoursDailyForm(this);
}

QDialog * ConstraintStudentsMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxHoursDailyForm(this, (ConstraintStudentsMaxHoursDaily*)ctr);
}
