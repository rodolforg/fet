/***************************************************************************
                          constrainttwoactivitiesorderedform.cpp  -  description
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

#include "constrainttwoactivitiesorderedform.h"
#include "addconstrainttwoactivitiesorderedform.h"
#include "modifyconstrainttwoactivitiesorderedform.h"

ConstraintTwoActivitiesOrderedForm::ConstraintTwoActivitiesOrderedForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesOrderedForm_template", "Constraints two activies ordered"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesOrderedForm_template", "This constraint forces two activities A1 and A2: A2 to be after A1, separated by any time interval in the week");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTwoActivitiesOrderedForm::~ConstraintTwoActivitiesOrderedForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesOrderedForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED)
		return true;
	else
		return false;
}

QDialog * ConstraintTwoActivitiesOrderedForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesOrderedForm(this);
}

QDialog * ConstraintTwoActivitiesOrderedForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesOrderedForm(this, (ConstraintTwoActivitiesOrdered*)ctr);
}
