/***************************************************************************
                          constraintstudentssetmingapsbetweenbuildingchangesform.cpp  -  description
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

#include "constraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "addconstraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::ConstraintStudentsSetMinGapsBetweenBuildingChangesForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints students set min gaps between building changes"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::~ConstraintStudentsSetMinGapsBetweenBuildingChangesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ConstraintStudentsSetMinGapsBetweenBuildingChanges* c=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString studentsSet=filterWidget->studentsSet();
		return c->studentsName==studentsSet || studentsSet.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::createAddDialog()
{
	return new AddConstraintStudentsSetMinGapsBetweenBuildingChangesForm(this);
}

QDialog * ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm(this, (ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
}
