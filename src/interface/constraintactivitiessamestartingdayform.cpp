/***************************************************************************
                          constraintactivitiessamestartingdayform.cpp  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#include "constraintactivitiessamestartingdayform.h"
#include "addconstraintactivitiessamestartingdayform.h"
#include "modifyconstraintactivitiessamestartingdayform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "centerwidgetonscreen.h"

ConstraintActivitiesSameStartingDayForm::ConstraintActivitiesSameStartingDayForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesSameStartingDayForm_template", "Constraints activities same starting day"));

	setInstructionText(QCoreApplication::translate("ConstraintActivitiesSameStartingDayForm_template", "Please try to read Help/Important tips, advice 2). It is IMPORTANT to remove redundant min days constraints after adding constraints same starting day. Please also click Help button here for information."));
	setHelp();

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesSameStartingDayForm::filterChanged);

	filterChanged();
}

ConstraintActivitiesSameStartingDayForm::~ConstraintActivitiesSameStartingDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesSameStartingDayForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY)
		return false;

	const ConstraintActivitiesSameStartingDay* c=(const ConstraintActivitiesSameStartingDay*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();

	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;

	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;

	for(int i=0; i<c->n_activities; i++){
		//bool found=true;

		int id=c->activitiesId[i];
		const Activity* act=NULL;
		foreach(const Activity* a, gt.rules.activitiesList) {
			if(a->id==id) {
				act=a;
				break;
			}
		}

		if(act!=NULL){
			//teacher
			if(tn.isEmpty() || act->teachersNames.contains(tn))
				foundTeacher=true;

			//subject
			if(sbn.isEmpty() || sbn==act->subjectName)
				foundSubject=true;

			//activity tag
			if(sbtn.isEmpty() || act->activityTagsNames.contains(sbtn))
				foundActivityTag=true;

			//students
			if(stn.isEmpty() || act->studentsNames.contains(stn))
				foundStudents=true;
		}
	}

	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesSameStartingDayForm::createAddDialog()
{
	return new AddConstraintActivitiesSameStartingDayForm(this);
}

QDialog * ConstraintActivitiesSameStartingDayForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesSameStartingDayForm(this, (ConstraintActivitiesSameStartingDay*)ctr);
}

void ConstraintActivitiesSameStartingDayForm::setHelp()
{
	QString s;

	s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
	" min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
	" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
	" Please read Help/Important tips - tip number 2 for details");

	setHelpText(s);
}
