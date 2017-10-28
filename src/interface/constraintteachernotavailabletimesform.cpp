/***************************************************************************
                          constraintteachernotavailabletimesform.cpp  -  description
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

#include "constraintteachernotavailabletimesform.h"
#include "addconstraintteachernotavailabletimesform.h"
#include "modifyconstraintteachernotavailabletimesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherNotAvailableTimesForm::ConstraintTeacherNotAvailableTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherNotAvailableTimesForm_template", "Constraints teacher not available times"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherNotAvailableTimesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherNotAvailableTimesForm::~ConstraintTeacherNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherNotAvailableTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
		ConstraintTeacherNotAvailableTimes* ctna=(ConstraintTeacherNotAvailableTimes*) ctr;
		QString teacherName = ((TeacherStudentSetSubjectActivityTag_FilterWidget*)getFilterWidget())->teacher();
		return ctna->teacher==teacherName || teacherName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintTeacherNotAvailableTimesForm::createAddDialog()
{
	return new AddConstraintTeacherNotAvailableTimesForm(this);
}

QDialog * ConstraintTeacherNotAvailableTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherNotAvailableTimesForm(this, (ConstraintTeacherNotAvailableTimes*)ctr);
}
