/***************************************************************************
                          constraintteachermaxspanperdayform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include "constraintteachermaxspanperdayform.h"
#include "addconstraintteachermaxspanperdayform.h"
#include "modifyconstraintteachermaxspanperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxSpanPerDayForm::ConstraintTeacherMaxSpanPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMaxSpanPerDayForm_template", "Constraints teacher max span per day"));

	QString s = QCoreApplication::translate("ConstraintTeacherMaxSpanPerDayForm_template", "This constraint ensures a maximum span of activities on each day. Span means the count of slots (free or occupied) starting with the first occupied slot of the day and ending with the last occupied slot of the day.");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxSpanPerDayForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxSpanPerDayForm::~ConstraintTeacherMaxSpanPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxSpanPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY){
		ConstraintTeacherMaxSpanPerDay* c=(ConstraintTeacherMaxSpanPerDay*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxSpanPerDayForm::createAddDialog()
{
	return new AddConstraintTeacherMaxSpanPerDayForm(this);
}

QDialog * ConstraintTeacherMaxSpanPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxSpanPerDayForm(this, (ConstraintTeacherMaxSpanPerDay*)ctr);
}
