/***************************************************************************
                          constraintstudentssetearlymaxbeginningsatsecondhourform.cpp  -  description
                             -------------------
    begin                : July 18, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "addconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm_template", "Constraints students set early max beginnings at second hour"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::~ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* c=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::createAddDialog()
{
	return new AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(this);
}

QDialog * ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(this, (ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr);
}
