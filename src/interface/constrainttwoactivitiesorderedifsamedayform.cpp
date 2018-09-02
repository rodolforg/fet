/***************************************************************************
                          constrainttwoactivitiesorderedifsamedayform.cpp  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Lalescu Liviu
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

#include "constrainttwoactivitiesorderedifsamedayform.h"
#include "addconstrainttwoactivitiesorderedifsamedayform.h"
#include "modifyconstrainttwoactivitiesorderedifsamedayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintTwoActivitiesOrderedIfSameDayForm::ConstraintTwoActivitiesOrderedIfSameDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesOrderedIfSameDayForm_template", "Constraints two activies ordered"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesOrderedIfSameDayForm_template", "This constraint forces two activities A1 and A2: if they are on the same day, A2 needs to begin later than A1 has finished.");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTwoActivitiesOrderedIfSameDayForm::filterChanged);

	this->filterChanged();
}

ConstraintTwoActivitiesOrderedIfSameDayForm::~ConstraintTwoActivitiesOrderedIfSameDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesOrderedIfSameDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY)
		return false;

	ConstraintTwoActivitiesOrderedIfSameDay* c=(ConstraintTwoActivitiesOrderedIfSameDay*) ctr;
	QSet<int> activitiesIds;
	activitiesIds << c->firstActivityId << c->secondActivityId;

	QSet<const Activity *> activities;
	foreach(int id, activitiesIds){
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

QDialog * ConstraintTwoActivitiesOrderedIfSameDayForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesOrderedIfSameDayForm(this);
}

QDialog * ConstraintTwoActivitiesOrderedIfSameDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesOrderedIfSameDayForm(this, (ConstraintTwoActivitiesOrderedIfSameDay*)ctr);
}
