/***************************************************************************
                          constraintteachersactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "constraintteachersactivitytagmaxhourscontinuouslyform.h"
#include "addconstraintteachersactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintteachersactivitytagmaxhourscontinuouslyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintTeachersActivityTagMaxHoursContinuouslyForm::ConstraintTeachersActivityTagMaxHoursContinuouslyForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersActivityTagMaxHoursContinuouslyForm_template", "Constraints teachers activity tag max hours continuously"));

	QString s = QCoreApplication::translate("ConstraintTeachersActivityTagMaxHoursContinuouslyForm_template", "This constraint ensures that all teachers do not have more than max hours continuously of activities with the specified activity tag");
	setInstructionText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeachersActivityTagMaxHoursContinuouslyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeachersActivityTagMaxHoursContinuouslyForm::~ConstraintTeachersActivityTagMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersActivityTagMaxHoursContinuouslyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ConstraintTeachersActivityTagMaxHoursContinuously* ct=(ConstraintTeachersActivityTagMaxHoursContinuously*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString activityTagName = filter_widget->activityTag();
		return (ct->activityTagName==activityTagName || activityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeachersActivityTagMaxHoursContinuouslyForm::createAddDialog()
{
	return new AddConstraintTeachersActivityTagMaxHoursContinuouslyForm(this);
}

QDialog * ConstraintTeachersActivityTagMaxHoursContinuouslyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm(this, (ConstraintTeachersActivityTagMaxHoursContinuously*)ctr);
}
