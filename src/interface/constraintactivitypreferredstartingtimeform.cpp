/***************************************************************************
                          constraintactivitypreferredstartingtimeform.cpp  -  description
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

#include "constraintactivitypreferredstartingtimeform.h"
#include "addconstraintactivitypreferredstartingtimeform.h"
#include "modifyconstraintactivitypreferredstartingtimeform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "lockunlock.h"

#include "centerwidgetonscreen.h"

ConstraintActivityPreferredStartingTimeForm::ConstraintActivityPreferredStartingTimeForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivityPreferredStartingTimeForm_template", "Constraints activity preferred starting time"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityPreferredStartingTimeForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityPreferredStartingTimeForm::~ConstraintActivityPreferredStartingTimeForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredStartingTimeForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
		return false;
		
	const ConstraintActivityPreferredStartingTime* c=(const ConstraintActivityPreferredStartingTime*) ctr;
	int id=c->activityId;
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

QDialog * ConstraintActivityPreferredStartingTimeForm::createAddDialog()
{
	return new AddConstraintActivityPreferredStartingTimeForm(this);
}

QDialog * ConstraintActivityPreferredStartingTimeForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivityPreferredStartingTimeForm(this, (ConstraintActivityPreferredStartingTime*)ctr);
}

void ConstraintActivityPreferredStartingTimeForm::afterRemoveConstraint()
{
	LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
	LockUnlock::increaseCommunicationSpinBox();
}

void ConstraintActivityPreferredStartingTimeForm::toggleActiveConstraint(void* ctr, bool checked) const
{
	TimeConstraintBaseDialog::toggleActiveConstraint(ctr, checked);

	LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
	LockUnlock::increaseCommunicationSpinBox();
}
