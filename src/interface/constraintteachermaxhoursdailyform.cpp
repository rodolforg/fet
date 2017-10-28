/***************************************************************************
                          constraintteachermaxhoursdailyform.cpp  -  description
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

#include "constraintteachermaxhoursdailyform.h"
#include "addconstraintteachermaxhoursdailyform.h"
#include "modifyconstraintteachermaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxHoursDailyForm::ConstraintTeacherMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMaxHoursDailyForm_template", "Constraints teacher max hours daily"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxHoursDailyForm::~ConstraintTeacherMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
		ConstraintTeacherMaxHoursDaily* c=(ConstraintTeacherMaxHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeacherMaxHoursDailyForm(this);
}

QDialog * ConstraintTeacherMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxHoursDailyForm(this, (ConstraintTeacherMaxHoursDaily*)ctr);
}
