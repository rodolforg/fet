/***************************************************************************
                          constraintstudentsearlymaxbeginningsatsecondhourform.cpp  -  description
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

#include "constraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "addconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h"

ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm_template", "Constraints students early max beginnings at second hour"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::~ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::createAddDialog()
{
	return new AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm(this);
}

QDialog * ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm(this, (ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)ctr);
}

