/***************************************************************************
                          constraintteacherhomeroomform.cpp  -  description
                             -------------------
    begin                : Apr 8, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "constraintteacherhomeroomform.h"
#include "addconstraintteacherhomeroomform.h"
#include "modifyconstraintteacherhomeroomform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherHomeRoomForm::ConstraintTeacherHomeRoomForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeacherHomeRoomForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teacher home room"));

	setInstructionText(QCoreApplication::translate(context, "Note: a home room for a teacher means that every activity which has this and ONLY this teacher will have this as home room. Preferred rooms override the home rooms."));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherHomeRoomForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherHomeRoomForm::~ConstraintTeacherHomeRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherHomeRoomForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
		ConstraintTeacherHomeRoom* c=(ConstraintTeacherHomeRoom*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString teacher = filterWidget->teacher();
		return (c->roomName==room || room.isEmpty())
		 && (c->teacherName==teacher || teacher.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherHomeRoomForm::createAddDialog()
{
	return new AddConstraintTeacherHomeRoomForm(this);
}

QDialog * ConstraintTeacherHomeRoomForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeacherHomeRoomForm(this, (ConstraintTeacherHomeRoom*)ctr);
}
