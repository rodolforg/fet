/***************************************************************************
                          constraintstudentsmaxhourscontinuouslyform.cpp  -  description
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

#include "constraintstudentsmaxhourscontinuouslyform.h"
#include "addconstraintstudentsmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsmaxhourscontinuouslyform.h"

ConstraintStudentsMaxHoursContinuouslyForm::ConstraintStudentsMaxHoursContinuouslyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsMaxHoursContinuouslyForm_template", "Constraints students max hours continuously"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsMaxHoursContinuouslyForm::~ConstraintStudentsMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMaxHoursContinuouslyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY)
		return true;
	else
		return false;
}

QDialog * ConstraintStudentsMaxHoursContinuouslyForm::createAddDialog()
{
	return new AddConstraintStudentsMaxHoursContinuouslyForm(this);
}

QDialog * ConstraintStudentsMaxHoursContinuouslyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsMaxHoursContinuouslyForm(this, (ConstraintStudentsMaxHoursContinuously*)ctr);
}
