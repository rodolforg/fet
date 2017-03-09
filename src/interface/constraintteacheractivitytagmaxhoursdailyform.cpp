/***************************************************************************
                          constraintteacheractivitytagmaxhoursdailyform.cpp  -  description
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

#include "constraintteacheractivitytagmaxhoursdailyform.h"
#include "addconstraintteacheractivitytagmaxhoursdailyform.h"
#include "modifyconstraintteacheractivitytagmaxhoursdailyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTeacherActivityTagMaxHoursDailyForm::ConstraintTeacherActivityTagMaxHoursDailyForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTeacherActivityTagMaxHoursDailyForm_template", "Constraints teacher activity tag max hours daily"));

	QString s = QCoreApplication::translate("ConstraintTeacherActivityTagMaxHoursDailyForm_template", "This constraint ensures that the specified teacher does not have more than max hours daily of activities with the specified activity tag.");
	s += "\n\n" + QCoreApplication::translate("ConstraintTeacherActivityTagMaxHoursDailyForm_template", "IMPORTANT: this constraint is not perfectly optimized. Press Help button for more information. Use with caution, as explained.");
	setInstructionText(s);

	s = HelpOnImperfectConstraints::getHelpText();
	setHelpText(s);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTeacherActivityTagMaxHoursDailyForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintTeacherActivityTagMaxHoursDailyForm::~ConstraintTeacherActivityTagMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherActivityTagMaxHoursDailyForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ConstraintTeacherActivityTagMaxHoursDaily* ct=(ConstraintTeacherActivityTagMaxHoursDaily*) ctr;
		TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
		QString teacherName = filter_widget->teacher();
		QString activityTagName = filter_widget->activityTag();
		return (ct->teacherName==teacherName || teacherName.isEmpty())
		 &&
		 (ct->activityTagName==activityTagName || activityTagName.isEmpty());
	}
	else
		return false;
}

QDialog * ConstraintTeacherActivityTagMaxHoursDailyForm::createAddDialog()
{
	return new AddConstraintTeacherActivityTagMaxHoursDailyForm(this);
}

QDialog * ConstraintTeacherActivityTagMaxHoursDailyForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTeacherActivityTagMaxHoursDailyForm(this, (ConstraintTeacherActivityTagMaxHoursDaily*)ctr);
}
