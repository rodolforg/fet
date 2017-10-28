/***************************************************************************
                          constraintstudentssetmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
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

#include "constraintstudentssetmaxhoursdailyform.h"
#include "addconstraintstudentssetmaxhoursdailyform.h"
#include "modifyconstraintstudentssetmaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetMaxHoursDailyForm::ConstraintStudentsSetMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMaxHoursDailyForm_template", "Constraints students set max hours daily"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMaxHoursDailyForm::~ConstraintStudentsSetMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
		ConstraintStudentsSetMaxHoursDaily* c=(ConstraintStudentsSetMaxHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsSetMaxHoursDailyForm(this);
}

QDialog * ConstraintStudentsSetMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMaxHoursDailyForm(this, (ConstraintStudentsSetMaxHoursDaily*)ctr);
}
