/***************************************************************************
                          constraintteacherminrestinghoursform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include "constraintteacherminrestinghoursform.h"
#include "addconstraintteacherminrestinghoursform.h"
#include "modifyconstraintteacherminrestinghoursform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMinRestingHoursForm::ConstraintTeacherMinRestingHoursForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMinRestingHoursForm_template", "Constraints teacher max hours continuously"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMinRestingHoursForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMinRestingHoursForm::~ConstraintTeacherMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMinRestingHoursForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MIN_RESTING_HOURS){
		ConstraintTeacherMinRestingHours* c=(ConstraintTeacherMinRestingHours*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMinRestingHoursForm::createAddDialog()
{
	return new AddConstraintTeacherMinRestingHoursForm(this);
}

QDialog * ConstraintTeacherMinRestingHoursForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMinRestingHoursForm(this, (ConstraintTeacherMinRestingHours*)ctr);
}
