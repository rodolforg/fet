/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintteachermincontinuousgapinintervalform.h"
#include "addconstraintteachersmincontinuousgapinintervalform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

#include "fet.h"

ConstraintTeacherMinContinuousGapInIntervalForm::ConstraintTeacherMinContinuousGapInIntervalForm(QWidget *parent) : TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle("Constraints teacher min continuous gap in interval");

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherMinContinuousGapInIntervalForm::filterChanged);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintTeacherMinContinuousGapInIntervalForm::~ConstraintTeacherMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMinContinuousGapInIntervalForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type == CONSTRAINT_TEACHER_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
		ConstraintTeacherMinContinuousGapInInterval *c = (ConstraintTeacherMinContinuousGapInInterval*)ctr;
		QString teacher = ((TeacherStudentSetSubjectActivityTag_FilterWidget*)getFilterWidget())->teacher();
		return teacher.isEmpty() || c->teacherName == teacher;
	}
	else
		return false;
}

QDialog * ConstraintTeacherMinContinuousGapInIntervalForm::createAddDialog()
{
	return new AddConstraintTeachersMinContinuousGapInIntervalForm(this, false);
}

QDialog * ConstraintTeacherMinContinuousGapInIntervalForm::createModifyDialog(TimeConstraint *ctr)
{
	return new AddConstraintTeachersMinContinuousGapInIntervalForm(this, (ConstraintTeacherMinContinuousGapInInterval*)ctr);
}
