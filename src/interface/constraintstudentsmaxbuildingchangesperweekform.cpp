/***************************************************************************
                          constraintstudentsmaxbuildingchangesperweekform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "constraintstudentsmaxbuildingchangesperweekform.h"
#include "addconstraintstudentsmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperweekform.h"


ConstraintStudentsMaxBuildingChangesPerWeekForm::ConstraintStudentsMaxBuildingChangesPerWeekForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsMaxBuildingChangesPerWeekForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students max building changes per week"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsMaxBuildingChangesPerWeekForm::~ConstraintStudentsMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxBuildingChangesPerWeekForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintStudentsMaxBuildingChangesPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsMaxBuildingChangesPerWeekForm(this);
}

QDialog * ConstraintStudentsMaxBuildingChangesPerWeekForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxBuildingChangesPerWeekForm(this, (ConstraintStudentsMaxBuildingChangesPerWeek*)ctr);
}
