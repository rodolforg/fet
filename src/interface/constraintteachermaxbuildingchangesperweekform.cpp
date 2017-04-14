/***************************************************************************
                          constraintteachermaxbuildingchangesperweekform.cpp  -  description
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

#include "constraintteachermaxbuildingchangesperweekform.h"
#include "addconstraintteachermaxbuildingchangesperweekform.h"
#include "modifyconstraintteachermaxbuildingchangesperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxBuildingChangesPerWeekForm::ConstraintTeacherMaxBuildingChangesPerWeekForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeacherMaxBuildingChangesPerWeekForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teacher max building changes per week"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxBuildingChangesPerWeekForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxBuildingChangesPerWeekForm::~ConstraintTeacherMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxBuildingChangesPerWeekForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
		ConstraintTeacherMaxBuildingChangesPerWeek* c=(ConstraintTeacherMaxBuildingChangesPerWeek*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacher=filterWidget->teacher();
		return c->teacherName==teacher || teacher.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxBuildingChangesPerWeekForm::createAddDialog()
{
	return new AddConstraintTeacherMaxBuildingChangesPerWeekForm(this);
}

QDialog * ConstraintTeacherMaxBuildingChangesPerWeekForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxBuildingChangesPerWeekForm(this, (ConstraintTeacherMaxBuildingChangesPerWeek*)ctr);
}
