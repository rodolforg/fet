/***************************************************************************
                          constrainttwoactivitiesgroupedform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constrainttwoactivitiesgroupedform.h"
#include "addconstrainttwoactivitiesgroupedform.h"
#include "modifyconstrainttwoactivitiesgroupedform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintTwoActivitiesGroupedForm::ConstraintTwoActivitiesGroupedForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "Constraints two activies grouped"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "This constraint forces two activities to be in the same day, one immediately following the other, in any order, possibly separated by breaks");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintTwoActivitiesGroupedForm_template", "It is a good practice NOT to add this type of constraint to activities which are constrained to be in different days by min days between activities constraints. Please read Help/Important tips, tip number 3).");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTwoActivitiesGroupedForm::filterChanged);

	this->filterChanged();
}

ConstraintTwoActivitiesGroupedForm::~ConstraintTwoActivitiesGroupedForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesGroupedForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_TWO_ACTIVITIES_GROUPED)
		return false;

	const ConstraintTwoActivitiesGrouped* c=(const ConstraintTwoActivitiesGrouped*) ctr;
	QSet<int> activitiesIds;
	activitiesIds << c->firstActivityId << c->secondActivityId;

	QSet<const Activity *> activities;
	for(int id : qAsConst(activitiesIds)){
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

QDialog * ConstraintTwoActivitiesGroupedForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesGroupedForm(this);
}

QDialog * ConstraintTwoActivitiesGroupedForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesGroupedForm(this, (ConstraintTwoActivitiesGrouped*)ctr);
}
