/***************************************************************************
                          constraintstudentssetactivitytagmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "helponimperfectconstraints.h"

#include "constraintstudentssetactivitytagmaxhoursdailyform.h"
#include "addconstraintstudentssetactivitytagmaxhoursdailyform.h"
#include "modifyconstraintstudentssetactivitytagmaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetActivityTagMaxHoursDailyForm::ConstraintStudentsSetActivityTagMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetActivityTagMaxHoursDailyForm_template", "Constraints students set activity tag max hours daily"));

	QString s = QCoreApplication::translate("ConstraintStudentsSetActivityTagMaxHoursDailyForm_template", "This constraint ensures that the specified students do not have more than max hours daily of activities with the specified activity tag.");
	s += "\n\n" + QCoreApplication::translate("ConstraintStudentsSetActivityTagMaxHoursDailyForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
	setInstructionText(s);

	s = HelpOnImperfectConstraints::getHelpText();
	setHelpText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetActivityTagMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetActivityTagMaxHoursDailyForm::~ConstraintStudentsSetActivityTagMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetActivityTagMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ConstraintStudentsSetActivityTagMaxHoursDaily* c=(ConstraintStudentsSetActivityTagMaxHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsName = filter_widget->studentsSet();
		QString activityTagName = filter_widget->activityTag();
		return (c->students==studentsName || studentsName.isEmpty())
		 &&
		 (c->activityTagName==activityTagName || activityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetActivityTagMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsSetActivityTagMaxHoursDailyForm(this);
}

QDialog * ConstraintStudentsSetActivityTagMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetActivityTagMaxHoursDailyForm(this, (ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr);
}
