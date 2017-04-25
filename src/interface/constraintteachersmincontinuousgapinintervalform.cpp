/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintteachersmincontinuousgapinintervalform.h"
#include "addconstraintteachersmincontinuousgapinintervalform.h"

#include "centerwidgetonscreen.h"

ConstraintTeachersMinContinuousGapInIntervalForm::ConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent) : TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle("Constraints teachers min continuous gap in interval");

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMinContinuousGapInIntervalForm::~ConstraintTeachersMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMinContinuousGapInIntervalForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_CONTINUOUS_GAP_IN_INTERVAL)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMinContinuousGapInIntervalForm::createAddDialog()
{
	return new AddConstraintTeachersMinContinuousGapInIntervalForm(this, true);
}

QDialog * ConstraintTeachersMinContinuousGapInIntervalForm::createModifyDialog(TimeConstraint *ctr)
{
	return new AddConstraintTeachersMinContinuousGapInIntervalForm(this, (ConstraintTeachersMinContinuousGapInInterval*)ctr);
}
