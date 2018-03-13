/***************************************************************************
                          constraintbreaktimesform.cpp  -  description
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

#include "constraintbreaktimesform.h"
#include "addconstraintbreaktimesform.h"
#include "modifyconstraintbreaktimesform.h"

#include "centerwidgetonscreen.h"

ConstraintBreakTimesForm::ConstraintBreakTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintBreakTimesForm_template", "Constraints break times"));

	QString instruction = QCoreApplication::translate("ConstraintBreakTimesForm_template", "Note: An activity with duration greater than 1 cannot be split over a break. Also, the activities constrained by min days between activities constraints with 'consecutive if same day' = true cannot spread over a break.");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintBreakTimesForm::~ConstraintBreakTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintBreakTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_BREAK_TIMES)
		return true;
	else
		return false;
}

QDialog * ConstraintBreakTimesForm::createAddDialog()
{
	return new AddConstraintBreakTimesForm(this);
}

QDialog * ConstraintBreakTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintBreakTimesForm(this, (ConstraintBreakTimes*)ctr);
}
