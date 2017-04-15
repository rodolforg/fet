/***************************************************************************
                          constraintstudentssethomeroomform.cpp  -  description
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

#include "constraintstudentssethomeroomform.h"
#include "addconstraintstudentssethomeroomform.h"
#include "modifyconstraintstudentssethomeroomform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetHomeRoomForm::ConstraintStudentsSetHomeRoomForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsSetHomeRoomForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students set home room"));

	setInstructionText(QCoreApplication::translate(context, "Note: a home room for a students set means that every activity which has this and ONLY this students set will have this as home room (not other superior or inferior set). You can add home rooms for a group, for a year or for a subgroup, careful not to make impossible timetables. Preferred rooms override the home rooms."));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetHomeRoomForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetHomeRoomForm::~ConstraintStudentsSetHomeRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetHomeRoomForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
		ConstraintStudentsSetHomeRoom* c=(ConstraintStudentsSetHomeRoom*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString studentsSet = filterWidget->studentsSet();
		return (c->roomName==room || room.isEmpty())
		 && (c->studentsName==studentsSet || studentsSet.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetHomeRoomForm::createAddDialog()
{
	return new AddConstraintStudentsSetHomeRoomForm(this);
}

QDialog * ConstraintStudentsSetHomeRoomForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsSetHomeRoomForm(this, (ConstraintStudentsSetHomeRoom*)ctr);
}
