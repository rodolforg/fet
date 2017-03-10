/***************************************************************************
                          constraintteachermaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : Jan 21, 2008
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

#include "constraintteachermaxgapsperdayform.h"
#include "addconstraintteachermaxgapsperdayform.h"
#include "modifyconstraintteachermaxgapsperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxGapsPerDayForm::ConstraintTeacherMaxGapsPerDayForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMaxGapsPerDayForm_template", "Constraints teacher max gaps per day"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxGapsPerDayForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxGapsPerDayForm::~ConstraintTeacherMaxGapsPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxGapsPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
		ConstraintTeacherMaxGapsPerDay* c=(ConstraintTeacherMaxGapsPerDay*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxGapsPerDayForm::createAddDialog()
{
	return new AddConstraintTeacherMaxGapsPerDayForm(this);
}

QDialog * ConstraintTeacherMaxGapsPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxGapsPerDayForm(this, (ConstraintTeacherMaxGapsPerDay*)ctr);
}
