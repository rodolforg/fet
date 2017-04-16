/***************************************************************************
                          constraintstudentsactivitytagmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "helponimperfectconstraints.h"

#include "constraintstudentsactivitytagmaxhoursdailyform.h"
#include "addconstraintstudentsactivitytagmaxhoursdailyform.h"
#include "modifyconstraintstudentsactivitytagmaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsActivityTagMaxHoursDailyForm::ConstraintStudentsActivityTagMaxHoursDailyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsActivityTagMaxHoursDailyForm_template", "Constraints students activity tag max hours daily"));

	QString instruction = QCoreApplication::translate("ConstraintStudentsActivityTagMaxHoursDailyForm_template", "This constraint ensures that all students do not have more than max hours daily of activities with the specified activity tag.");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintStudentsActivityTagMaxHoursDailyForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
	setInstructionText(instruction);

	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsActivityTagMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsActivityTagMaxHoursDailyForm::~ConstraintStudentsActivityTagMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsActivityTagMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ConstraintStudentsActivityTagMaxHoursDaily* c=(ConstraintStudentsActivityTagMaxHoursDaily*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString filterActivityTagName = filterWidget->activityTag();
		return (c->activityTagName == filterActivityTagName || filterActivityTagName.isEmpty());
	}
	else
		return false;
}

QDialog *ConstraintStudentsActivityTagMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintStudentsActivityTagMaxHoursDailyForm(this);
}

QDialog *ConstraintStudentsActivityTagMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsActivityTagMaxHoursDailyForm(this, (ConstraintStudentsActivityTagMaxHoursDaily*) ctr);
}

void ConstraintStudentsActivityTagMaxHoursDailyForm::setHelp()
{
	setHelpText(HelpOnImperfectConstraints::getHelpText());
}
