/***************************************************************************
                          constrainttwoactivitiesconsecutiveform.cpp  -  description
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

#include "constrainttwoactivitiesconsecutiveform.h"
#include "addconstrainttwoactivitiesconsecutiveform.h"
#include "modifyconstrainttwoactivitiesconsecutiveform.h"

ConstraintTwoActivitiesConsecutiveForm::ConstraintTwoActivitiesConsecutiveForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "Constraints two activies consecutive"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "This constraint forces two activities A1 and A2: A2 to be immediately after A1, same day, possibly separated by breaks");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "It is a good practice NOT to add this type of constraint to activities which are constrained to be in different days by min days between activities constraints. Please read Help/Important tips, tip number 3).");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTwoActivitiesConsecutiveForm::~ConstraintTwoActivitiesConsecutiveForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesConsecutiveForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE)
		return true;
	else
		return false;
}

QDialog * ConstraintTwoActivitiesConsecutiveForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesConsecutiveForm(this);
}

QDialog * ConstraintTwoActivitiesConsecutiveForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesConsecutiveForm(this, (ConstraintTwoActivitiesConsecutive*)ctr);
}
