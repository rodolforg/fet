/***************************************************************************
                          constraintstudentssetmaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "helponimperfectconstraints.h"

#include "constraintstudentssetmaxgapsperdayform.h"
#include "addconstraintstudentssetmaxgapsperdayform.h"
#include "modifyconstraintstudentssetmaxgapsperdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintStudentsSetMaxGapsPerDayForm::ConstraintStudentsSetMaxGapsPerDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
    //: This is the title of the dialog to see the list of all constraints of this type
    setWindowTitle(QCoreApplication::translate("ConstraintStudentsSetMaxGapsPerDayForm_template", "Constraints students set max gaps per day"));

    QString s = QCoreApplication::translate("ConstraintStudentsSetMaxGapsPerDayForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
    setInstructionText(s);

    s = HelpOnImperfectConstraints::getHelpText();
    setHelpText(s);

    TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
    filterWidget->setStudentSetsVisible(true);
    setFilterWidget(filterWidget);
    connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintStudentsSetMaxGapsPerDayForm::filterChanged);

    restoreFETDialogGeometry(this);
    this->filterChanged();
}

ConstraintStudentsSetMaxGapsPerDayForm::~ConstraintStudentsSetMaxGapsPerDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxGapsPerDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
		ConstraintStudentsSetMaxGapsPerDay* c=(ConstraintStudentsSetMaxGapsPerDay*) ctr;
        TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
        QString studentsName = filter_widget->studentsSet();
        return c->students==studentsName || studentsName.isEmpty();
    }
	else
		return false;
}

QDialog * ConstraintStudentsSetMaxGapsPerDayForm::createAddDialog()
{
    return new AddConstraintStudentsSetMaxGapsPerDayForm(this);
}

QDialog * ConstraintStudentsSetMaxGapsPerDayForm::createModifyDialog(TimeConstraint *ctr)
{
    return new ModifyConstraintStudentsSetMaxGapsPerDayForm(this, (ConstraintStudentsSetMaxGapsPerDay*)ctr);
}
