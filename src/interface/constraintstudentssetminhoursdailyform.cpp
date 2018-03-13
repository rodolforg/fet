/***************************************************************************
                          constraintstudentssetminhoursdailyform.cpp  -  description
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

#include "constraintstudentssetminhoursdailyform.h"
#include "addconstraintstudentssetminhoursdailyform.h"
#include "modifyconstraintstudentssetminhoursdailyform.h"

#include "helponstudentsminhoursdaily.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetMinHoursDailyForm::ConstraintStudentsSetMinHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMinHoursDailyForm_template", "Constraints students set min hours daily"));

	QString s = QCoreApplication::translate("ConstraintStudentsSetMinHoursDailyForm_template", "IMPORTANT: allow empty days for students is a nonstandard option. Only select it if your institution permits that and if a timetable with empty days for students exists. Otherwise do not select it, for a good performance of generation. Press Help button for more information.");
	setInstructionText(s);

	s = HelpOnStudentsMinHoursDaily::getHelpText();
	setHelpText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMinHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMinHoursDailyForm::~ConstraintStudentsSetMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMinHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
		ConstraintStudentsSetMinHoursDaily* c=(ConstraintStudentsSetMinHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		return c->students==studentsName || studentsName.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMinHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsSetMinHoursDailyForm(this);
}

QDialog * ConstraintStudentsSetMinHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMinHoursDailyForm(this, (ConstraintStudentsSetMinHoursDaily*)ctr);
}
