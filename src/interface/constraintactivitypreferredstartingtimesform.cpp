/***************************************************************************
                          constraintactivitypreferredstartingtimesform.cpp  -  description
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

#include "constraintactivitypreferredstartingtimesform.h"
#include "addconstraintactivitypreferredstartingtimesform.h"
#include "modifyconstraintactivitypreferredstartingtimesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"


ConstraintActivityPreferredStartingTimesForm::ConstraintActivityPreferredStartingTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivityPreferredStartingTimesForm_template", "Constraints activity preferred starting times"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityPreferredStartingTimesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityPreferredStartingTimesForm::~ConstraintActivityPreferredStartingTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredStartingTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES)
		return false;
		
	const ConstraintActivityPreferredStartingTimes* c=(const ConstraintActivityPreferredStartingTimes*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();
		
	int id=c->activityId;
	const Activity* act=NULL;
	foreach(const Activity* a, gt.rules.activitiesList) {
		if(a->id==id) {
			act=a;
			break;
		}
	}

	bool found=true;

	if(act!=NULL){
		//teacher
		if(!tn.isEmpty() && !act->teachersNames.contains(tn))
			found=false;

		//subject
		if(sbn!="" && sbn!=act->subjectName)
			found=false;

		//activity tag
		if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
			found=false;

		//students
		if(stn!="" && !act->studentsNames.contains(stn))
			found=false;
	}
	
	if(found)
		return true;

	return false;
}

QDialog * ConstraintActivityPreferredStartingTimesForm::createAddDialog()
{
	return new AddConstraintActivityPreferredStartingTimesForm(this);
}

QDialog * ConstraintActivityPreferredStartingTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivityPreferredStartingTimesForm(this, (ConstraintActivityPreferredStartingTimes*)ctr);
}
