/***************************************************************************
                          constraintstudentsmaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "helponimperfectconstraints.h"

#include "constraintstudentsmaxgapsperdayform.h"
#include "addconstraintstudentsmaxgapsperdayform.h"
#include "modifyconstraintstudentsmaxgapsperdayform.h"

ConstraintStudentsMaxGapsPerDayForm::ConstraintStudentsMaxGapsPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxGapsPerDayForm_template", "Constraints students max gaps per day"));

	QString s = QCoreApplication::translate("ConstraintStudentsMaxGapsPerDayForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
	setInstructionText(s);

	setHelp();

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMaxGapsPerDayForm::~ConstraintStudentsMaxGapsPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxGapsPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxGapsPerDayForm::createAddDialog()
{
	return new AddConstraintStudentsMaxGapsPerDayForm(this);
}

QDialog * ConstraintStudentsMaxGapsPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxGapsPerDayForm(this, (ConstraintStudentsMaxGapsPerDay*)ctr);
}

void ConstraintStudentsMaxGapsPerDayForm::setHelp()
{
	QString s = HelpOnImperfectConstraints::getHelpText();
	setHelpText(s);
}
