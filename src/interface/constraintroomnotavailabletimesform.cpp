/***************************************************************************
                          constraintroomnotavailabletimesform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "constraintroomnotavailabletimesform.h"
#include "addconstraintroomnotavailabletimesform.h"
#include "modifyconstraintroomnotavailabletimesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintRoomNotAvailableTimesForm::ConstraintRoomNotAvailableTimesForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintRoomNotAvailableTimesForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints room not available times"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintRoomNotAvailableTimesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintRoomNotAvailableTimesForm::~ConstraintRoomNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintRoomNotAvailableTimesForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
		ConstraintRoomNotAvailableTimes* c=(ConstraintRoomNotAvailableTimes*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room=filterWidget->room();
		return (c->room==room || room=="");
	}
	else
		return false;
}

QDialog * ConstraintRoomNotAvailableTimesForm::createAddDialog()
{
	return new AddConstraintRoomNotAvailableTimesForm(this);
}

QDialog * ConstraintRoomNotAvailableTimesForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintRoomNotAvailableTimesForm(this, (ConstraintRoomNotAvailableTimes*)ctr);
}
