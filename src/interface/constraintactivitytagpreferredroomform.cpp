/***************************************************************************
                          constraintactivitytagpreferredroomform.cpp  -  description
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

#include "constraintactivitytagpreferredroomform.h"
#include "addconstraintactivitytagpreferredroomform.h"
#include "modifyconstraintactivitytagpreferredroomform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivityTagPreferredRoomForm::ConstraintActivityTagPreferredRoomForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintActivityTagPreferredRoomForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints activity tag preferred room"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityTagPreferredRoomForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityTagPreferredRoomForm::~ConstraintActivityTagPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityTagPreferredRoomForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
		ConstraintActivityTagPreferredRoom* c=(ConstraintActivityTagPreferredRoom*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString activityTag=filterWidget->activityTag();
		QString room=filterWidget->room();
		return (c->roomName==room || room.isEmpty())
		 && (c->activityTagName==activityTag || activityTag.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintActivityTagPreferredRoomForm::createAddDialog()
{
	return new AddConstraintActivityTagPreferredRoomForm(this);
}

QDialog * ConstraintActivityTagPreferredRoomForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintActivityTagPreferredRoomForm(this, (ConstraintActivityTagPreferredRoom*)ctr);
}
