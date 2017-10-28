/***************************************************************************
                          constraintsubjectactivitytagpreferredroomsform.cpp  -  description
                             -------------------
    begin                : 18 Aug 2007
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

#include "constraintsubjectactivitytagpreferredroomsform.h"
#include "addconstraintsubjectactivitytagpreferredroomsform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintSubjectActivityTagPreferredRoomsForm::ConstraintSubjectActivityTagPreferredRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintSubjectActivityTagPreferredRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints subject activity tag preferred rooms"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintSubjectActivityTagPreferredRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintSubjectActivityTagPreferredRoomsForm::~ConstraintSubjectActivityTagPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintSubjectActivityTagPreferredRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString subject = filterWidget->subject();
		QString activityTag = filterWidget->activityTag();
		return (c->subjectName==subject || subject.isEmpty())
		 && (c->activityTagName==activityTag || activityTag.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintSubjectActivityTagPreferredRoomsForm::createAddDialog()
{
	return new AddConstraintSubjectActivityTagPreferredRoomsForm(this);
}

QDialog * ConstraintSubjectActivityTagPreferredRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintSubjectActivityTagPreferredRoomsForm(this, (ConstraintSubjectActivityTagPreferredRooms*)ctr);
}
