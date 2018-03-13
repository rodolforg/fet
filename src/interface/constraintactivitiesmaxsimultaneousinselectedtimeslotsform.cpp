/***************************************************************************
                          constraintactivitiesmaxsimultaneousinselectedtimeslotsform.cpp  -  description
                             -------------------
    begin                : Sept 26, 2011
    copyright            : (C) 2011 by Lalescu Liviu
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

#include "constraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"
#include "addconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"
#include "modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm_template", "Constraints activities max simultaneous in selected time slots"));

	setInstructionText(QCoreApplication::translate("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm_template", "IMPORTANT: Please do not overuse this constraint, because the generation may be slowed down very much. Please read Help for more details."));
	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::~ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS)
		return false;

	const ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* c=(const ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*) ctr;

	QSet<const Activity *> activities;
	foreach(int id, c->activitiesIds){
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

QDialog * ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::createAddDialog()
{
	return new AddConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(this);
}

QDialog * ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(this, (ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)ctr);
}

void ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::setHelp()
{
	QString s;

	s+=tr("IMPORTANT NOTE: Please use this constraint ONLY when strictly necessary, when it is really useful, when it has effect, "
	 "when it is not implied by the other constraints, and in a non-redundant way. Otherwise it may slow down very much the generation.");
	s+=QString("\n\n");
	s+=tr("Important note: if you want the maximum number of simultaneous activities to be 1, then it is more efficient to use "
	 "constraint activities not overlapping and the result is the same. (The underlying data structures make FET compute better the initial order "
	 "of placing the activities and also the generation speed is faster, if using constraints of type activities not overlapping.)");
	s+=QString("\n\n");
	s+=tr("This constraint type was added on 30 September 2011.");
	s+=QString("\n\n");
	s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
	 "of activities from those selected which can be simultaneous in each of the selected time slots.");
	s+=QString("\n\n");
	s+=tr("Uses of this constraint are probably wide. The main reason it was added was to enable users to specify that each time slot needs to have "
	 "a specified number of free teachers, for supervision of students.");
	s+=QString(" ");
	s+=tr("Note that if you have activities with two or more teachers, you might need to use a trick for that: add one (or more) dummy activities, "
	 "no teachers/no students, same starting time with the initial activity.");

	setHelpText(s);
}
