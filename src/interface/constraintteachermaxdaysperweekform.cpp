/***************************************************************************
                          constraintteachermaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "constraintteachermaxdaysperweekform.h"
#include "addconstraintteachermaxdaysperweekform.h"
#include "modifyconstraintteachermaxdaysperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxDaysPerWeekForm::ConstraintTeacherMaxDaysPerWeekForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMaxDaysPerWeekForm_template", "Constraints teacher max days per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxDaysPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxDaysPerWeekForm::~ConstraintTeacherMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
		ConstraintTeacherMaxDaysPerWeek* c=(ConstraintTeacherMaxDaysPerWeek*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxDaysPerWeekForm::createAddDialog()
{
	return new AddConstraintTeacherMaxDaysPerWeekForm(this);
}

QDialog * ConstraintTeacherMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxDaysPerWeekForm(this, (ConstraintTeacherMaxDaysPerWeek*)ctr);
}
