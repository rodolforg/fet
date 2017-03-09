/***************************************************************************
                          constraintteachersactivitytagmaxhoursdailyform.cpp  -  description
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

#include "constraintteachersactivitytagmaxhoursdailyform.h"
#include "addconstraintteachersactivitytagmaxhoursdailyform.h"
#include "modifyconstraintteachersactivitytagmaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeachersActivityTagMaxHoursDailyForm::ConstraintTeachersActivityTagMaxHoursDailyForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeachersActivityTagMaxHoursDailyForm_template", "Constraints teachers activity tag max hours daily"));

	QString s = QCoreApplication::translate("ConstraintTeachersActivityTagMaxHoursDailyForm_template", "This constraint ensures that all teachers do not have more than max hours daily of activities with the specified activity tag.");
	s += "\n\n" + QCoreApplication::translate("ConstraintTeachersActivityTagMaxHoursDailyForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
	setInstructionText(s);

	s = HelpOnImperfectConstraints::getHelpText();
	setHelpText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeachersActivityTagMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeachersActivityTagMaxHoursDailyForm::~ConstraintTeachersActivityTagMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeachersActivityTagMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ConstraintTeachersActivityTagMaxHoursDaily* ct=(ConstraintTeachersActivityTagMaxHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString activityTagName = filter_widget->activityTag();
		return (ct->activityTagName==activityTagName || activityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeachersActivityTagMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeachersActivityTagMaxHoursDailyForm(this);
}

QDialog * ConstraintTeachersActivityTagMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeachersActivityTagMaxHoursDailyForm(this, (ConstraintTeachersActivityTagMaxHoursDaily*)ctr);
}
