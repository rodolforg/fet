/***************************************************************************
                          constraintstudentsactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "constraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "addconstraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsActivityTagMaxHoursContinuouslyForm::ConstraintStudentsActivityTagMaxHoursContinuouslyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template", "Constraints students activity tag max hours continuously"));

	QString instruction = QCoreApplication::translate("ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template", "This constraint ensures that all students do not have more than max hours continuously of activities with the specified activity tag");
	setInstructionText(instruction);
	
	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsActivityTagMaxHoursContinuouslyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsActivityTagMaxHoursContinuouslyForm::~ConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsActivityTagMaxHoursContinuouslyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ConstraintStudentsActivityTagMaxHoursContinuously* c=(ConstraintStudentsActivityTagMaxHoursContinuously*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString filterActivityTagName = filterWidget->activityTag();
		return (c->activityTagName == filterActivityTagName || filterActivityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintStudentsActivityTagMaxHoursContinuouslyForm::createAddDialog()
{
	return new AddConstraintStudentsActivityTagMaxHoursContinuouslyForm(this);
}

QDialog * ConstraintStudentsActivityTagMaxHoursContinuouslyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm(this, (ConstraintStudentsActivityTagMaxHoursContinuously*)ctr);
}

