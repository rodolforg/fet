/***************************************************************************
                          constraintteachersmaxbuildingchangesperdayform.cpp  -  description
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

#include "constraintteachersmaxbuildingchangesperdayform.h"
#include "addconstraintteachersmaxbuildingchangesperdayform.h"
#include "modifyconstraintteachersmaxbuildingchangesperdayform.h"

ConstraintTeachersMaxBuildingChangesPerDayForm::ConstraintTeachersMaxBuildingChangesPerDayForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeachersMaxBuildingChangesPerDayForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teachers max building changes per day"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeachersMaxBuildingChangesPerDayForm::~ConstraintTeachersMaxBuildingChangesPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxBuildingChangesPerDayForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintTeachersMaxBuildingChangesPerDayForm::createAddDialog()
{
	return new AddConstraintTeachersMaxBuildingChangesPerDayForm(this);
}

QDialog * ConstraintTeachersMaxBuildingChangesPerDayForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxBuildingChangesPerDayForm(this, (ConstraintTeachersMaxBuildingChangesPerDay*)ctr);
}
