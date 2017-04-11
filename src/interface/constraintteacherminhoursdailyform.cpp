/***************************************************************************
                          constraintteacherminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept. 21, 2007
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

#include "constraintteacherminhoursdailyform.h"
#include "addconstraintteacherminhoursdailyform.h"
#include "modifyconstraintteacherminhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMinHoursDailyForm::ConstraintTeacherMinHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMinHoursDailyForm_template", "Constraints teacher min hours daily"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMinHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMinHoursDailyForm::~ConstraintTeacherMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMinHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
		ConstraintTeacherMinHoursDaily* c=(ConstraintTeacherMinHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMinHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeacherMinHoursDailyForm(this);
}

QDialog * ConstraintTeacherMinHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMinHoursDailyForm(this, (ConstraintTeacherMinHoursDaily*)ctr);
}
