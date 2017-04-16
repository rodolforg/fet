/***************************************************************************
                          constraintsubjectpreferredroomsform.cpp  -  description
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

#include "constraintsubjectpreferredroomsform.h"
#include "addconstraintsubjectpreferredroomsform.h"
#include "modifyconstraintsubjectpreferredroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintSubjectPreferredRoomsForm::ConstraintSubjectPreferredRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintSubjectPreferredRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints subject preferred rooms"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setSubjectsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintSubjectPreferredRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintSubjectPreferredRoomsForm::~ConstraintSubjectPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintSubjectPreferredRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
		ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString subject = filterWidget->subject();
		return (c->subjectName==subject || subject.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintSubjectPreferredRoomsForm::createAddDialog()
{
	return new AddConstraintSubjectPreferredRoomsForm(this);
}

QDialog * ConstraintSubjectPreferredRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintSubjectPreferredRoomsForm(this, (ConstraintSubjectPreferredRooms*)ctr);
}
