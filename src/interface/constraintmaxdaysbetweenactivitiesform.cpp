/***************************************************************************
                          constraintmaxdaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "constraintmaxdaysbetweenactivitiesform.h"
#include "addconstraintmaxdaysbetweenactivitiesform.h"
#include "modifyconstraintmaxdaysbetweenactivitiesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintMaxDaysBetweenActivitiesForm::ConstraintMaxDaysBetweenActivitiesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintMaxDaysBetweenActivitiesForm_template", "Constraints max days between activities"));

	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintMaxDaysBetweenActivitiesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintMaxDaysBetweenActivitiesForm::~ConstraintMaxDaysBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintMaxDaysBetweenActivitiesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES)
		return false;
		
	ConstraintMaxDaysBetweenActivities* c=(ConstraintMaxDaysBetweenActivities*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();
	
	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<c->n_activities; i++){
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

QDialog * ConstraintMaxDaysBetweenActivitiesForm::createAddDialog()
{
	return new AddConstraintMaxDaysBetweenActivitiesForm(this);
}

QDialog * ConstraintMaxDaysBetweenActivitiesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintMaxDaysBetweenActivitiesForm(this, (ConstraintMaxDaysBetweenActivities*)ctr);
}

void ConstraintMaxDaysBetweenActivitiesForm::setHelp()
{
	QString s;
	
	s+=tr("Help about the constraint max days between activities:");
	s+="\n\n";
	s+=tr("This constraint was suggested for the following situation: a user needed that activities A1, A2 and A3 to be in follower days"
	 " (like: A1 on Tuesday, A2 on Wednesday and A3 on Thursday. So, they must be in 3 consecutive days). This is simple: add a constraint"
	 " max days between activities for A1, A2 and A3, with max 2 days between them. It is supposed that these activities are constrained"
	 " not to be in the same day by a constraint min days between activities.");
	s+="\n\n";
	s+=tr("So, the general situation: this constraint ensures that between each pair from the selected activities, the distance in days is at most the selected value."
	" Distance = 1 day between a pair A1 and A2 means that A1 and A2 are in consecutive days (like Thursday and Friday)."
	" Distance = 3 days means that A1 and A2 are 3 days apart, for instance Monday and Thursday.");
	s+="\n\n";
	s+=tr("Another example: teacher T wants to ensure that his activities take place in at most 4 consecutive days (so, from Monday to Thursday or from"
	" Tuesday to Friday). Then, add all his activities and max days between them = 3.");

	setHelpText(s);
}
