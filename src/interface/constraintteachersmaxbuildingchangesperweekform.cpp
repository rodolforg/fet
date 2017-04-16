/***************************************************************************
                          constraintteachersmaxbuildingchangesperweekform.cpp  -  description
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

#include "constraintteachersmaxbuildingchangesperweekform.h"
#include "addconstraintteachersmaxbuildingchangesperweekform.h"
#include "modifyconstraintteachersmaxbuildingchangesperweekform.h"

ConstraintTeachersMaxBuildingChangesPerWeekForm::ConstraintTeachersMaxBuildingChangesPerWeekForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeachersMaxBuildingChangesPerWeekForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teachers max building changes per week"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeachersMaxBuildingChangesPerWeekForm::~ConstraintTeachersMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxBuildingChangesPerWeekForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintTeachersMaxBuildingChangesPerWeekForm::createAddDialog()
{
	return new AddConstraintTeachersMaxBuildingChangesPerWeekForm(this);
}

QDialog * ConstraintTeachersMaxBuildingChangesPerWeekForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxBuildingChangesPerWeekForm(this, (ConstraintTeachersMaxBuildingChangesPerWeek*)ctr);
}
