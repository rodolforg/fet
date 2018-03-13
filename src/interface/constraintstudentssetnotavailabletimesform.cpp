/***************************************************************************
                          constraintstudentssetnotavailabletimesform.cpp  -  description
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

#include "constraintstudentssetnotavailabletimesform.h"
#include "addconstraintstudentssetnotavailabletimesform.h"
#include "modifyconstraintstudentssetnotavailabletimesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetNotAvailableTimesForm::ConstraintStudentsSetNotAvailableTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetNotAvailableTimesForm_template", "Constraints students set not available times"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetNotAvailableTimesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetNotAvailableTimesForm::~ConstraintStudentsSetNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetNotAvailableTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
		ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetNotAvailableTimesForm::createAddDialog()
{
	return new AddConstraintStudentsSetNotAvailableTimesForm(this);
}

QDialog * ConstraintStudentsSetNotAvailableTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetNotAvailableTimesForm(this, (ConstraintStudentsSetNotAvailableTimes*)ctr);
}
