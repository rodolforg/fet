/***************************************************************************
                          constraintstudentssetintervalmaxdaysperweekform.cpp  -  description
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

#include "constraintstudentssetintervalmaxdaysperweekform.h"
#include "addconstraintstudentssetintervalmaxdaysperweekform.h"
#include "modifyconstraintstudentssetintervalmaxdaysperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetIntervalMaxDaysPerWeekForm::ConstraintStudentsSetIntervalMaxDaysPerWeekForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetIntervalMaxDaysPerWeekForm_template", "Constraints students set interval max days per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetIntervalMaxDaysPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetIntervalMaxDaysPerWeekForm::~ConstraintStudentsSetIntervalMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
		ConstraintStudentsSetIntervalMaxDaysPerWeek* c=(ConstraintStudentsSetIntervalMaxDaysPerWeek*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetIntervalMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsSetIntervalMaxDaysPerWeekForm(this);
}

QDialog * ConstraintStudentsSetIntervalMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm(this, (ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr);
}
