/***************************************************************************
                          constraintteacherintervalmaxdaysperweekform.cpp  -  description
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

#include "constraintteacherintervalmaxdaysperweekform.h"
#include "addconstraintteacherintervalmaxdaysperweekform.h"
#include "modifyconstraintteacherintervalmaxdaysperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherIntervalMaxDaysPerWeekForm::ConstraintTeacherIntervalMaxDaysPerWeekForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherIntervalMaxDaysPerWeekForm_template", "Constraints teacher interval max days per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherIntervalMaxDaysPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherIntervalMaxDaysPerWeekForm::~ConstraintTeacherIntervalMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherIntervalMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
		ConstraintTeacherIntervalMaxDaysPerWeek* c=(ConstraintTeacherIntervalMaxDaysPerWeek*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherIntervalMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintTeacherIntervalMaxDaysPerWeekForm(this);
}

QDialog * ConstraintTeacherIntervalMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherIntervalMaxDaysPerWeekForm(this, (ConstraintTeacherIntervalMaxDaysPerWeek*)ctr);
}
