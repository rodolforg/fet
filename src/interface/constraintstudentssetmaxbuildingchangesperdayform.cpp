/***************************************************************************
                          constraintstudentssetmaxbuildingchangesperdayform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "constraintstudentssetmaxbuildingchangesperdayform.h"
#include "addconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetMaxBuildingChangesPerDayForm::ConstraintStudentsSetMaxBuildingChangesPerDayForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsSetMaxBuildingChangesPerDayForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students set max building changes per day"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxBuildingChangesPerDayForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMaxBuildingChangesPerDayForm::~ConstraintStudentsSetMaxBuildingChangesPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
		ConstraintStudentsSetMaxBuildingChangesPerDay* c=(ConstraintStudentsSetMaxBuildingChangesPerDay*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsSet=filterWidget->studentsSet();
		return c->studentsName==studentsSet || studentsSet.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxBuildingChangesPerDayForm::createAddDialog()
{
	return new AddConstraintStudentsSetMaxBuildingChangesPerDayForm(this);
}

QDialog * ConstraintStudentsSetMaxBuildingChangesPerDayForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm(this, (ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr);
}
