/***************************************************************************
                          constraintactivitiessamestartinghourform.cpp  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#include "constraintactivitiessamestartinghourform.h"
#include "addconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivitiesSameStartingHourForm::ConstraintActivitiesSameStartingHourForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesSameStartingHourForm_template", "Constraints activities same starting hour"));

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesSameStartingHourForm::filterChanged);

	filterChanged();
}

ConstraintActivitiesSameStartingHourForm::~ConstraintActivitiesSameStartingHourForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesSameStartingHourForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR)
		return false;

	const ConstraintActivitiesSameStartingHour* c=(const ConstraintActivitiesSameStartingHour*) ctr;
	QSet<const Activity *> activities;
	foreach(int id, c->activitiesId){
		foreach(const Activity* a, gt.rules.activitiesList) {
			if(a->id==id) {
				activities << a;
				break;
			}
		}
	}

	TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	return filter_widget->filterActivitySet(activities);
}

QDialog * ConstraintActivitiesSameStartingHourForm::createAddDialog()
{
	return new AddConstraintActivitiesSameStartingHourForm(this);
}

QDialog * ConstraintActivitiesSameStartingHourForm::createModifyDialog(TimeConstraint* ctr)
{
	return new ModifyConstraintActivitiesSameStartingHourForm(this, (ConstraintActivitiesSameStartingHour*)ctr);
}
