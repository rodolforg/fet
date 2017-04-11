/***************************************************************************
                          constraintteachersmaxspanperdayform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include "constraintteachersmaxspanperdayform.h"
#include "addconstraintteachersmaxspanperdayform.h"
#include "modifyconstraintteachersmaxspanperdayform.h"

ConstraintTeachersMaxSpanPerDayForm::ConstraintTeachersMaxSpanPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersMaxSpanPerDayForm_template", "Constraints teachers max span per day"));

	QString s = QCoreApplication::translate("ConstraintTeachersMaxSpanPerDayForm_template", "This constraint ensures a maximum span of activities on each day. Span means the count of slots (free or occupied) starting with the first occupied slot of the day and ending with the last occupied slot of the day.");
	setInstructionText(s);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeachersMaxSpanPerDayForm::~ConstraintTeachersMaxSpanPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersMaxSpanPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY)
		return true;
	else
		return false;
}

QDialog * ConstraintTeachersMaxSpanPerDayForm::createAddDialog()
{
	return new AddConstraintTeachersMaxSpanPerDayForm(this);
}

QDialog * ConstraintTeachersMaxSpanPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersMaxSpanPerDayForm(this, (ConstraintTeachersMaxSpanPerDay*)ctr);
}
