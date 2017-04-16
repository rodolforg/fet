/***************************************************************************
                          constraintstudentssethomeroomsform.cpp  -  description
                             -------------------
    begin                : 8 April 2005
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

#include "constraintstudentssethomeroomsform.h"
#include "addconstraintstudentssethomeroomsform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetHomeRoomsForm::ConstraintStudentsSetHomeRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsSetHomeRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students set home rooms"));

	setInstructionText(QCoreApplication::translate(context, "Note: a home room for a students set means that every activity which has this and ONLY this students set will have these home rooms (not other superior or inferior set). You can add home rooms for a group, for a year or for a subgroup, careful not to make impossible timetables. Preferred rooms override the home rooms."));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetHomeRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetHomeRoomsForm::~ConstraintStudentsSetHomeRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetHomeRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ConstraintStudentsSetHomeRooms* c=(ConstraintStudentsSetHomeRooms*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString studentsSet = filterWidget->studentsSet();
		return (room.isEmpty() || c->roomsNames.contains(room))
		 && (c->studentsName==studentsSet || studentsSet.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetHomeRoomsForm::createAddDialog()
{
	return new AddConstraintStudentsSetHomeRoomsForm(this);
}

QDialog * ConstraintStudentsSetHomeRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsSetHomeRoomsForm(this, (ConstraintStudentsSetHomeRooms*)ctr);
}
