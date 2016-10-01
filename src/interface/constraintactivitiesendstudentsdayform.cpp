/***************************************************************************
                          constraintactivitiesendstudentsdayform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "constraintactivitiesendstudentsdayform.h"
#include "addconstraintactivitiesendstudentsdayform.h"
#include "modifyconstraintactivitiesendstudentsdayform.h"

ConstraintActivitiesEndStudentsDayForm::ConstraintActivitiesEndStudentsDayForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesEndStudentsDayForm_template", "Constraints activities end students day"));
//	populateFilters();
	filterChanged();
}

bool ConstraintActivitiesEndStudentsDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesEndStudentsDayForm::createAddDialog()
{
	return new AddConstraintActivitiesEndStudentsDayForm(this);
}

QDialog * ConstraintActivitiesEndStudentsDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesEndStudentsDayForm(this, (ConstraintActivitiesEndStudentsDay*)ctr);
}
