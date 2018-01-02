/***************************************************************************
                          constraintteachersmingapsbetweenbuildingchangesform.cpp  -  description
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

#include "constraintteachersmingapsbetweenbuildingchangesform.h"
#include "addconstraintteachersmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintteachersmingapsbetweenbuildingchangesform.h"

ConstraintTeachersMinGapsBetweenBuildingChangesForm::ConstraintTeachersMinGapsBetweenBuildingChangesForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeachersMinGapsBetweenBuildingChangesForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teachers min gaps between building changes"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeachersMinGapsBetweenBuildingChangesForm::~ConstraintTeachersMinGapsBetweenBuildingChangesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMinGapsBetweenBuildingChangesForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintTeachersMinGapsBetweenBuildingChangesForm::createAddDialog()
{
	return new AddConstraintTeachersMinGapsBetweenBuildingChangesForm(this);
}

QDialog * ConstraintTeachersMinGapsBetweenBuildingChangesForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm(this, (ConstraintTeachersMinGapsBetweenBuildingChanges*)ctr);
}
