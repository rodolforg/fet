/***************************************************************************
                          constraintsubjectactivitytagpreferredroomform.cpp  -  description
                             -------------------
    begin                : Aug 18, 2007
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

#include "constraintsubjectactivitytagpreferredroomform.h"
#include "addconstraintsubjectactivitytagpreferredroomform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintSubjectActivityTagPreferredRoomForm::ConstraintSubjectActivityTagPreferredRoomForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintSubjectActivityTagPreferredRoomForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints subject activity tag preferred room"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setRoomsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintSubjectActivityTagPreferredRoomForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintSubjectActivityTagPreferredRoomForm::~ConstraintSubjectActivityTagPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintSubjectActivityTagPreferredRoomForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
		ConstraintSubjectActivityTagPreferredRoom* c=(ConstraintSubjectActivityTagPreferredRoom*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString subject = filterWidget->subject();
		QString activityTag = filterWidget->activityTag();
		return (c->roomName==room || room.isEmpty())
		 && (c->subjectName==subject || subject.isEmpty())
		 && (c->activityTagName==activityTag || activityTag.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintSubjectActivityTagPreferredRoomForm::createAddDialog()
{
	return new AddConstraintSubjectActivityTagPreferredRoomForm(this);
}

QDialog * ConstraintSubjectActivityTagPreferredRoomForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintSubjectActivityTagPreferredRoomForm(this, (ConstraintSubjectActivityTagPreferredRoom*)ctr);
}
