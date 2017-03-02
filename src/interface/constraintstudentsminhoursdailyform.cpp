/***************************************************************************
                          constraintstudentsminhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
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

#include "constraintstudentsminhoursdailyform.h"
#include "addconstraintstudentsminhoursdailyform.h"
#include "modifyconstraintstudentsminhoursdailyform.h"

#include "helponstudentsminhoursdaily.h"

ConstraintStudentsMinHoursDailyForm::ConstraintStudentsMinHoursDailyForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMinHoursDailyForm_template", "Constraints students min hours daily"));

	QString s = QCoreApplication::translate("ConstraintStudentsMinHoursDailyForm_template", "IMPORTANT: allow empty days for students is a nonstandard option. Only select it if your institution permits that and if a timetable with empty days for students exists. Otherwise do not  select it, for a good performance of generation. Press Help button for more information.");
	setInstructionText(s);

	s = HelpOnStudentsMinHoursDaily::getHelpText();
	setHelpText(s);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMinHoursDailyForm::~ConstraintStudentsMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMinHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMinHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsMinHoursDailyForm(this);
}

QDialog * ConstraintStudentsMinHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMinHoursDailyForm(this, (ConstraintStudentsMinHoursDaily*)ctr);
}

