/***************************************************************************
                          constraintstudentsmaxbuildingchangesperdayform.cpp  -  description
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

#include "constraintstudentsmaxbuildingchangesperdayform.h"
#include "addconstraintstudentsmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperdayform.h"

ConstraintStudentsMaxBuildingChangesPerDayForm::ConstraintStudentsMaxBuildingChangesPerDayForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsMaxBuildingChangesPerDayForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students max building changes per day"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsMaxBuildingChangesPerDayForm::~ConstraintStudentsMaxBuildingChangesPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxBuildingChangesPerDayForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintStudentsMaxBuildingChangesPerDayForm::createAddDialog()
{
	return new AddConstraintStudentsMaxBuildingChangesPerDayForm(this);
}

QDialog * ConstraintStudentsMaxBuildingChangesPerDayForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxBuildingChangesPerDayForm(this, (ConstraintStudentsMaxBuildingChangesPerDay*)ctr);
}
