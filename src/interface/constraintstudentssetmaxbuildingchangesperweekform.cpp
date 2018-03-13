/***************************************************************************
                          constraintstudentssetmaxbuildingchangesperweekform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "constraintstudentssetmaxbuildingchangesperweekform.h"
#include "addconstraintstudentssetmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetMaxBuildingChangesPerWeekForm::ConstraintStudentsSetMaxBuildingChangesPerWeekForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsSetMaxBuildingChangesPerWeekForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students set max building changes per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxBuildingChangesPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMaxBuildingChangesPerWeekForm::~ConstraintStudentsSetMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeekForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
		ConstraintStudentsSetMaxBuildingChangesPerWeek* c=(ConstraintStudentsSetMaxBuildingChangesPerWeek*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsSet=filterWidget->studentsSet();
		return c->studentsName==studentsSet || studentsSet.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxBuildingChangesPerWeekForm::createAddDialog()
{
	return new AddConstraintStudentsSetMaxBuildingChangesPerWeekForm(this);
}

QDialog * ConstraintStudentsSetMaxBuildingChangesPerWeekForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm(this, (ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr);
}
