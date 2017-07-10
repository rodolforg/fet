/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "constraintstudentssetmincontinuousgapinintervalform.h"
#include "addconstraintstudentsmincontinuousgapinintervalform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

#include "fet.h"

ConstraintStudentsSetMinContinuousGapInIntervalForm::ConstraintStudentsSetMinContinuousGapInIntervalForm(QWidget *parent) : TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle("Constraints students set min continuous gap in interval");

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setStudentSetsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMinContinuousGapInIntervalForm::filterChanged);

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintStudentsSetMinContinuousGapInIntervalForm::~ConstraintStudentsSetMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMinContinuousGapInIntervalForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTSSET_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
		ConstraintStudentsSetMinContinuousGapInInterval *c = (ConstraintStudentsSetMinContinuousGapInInterval*)ctr;
		QString students = ((TeacherStudentSetSubjectActivityTag_FilterWidget*)getFilterWidget())->studentsSet();
		return students.isEmpty() || c->students == students;
	}
	else
		return false;
}

QDialog * ConstraintStudentsSetMinContinuousGapInIntervalForm::createAddDialog()
{
	return new AddConstraintStudentsMinContinuousGapInIntervalForm(this, false);
}

QDialog * ConstraintStudentsSetMinContinuousGapInIntervalForm::createModifyDialog(TimeConstraint *ctr)
{
	return new AddConstraintStudentsMinContinuousGapInIntervalForm(this, (ConstraintStudentsSetMinContinuousGapInInterval*)ctr);
}
