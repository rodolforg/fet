/***************************************************************************
                          constraintactivitiessamestartinghourform.cpp  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#include "constraintactivitiessamestartinghourform.h"
#include "addconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"

ConstraintActivitiesSameStartingHourForm::ConstraintActivitiesSameStartingHourForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesSameStartingHourForm_template", "Constraints activities same starting hour"));

//	populateFilters();
	filterChanged();
}

bool ConstraintActivitiesSameStartingHourForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesSameStartingHourForm::createAddDialog()
{
	return new AddConstraintActivitiesSameStartingHourForm(this);
}

QDialog * ConstraintActivitiesSameStartingHourForm::createModifyDialog(TimeConstraint* ctr)
{
	return new ModifyConstraintActivitiesSameStartingHourForm(this, (ConstraintActivitiesSameStartingHour*)ctr);
}
