/***************************************************************************
                          constraintstudentsmingapsbetweenbuildingchangesform.cpp  -  description
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

#include "constraintstudentsmingapsbetweenbuildingchangesform.h"
#include "addconstraintstudentsmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentsmingapsbetweenbuildingchangesform.h"

ConstraintStudentsMinGapsBetweenBuildingChangesForm::ConstraintStudentsMinGapsBetweenBuildingChangesForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsMinGapsBetweenBuildingChangesForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students min gaps between building changes"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsMinGapsBetweenBuildingChangesForm::~ConstraintStudentsMinGapsBetweenBuildingChangesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMinGapsBetweenBuildingChangesForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		return true;
	}
	else
		return false;
}

QDialog * ConstraintStudentsMinGapsBetweenBuildingChangesForm::createAddDialog()
{
	return new AddConstraintStudentsMinGapsBetweenBuildingChangesForm(this);
}

QDialog * ConstraintStudentsMinGapsBetweenBuildingChangesForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsMinGapsBetweenBuildingChangesForm(this, (ConstraintStudentsMinGapsBetweenBuildingChanges*)ctr);
}
