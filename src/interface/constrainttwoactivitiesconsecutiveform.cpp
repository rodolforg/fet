/***************************************************************************
                          constrainttwoactivitiesconsecutiveform.cpp  -  description
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

#include "constrainttwoactivitiesconsecutiveform.h"
#include "addconstrainttwoactivitiesconsecutiveform.h"
#include "modifyconstrainttwoactivitiesconsecutiveform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTwoActivitiesConsecutiveForm::ConstraintTwoActivitiesConsecutiveForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "Constraints two activies consecutive"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "This constraint forces two activities A1 and A2: A2 to be immediately after A1, same day, possibly separated by breaks");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintTwoActivitiesConsecutiveForm_template", "It is a good practice NOT to add this type of constraint to activities which are constrained to be in different days by min days between activities constraints. Please read Help/Important tips, tip number 3).");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTwoActivitiesConsecutiveForm::filterChanged);

	this->filterChanged();
}

ConstraintTwoActivitiesConsecutiveForm::~ConstraintTwoActivitiesConsecutiveForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesConsecutiveForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE)
		return false;

	const ConstraintTwoActivitiesConsecutive* c=(const ConstraintTwoActivitiesConsecutive*) ctr;
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

QDialog * ConstraintTwoActivitiesConsecutiveForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesConsecutiveForm(this);
}

QDialog * ConstraintTwoActivitiesConsecutiveForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesConsecutiveForm(this, (ConstraintTwoActivitiesConsecutive*)ctr);
}
