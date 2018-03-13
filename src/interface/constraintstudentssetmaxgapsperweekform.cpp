/***************************************************************************
                          constraintstudentssetmaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#include "constraintstudentssetmaxgapsperweekform.h"
#include "addconstraintstudentssetmaxgapsperweekform.h"
#include "modifyconstraintstudentssetmaxgapsperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintStudentsSetMaxGapsPerWeekForm::ConstraintStudentsSetMaxGapsPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
    //: This is the title of the dialog to see the list of all constraints of this type
    setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMaxGapsPerWeekForm_template", "Constraints students set max gaps per week"));

    TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
    filterWidget->setStudentSetsVisible(true);
    setFilterWidget(filterWidget);
    connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxGapsPerWeekForm::filterChanged);

    restoreFETDialogGeometry(this);
    this->filterChanged();
}

ConstraintStudentsSetMaxGapsPerWeekForm::~ConstraintStudentsSetMaxGapsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxGapsPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
		ConstraintStudentsSetMaxGapsPerWeek* c=(ConstraintStudentsSetMaxGapsPerWeek*) ctr;
        TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
        QString studentsName = filter_widget->studentsSet();
        return c->students==studentsName || studentsName.isEmpty();
    }
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxGapsPerWeekForm::createAddDialog()
{
    return new AddConstraintStudentsSetMaxGapsPerWeekForm(this);
}

QDialog * ConstraintStudentsSetMaxGapsPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
    return new ModifyConstraintStudentsSetMaxGapsPerWeekForm(this, (ConstraintStudentsSetMaxGapsPerWeek*)ctr);
}
