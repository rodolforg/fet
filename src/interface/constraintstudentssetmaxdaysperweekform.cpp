/***************************************************************************
                          constraintstudentssetmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#include "constraintstudentssetmaxdaysperweekform.h"
#include "addconstraintstudentssetmaxdaysperweekform.h"
#include "modifyconstraintstudentssetmaxdaysperweekform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetMaxDaysPerWeekForm::ConstraintStudentsSetMaxDaysPerWeekForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
    //: This is the title of the dialog to see the list of all constraints of this type
    setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMaxDaysPerWeekForm_template", "Constraints students set max days per week"));

    TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
    filterWidget->setStudentSetsVisible(true);
    setFilterWidget(filterWidget);
    connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxDaysPerWeekForm::filterChanged);

    restoreFETDialogGeometry(this);
    this->filterChanged();
}

ConstraintStudentsSetMaxDaysPerWeekForm::~ConstraintStudentsSetMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxDaysPerWeekForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
		ConstraintStudentsSetMaxDaysPerWeek* c=(ConstraintStudentsSetMaxDaysPerWeek*) ctr;
        TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
        QString studentsName = filter_widget->studentsSet();
        return c->students==studentsName || studentsName.isEmpty();
    }
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxDaysPerWeekForm::createAddDialog()
{
    return new AddConstraintStudentsSetMaxDaysPerWeekForm(this);
}

QDialog * ConstraintStudentsSetMaxDaysPerWeekForm::createModifyDialog(TimeConstraint *ctr)
{
    return new ModifyConstraintStudentsSetMaxDaysPerWeekForm(this, (ConstraintStudentsSetMaxDaysPerWeek*)ctr);
}
