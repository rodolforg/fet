/***************************************************************************
                          constraintsubjectpreferredroomform.cpp  -  description
                             -------------------
    begin                : Apr 8, 2005
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

#include "constraintsubjectpreferredroomform.h"
#include "addconstraintsubjectpreferredroomform.h"
#include "modifyconstraintsubjectpreferredroomform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintSubjectPreferredRoomForm::ConstraintSubjectPreferredRoomForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintSubjectPreferredRoomForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints subject preferred room"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setRoomsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintSubjectPreferredRoomForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintSubjectPreferredRoomForm::~ConstraintSubjectPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintSubjectPreferredRoomForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
		ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString room = filterWidget->room();
		QString subject = filterWidget->subject();
		return (c->roomName==room || room.isEmpty())
		 && (c->subjectName==subject || subject.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintSubjectPreferredRoomForm::createAddDialog()
{
	return new AddConstraintSubjectPreferredRoomForm(this);
}

QDialog * ConstraintSubjectPreferredRoomForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintSubjectPreferredRoomForm(this, (ConstraintSubjectPreferredRoom*)ctr);
}
