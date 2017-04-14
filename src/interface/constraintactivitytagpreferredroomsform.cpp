/***************************************************************************
                          constraintactivitytagpreferredroomsform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintactivitytagpreferredroomsform.h"
#include "addconstraintactivitytagpreferredroomsform.h"
#include "modifyconstraintactivitytagpreferredroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivityTagPreferredRoomsForm::ConstraintActivityTagPreferredRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintActivityTagPreferredRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints activity tag preferred rooms"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityTagPreferredRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityTagPreferredRoomsForm::~ConstraintActivityTagPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityTagPreferredRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ConstraintActivityTagPreferredRooms* c=(ConstraintActivityTagPreferredRooms*)ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString activityTag=filterWidget->activityTag();
		return (c->activityTagName==activityTag || activityTag=="");
	}
	else
		return false;
}

QDialog * ConstraintActivityTagPreferredRoomsForm::createAddDialog()
{
	return new AddConstraintActivityTagPreferredRoomsForm(this);
}

QDialog * ConstraintActivityTagPreferredRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintActivityTagPreferredRoomsForm(this, (ConstraintActivityTagPreferredRooms*)ctr);
}
