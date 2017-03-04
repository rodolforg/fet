/***************************************************************************
                          constraintstudentssetactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "constraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "addconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template", "Constraints students set activity tag max hours continuously"));

	QString s = QCoreApplication::translate("ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template", "This constraint ensures that the specified students do not have more than max hours continuously of activities with the specified activity tag");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::~ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ConstraintStudentsSetActivityTagMaxHoursContinuously* c=(ConstraintStudentsSetActivityTagMaxHoursContinuously*) ctr;
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

QDialog * ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::createAddDialog()
{
	return new AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(this);
}

QDialog * ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(this, (ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr);
}
