/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintstudentsmincontinuousgapinintervalform.h"
#include "addconstraintstudentsmincontinuousgapinintervalform.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsMinContinuousGapInIntervalForm::ConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent) : TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle("Constraints students min continuous gap in interval");

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMinContinuousGapInIntervalForm::~ConstraintStudentsMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMinContinuousGapInIntervalForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MIN_CONTINUOUS_GAP_IN_INTERVAL)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMinContinuousGapInIntervalForm::createAddDialog()
{
	return new AddConstraintStudentsMinContinuousGapInIntervalForm(this, true);
}

QDialog * ConstraintStudentsMinContinuousGapInIntervalForm::createModifyDialog(TimeConstraint *ctr)
{
	return new AddConstraintStudentsMinContinuousGapInIntervalForm(this, (ConstraintTeachersMinContinuousGapInInterval*)ctr);
}
