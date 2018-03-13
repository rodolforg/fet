/***************************************************************************
                          constraintactivitypreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "constraintactivitypreferredtimeslotsform.h"
#include "addconstraintactivitypreferredtimeslotsform.h"
#include "modifyconstraintactivitypreferredtimeslotsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivityPreferredTimeSlotsForm::ConstraintActivityPreferredTimeSlotsForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivityPreferredTimeSlotsForm_template", "Constraints activity preferred time slots"));

	QString instruction = QCoreApplication::translate("ConstraintActivityPreferredTimeSlotsForm_template", "Each hour slot of each considered activity must be allowed by this constraint (more restrictive than preferred starting times for activities with duration greater than 1)");
	setInstructionText(instruction);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityPreferredTimeSlotsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityPreferredTimeSlotsForm::~ConstraintActivityPreferredTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredTimeSlotsForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS)
		return false;
		
	const ConstraintActivityPreferredTimeSlots* c=(const ConstraintActivityPreferredTimeSlots*) ctr;
	int id=c->p_activityId;
	const Activity* act=NULL;
	foreach(const Activity* a, gt.rules.activitiesList) {
		if(a->id==id) {
			act=a;
			break;
		}
	}
	assert(act != NULL);

	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	return filterWidget->filterActivity(act);
}

QDialog * ConstraintActivityPreferredTimeSlotsForm::createAddDialog()
{
	return new AddConstraintActivityPreferredTimeSlotsForm(this);
}

QDialog * ConstraintActivityPreferredTimeSlotsForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivityPreferredTimeSlotsForm(this, (ConstraintActivityPreferredTimeSlots*)ctr);
}
