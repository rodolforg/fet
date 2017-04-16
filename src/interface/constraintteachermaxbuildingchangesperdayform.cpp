/***************************************************************************
                          constraintteachermaxbuildingchangesperdayform.cpp  -  description
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

#include "constraintteachermaxbuildingchangesperdayform.h"
#include "addconstraintteachermaxbuildingchangesperdayform.h"
#include "modifyconstraintteachermaxbuildingchangesperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherMaxBuildingChangesPerDayForm::ConstraintTeacherMaxBuildingChangesPerDayForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintTeacherMaxBuildingChangesPerDayForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints teacher max building changes per day"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMaxBuildingChangesPerDayForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherMaxBuildingChangesPerDayForm::~ConstraintTeacherMaxBuildingChangesPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxBuildingChangesPerDayForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
		ConstraintTeacherMaxBuildingChangesPerDay* c=(ConstraintTeacherMaxBuildingChangesPerDay*) ctr;
		const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacher=filterWidget->teacher();
		return c->teacherName==teacher || teacher.isEmpty();
	}
	else
		return false;
}

QDialog * ConstraintTeacherMaxBuildingChangesPerDayForm::createAddDialog()
{
	return new AddConstraintTeacherMaxBuildingChangesPerDayForm(this);
}

QDialog * ConstraintTeacherMaxBuildingChangesPerDayForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintTeacherMaxBuildingChangesPerDayForm(this, (ConstraintTeacherMaxBuildingChangesPerDay*)ctr);
}
