/***************************************************************************
                          constraintactivitiesnotoverlappingform.cpp  -  description
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

#include "constraintactivitiesnotoverlappingform.h"
#include "addconstraintactivitiesnotoverlappingform.h"
#include "modifyconstraintactivitiesnotoverlappingform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivitiesNotOverlappingForm::ConstraintActivitiesNotOverlappingForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesNotOverlappingForm_template", "Constraints activities not overlapping"));

//	setInstructionText(QCoreApplication::translate("ConstraintActivitiesNotOverlappingForm_template", ""));
//	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesNotOverlappingForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivitiesNotOverlappingForm::~ConstraintActivitiesNotOverlappingForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesNotOverlappingForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING)
		return false;

	const ConstraintActivitiesNotOverlapping* c=(const ConstraintActivitiesNotOverlapping*) ctr;

	QSet<const Activity *> activities;
	for(int id : qAsConst(c->activitiesId)){
		for(const Activity* a : qAsConst(gt.rules.activitiesList)) {
			if(a->id==id) {
				activities << a;
				break;
			}
		}
	}

	TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	return filter_widget->filterActivitySet(activities);
}

QDialog * ConstraintActivitiesNotOverlappingForm::createAddDialog()
{
	return new AddConstraintActivitiesNotOverlappingForm(this);
}

QDialog * ConstraintActivitiesNotOverlappingForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesNotOverlappingForm(this, (ConstraintActivitiesNotOverlapping*)ctr);
}


