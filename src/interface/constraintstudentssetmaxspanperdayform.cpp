/***************************************************************************
                          constraintstudentssetmaxspanperdayform.cpp  -  description
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

#include "constraintstudentssetmaxspanperdayform.h"
#include "addconstraintstudentssetmaxspanperdayform.h"
#include "modifyconstraintstudentssetmaxspanperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetMaxSpanPerDayForm::ConstraintStudentsSetMaxSpanPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMaxSpanPerDayForm_template", "Constraints students set max span per day"));

	QString s = QCoreApplication::translate("ConstraintStudentsSetMaxSpanPerDayForm_template", "This constraint ensures a maximum span of activities on each day. Span means the count of slots (free or occupied) starting with the first occupied slot of the day and ending with the last occupied slot of the day.");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxSpanPerDayForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMaxSpanPerDayForm::~ConstraintStudentsSetMaxSpanPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxSpanPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
		ConstraintStudentsSetMaxSpanPerDay* c=(ConstraintStudentsSetMaxSpanPerDay*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxSpanPerDayForm::createAddDialog()
{
	return new AddConstraintStudentsSetMaxSpanPerDayForm(this);
}

QDialog * ConstraintStudentsSetMaxSpanPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMaxSpanPerDayForm(this, (ConstraintStudentsSetMaxSpanPerDay*)ctr);
}
