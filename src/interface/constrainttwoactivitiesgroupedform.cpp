/***************************************************************************
                          constrainttwoactivitiesgroupedform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constrainttwoactivitiesgroupedform.h"
#include "addconstrainttwoactivitiesgroupedform.h"
#include "modifyconstrainttwoactivitiesgroupedform.h"

ConstraintTwoActivitiesGroupedForm::ConstraintTwoActivitiesGroupedForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "Constraints two activies grouped"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "This constraint forces two activities to be in the same day, one immediately following the other, in any order, possibly separated by breaks");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "It is a good practice NOT to add this type of constraint to activities which are constrained to be in different days by min days between activities constraints. Please read Help/Important tips, tip number 3).");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTwoActivitiesGroupedForm::~ConstraintTwoActivitiesGroupedForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesGroupedForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED)
		return true;
	else
		return false;
}

QDialog * ConstraintTwoActivitiesGroupedForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesGroupedForm(this);
}

QDialog * ConstraintTwoActivitiesGroupedForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesGroupedForm(this, (ConstraintTwoActivitiesGrouped*)ctr);
}
