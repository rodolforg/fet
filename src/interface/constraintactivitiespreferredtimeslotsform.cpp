/* **************************************************************************
                          constraintactivitiespreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : 15 May 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "constraintactivitiespreferredtimeslotsform.h"
#include "addconstraintactivitiespreferredtimeslotsform.h"
#include "modifyconstraintactivitiespreferredtimeslotsform.h"

ConstraintActivitiesPreferredTimeSlotsForm::ConstraintActivitiesPreferredTimeSlotsForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesPreferredTimeSlotsForm_template", "Constraints activities preferred time slots"));

	setInstructionText(QCoreApplication::translate("ConstraintActivitiesPreferredTimeSlotsForm_template", "Each hour slot of each considered activity must be allowed by this constraint (more restrictive than preferred starting times for activities with duration greater than 1)"));

//	populateFilters();
	filterChanged();
}

bool ConstraintActivitiesPreferredTimeSlotsForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesPreferredTimeSlotsForm::createAddDialog()
{
	return new AddConstraintActivitiesPreferredTimeSlotsForm(this);
}

QDialog * ConstraintActivitiesPreferredTimeSlotsForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesPreferredTimeSlotsForm(this, (ConstraintActivitiesPreferredTimeSlots*)ctr);
}
