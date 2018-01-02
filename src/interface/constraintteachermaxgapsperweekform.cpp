/***************************************************************************
                          constraintteachermaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : July 15 , 2007
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

#include "constraintteachermaxgapsperweekform.h"
#include "addconstraintteachermaxgapsperweekform.h"
#include "modifyconstraintteachermaxgapsperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxGapsPerWeekForm::ConstraintTeacherMaxGapsPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherMaxGapsPerWeekForm_template", "Constraints teacher max gaps per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxGapsPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxGapsPerWeekForm::~ConstraintTeacherMaxGapsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxGapsPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
		ConstraintTeacherMaxGapsPerWeek* c=(ConstraintTeacherMaxGapsPerWeek*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		return (c->teacherName==teacherName || teacherName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxGapsPerWeekForm::createAddDialog()
{
	return new AddConstraintTeacherMaxGapsPerWeekForm(this);
}

QDialog * ConstraintTeacherMaxGapsPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxGapsPerWeekForm(this, (ConstraintTeacherMaxGapsPerWeek*)ctr);
}
