/***************************************************************************
                          constraintteachersminrestinghoursform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include "constraintteachersminrestinghoursform.h"
#include "addconstraintteachersminrestinghoursform.h"
#include "modifyconstraintteachersminrestinghoursform.h"

ConstraintTeachersMinRestingHoursForm::ConstraintTeachersMinRestingHoursForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMinRestingHoursForm_template", "Constraints teachers min resting hours"));

	QString s = QCoreApplication::translate("ConstraintTeachersMinRestingHoursForm_template", "This constraint ensures a minimum number of resting hours between the end of a day and the beginning of the next day. Circular means that the time between the end of the last day of the week and the beginning of the first day of the week is also considered.");
	setInstructionText(s);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMinRestingHoursForm::~ConstraintTeachersMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMinRestingHoursForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_RESTING_HOURS)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMinRestingHoursForm::createAddDialog()
{
	return new AddConstraintTeachersMinRestingHoursForm(this);
}

QDialog * ConstraintTeachersMinRestingHoursForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMinRestingHoursForm(this, (ConstraintTeachersMinRestingHours*)ctr);
}
