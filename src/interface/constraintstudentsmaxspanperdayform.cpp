/***************************************************************************
                          constraintstudentsmaxspanperdayform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include "constraintstudentsmaxspanperdayform.h"
#include "addconstraintstudentsmaxspanperdayform.h"
#include "modifyconstraintstudentsmaxspanperdayform.h"

ConstraintStudentsMaxSpanPerDayForm::ConstraintStudentsMaxSpanPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxSpanPerDayForm_template", "Constraints students max span per day"));

	QString s = QCoreApplication::translate("ConstraintStudentsMaxSpanPerDayForm_template", "This constraint ensures a maximum span of activities on each day. Span means the count of slots (free or occupied) starting with the first occupied slot of the day and ending with the last occupied slot of the day.");
	setInstructionText(s);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMaxSpanPerDayForm::~ConstraintStudentsMaxSpanPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxSpanPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxSpanPerDayForm::createAddDialog()
{
	return new AddConstraintStudentsMaxSpanPerDayForm(this);
}

QDialog * ConstraintStudentsMaxSpanPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxSpanPerDayForm(this, (ConstraintStudentsMaxSpanPerDay*)ctr);
}
