/***************************************************************************
                          constraintteacherhomeroomsform.cpp  -  description
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

#include "constraintteacherhomeroomsform.h"
#include "addconstraintteacherhomeroomsform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherHomeRoomsForm::ConstraintTeacherHomeRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeacherHomeRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teacher home rooms"));

	setInstructionText(QCoreApplication::translate(context, "Note: home rooms for a teacher means that every activity which has this and ONLY this teacher will have these home rooms. Preferred rooms override the home rooms."));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherHomeRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherHomeRoomsForm::~ConstraintTeacherHomeRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherHomeRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ConstraintTeacherHomeRooms* c=(ConstraintTeacherHomeRooms*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString teacher = filterWidget->teacher();
		return (room.isEmpty() || c->roomsNames.contains(room))
		 && (c->teacherName==teacher || teacher.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherHomeRoomsForm::createAddDialog()
{
	return new AddConstraintTeacherHomeRoomsForm(this);
}

QDialog * ConstraintTeacherHomeRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeacherHomeRoomsForm(this, (ConstraintTeacherHomeRooms*)ctr);
}
