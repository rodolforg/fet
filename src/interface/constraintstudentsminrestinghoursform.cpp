/***************************************************************************
                          constraintstudentsminrestinghoursform.cpp  -  description
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

#include "constraintstudentsminrestinghoursform.h"
#include "addconstraintstudentsminrestinghoursform.h"
#include "modifyconstraintstudentsminrestinghoursform.h"

ConstraintStudentsMinRestingHoursForm::ConstraintStudentsMinRestingHoursForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMinRestingHoursForm_template", "Constraints students min resting hours"));

	QString s = QCoreApplication::translate("ConstraintStudentsMinRestingHoursForm_template", "This constraint ensures a minimum number of resting hours between the end of a day and the beginning of the next day. Circular means that the time between the end of the last day of the week and the beginning of the first day of the week is also considered.");
	setInstructionText(s);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMinRestingHoursForm::~ConstraintStudentsMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMinRestingHoursForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MIN_RESTING_HOURS)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMinRestingHoursForm::createAddDialog()
{
	return new AddConstraintStudentsMinRestingHoursForm(this);
}

QDialog * ConstraintStudentsMinRestingHoursForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMinRestingHoursForm(this, (ConstraintStudentsMinRestingHours*)ctr);
}
