/***************************************************************************
                          constraintteacheractivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "constraintteacheractivitytagmaxhourscontinuouslyform.h"
#include "addconstraintteacheractivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintteacheractivitytagmaxhourscontinuouslyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherActivityTagMaxHoursContinuouslyForm::ConstraintTeacherActivityTagMaxHoursContinuouslyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherActivityTagMaxHoursContinuouslyForm_template", "Constraints teacher activity tag max hours continuously"));

	QString s = QCoreApplication::translate("ConstraintTeacherActivityTagMaxHoursContinuouslyForm_template", "This constraint ensures that the specified teacher does not have more than max hours continuously of activities with the specified activity tag");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherActivityTagMaxHoursContinuouslyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherActivityTagMaxHoursContinuouslyForm::~ConstraintTeacherActivityTagMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherActivityTagMaxHoursContinuouslyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ConstraintTeacherActivityTagMaxHoursContinuously* ct=(ConstraintTeacherActivityTagMaxHoursContinuously*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		QString activityTagName = filter_widget->activityTag();
		return (ct->teacherName==teacherName || teacherName.isEmpty())
		 &&
		 (ct->activityTagName==activityTagName || activityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherActivityTagMaxHoursContinuouslyForm::createAddDialog()
{
	return new AddConstraintTeacherActivityTagMaxHoursContinuouslyForm(this);
}

QDialog * ConstraintTeacherActivityTagMaxHoursContinuouslyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherActivityTagMaxHoursContinuouslyForm(this, (ConstraintTeacherActivityTagMaxHoursContinuously*)ctr);
}
