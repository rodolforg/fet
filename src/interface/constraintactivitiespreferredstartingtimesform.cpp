/***************************************************************************
                          constraintactivitiespreferredstartingtimesform.cpp  -  description
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

#include "constraintactivitiespreferredstartingtimesform.h"
#include "addconstraintactivitiespreferredstartingtimesform.h"
#include "modifyconstraintactivitiespreferredstartingtimesform.h"

ConstraintActivitiesPreferredStartingTimesForm::ConstraintActivitiesPreferredStartingTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesPreferredStartingTimesForm_template", "Constraints activities preferred starting times"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintActivitiesPreferredStartingTimesForm::~ConstraintActivitiesPreferredStartingTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesPreferredStartingTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesPreferredStartingTimesForm::createAddDialog()
{
	return new AddConstraintActivitiesPreferredStartingTimesForm(this);
}

QDialog * ConstraintActivitiesPreferredStartingTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesPreferredStartingTimesForm(this, (ConstraintActivitiesPreferredStartingTimes*)ctr);
}
