/***************************************************************************
                          constraintactivitiessamestartingtimeform.cpp  -  description
                             -------------------
    begin                : 23 June 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "constraintactivitiessamestartingtimeform.h"
#include "addconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartingtimeform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivitiesSameStartingTimeForm::ConstraintActivitiesSameStartingTimeForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesSameStartingTimeForm_template", "Constraints activities same starting time"));

	QString instruction = QCoreApplication::translate("ConstraintActivitiesSameStartingTimeForm_template", "Please read Help/Important tips, advice 2). It is IMPORTANT to remove redundant min days constraints after adding constraints same starting time. Click the Help button!");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintActivitiesSameStartingTimeForm_template", "NOTE: You might not need this constraint. Press Help!");
	setInstructionText(instruction);

	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesSameStartingTimeForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivitiesSameStartingTimeForm::~ConstraintActivitiesSameStartingTimeForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesSameStartingTimeForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME)
		return false;
		
	const ConstraintActivitiesSameStartingTime* c=(const ConstraintActivitiesSameStartingTime*) ctr;
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
		Activity* act=NULL;
		foreach(Activity* a, gt.rules.activitiesList) {
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

QDialog * ConstraintActivitiesSameStartingTimeForm::createAddDialog()
{
	return new AddConstraintActivitiesSameStartingTimeForm(this);
}

QDialog * ConstraintActivitiesSameStartingTimeForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesSameStartingTimeForm(this, (ConstraintActivitiesSameStartingTime*)ctr);
}

void ConstraintActivitiesSameStartingTimeForm::setHelp()
{
	QString s;

	s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
	 " min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
	 " Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
	 " Please read Help/Important tips - tip number 2 for details");
	s+="\n\n";
	s+=tr("If you divide the years by categories (the students have optional courses), you may not need this kind of constraint. If the year is for instance"
	 " divided by language, English/French/German, sometimes people think that it is needed that these optional activities to have the same starting"
	 " time. But solutions exist without adding same starting time constraints. Some people try to avoid students gaps by using this"
	 " constraint, because they need to do it this way in other timetabling software. But it is not the best practice to do it with such a"
	 " constraint. The correct way is to add a max 0 gaps per week constraint for students and maybe also an early max 0 beginnings at second"
	 " hour constraint for students.");

	setHelpText(s);
}
