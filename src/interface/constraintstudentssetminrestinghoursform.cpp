/***************************************************************************
                          constraintstudentssetminrestinghoursform.cpp  -  description
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

#include "constraintstudentssetminrestinghoursform.h"
#include "addconstraintstudentssetminrestinghoursform.h"
#include "modifyconstraintstudentssetminrestinghoursform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetMinRestingHoursForm::ConstraintStudentsSetMinRestingHoursForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMinRestingHoursForm_template", "Constraints students set min resting hours"));

	QString s = QCoreApplication::translate("ConstraintStudentsSetMinRestingHoursForm_template", "This constraint ensures a minimum number of resting hours between the end of a day and the beginning of the next day. Circular means that the time between the end of the last day of the week and the beginning of the first day of the week is also considered.");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMinRestingHoursForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMinRestingHoursForm::~ConstraintStudentsSetMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMinRestingHoursForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS){
		ConstraintStudentsSetMinRestingHours* c=(ConstraintStudentsSetMinRestingHours*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMinRestingHoursForm::createAddDialog()
{
	return new AddConstraintStudentsSetMinRestingHoursForm(this);
}

QDialog * ConstraintStudentsSetMinRestingHoursForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMinRestingHoursForm(this, (ConstraintStudentsSetMinRestingHours*)ctr);
}
