/***************************************************************************
                          constraintteachermingapsbetweenbuildingchangesform.cpp  -  description
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

#include "constraintteachermingapsbetweenbuildingchangesform.h"
#include "addconstraintteachermingapsbetweenbuildingchangesform.h"
#include "modifyconstraintteachermingapsbetweenbuildingchangesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMinGapsBetweenBuildingChangesForm::ConstraintTeacherMinGapsBetweenBuildingChangesForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeacherMinGapsBetweenBuildingChangesForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teacher min gaps between building changes"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMinGapsBetweenBuildingChangesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMinGapsBetweenBuildingChangesForm::~ConstraintTeacherMinGapsBetweenBuildingChangesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMinGapsBetweenBuildingChangesForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ConstraintTeacherMinGapsBetweenBuildingChanges* c=(ConstraintTeacherMinGapsBetweenBuildingChanges*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacher=filterWidget->teacher();
		return c->teacherName==teacher || teacher.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintTeacherMinGapsBetweenBuildingChangesForm::createAddDialog()
{
	return new AddConstraintTeacherMinGapsBetweenBuildingChangesForm(this);
}

QDialog * ConstraintTeacherMinGapsBetweenBuildingChangesForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeacherMinGapsBetweenBuildingChangesForm(this, (ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr);
}
