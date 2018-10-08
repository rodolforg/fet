/***************************************************************************
                          constraintactivitypreferredroomsform.cpp  -  description
                             -------------------
    begin                : 28 March 2005
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

#include "constraintactivitypreferredroomsform.h"
#include "addconstraintactivitypreferredroomsform.h"
#include "modifyconstraintactivitypreferredroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivityPreferredRoomsForm::ConstraintActivityPreferredRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintActivityPreferredRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints activity preferred rooms"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityPreferredRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityPreferredRoomsForm::~ConstraintActivityPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS)
		return false;
		
	const ConstraintActivityPreferredRooms* c=(const ConstraintActivityPreferredRooms*) ctr;
	int id=c->activityId;
	const Activity* act=NULL;
	for(const Activity* a : qAsConst(gt.rules.activitiesList)) {
		if(a->id==id) {
			act=a;
			break;
		}
	}
	assert(act != NULL);

	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	return filterWidget->filterActivity(act);
}

QDialog * ConstraintActivityPreferredRoomsForm::createAddDialog()
{
	return new AddConstraintActivityPreferredRoomsForm(this);
}

QDialog * ConstraintActivityPreferredRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintActivityPreferredRoomsForm(this, (ConstraintActivityPreferredRooms*)ctr);
}
