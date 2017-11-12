/***************************************************************************
                          constraintactivitypreferredstartingtimeform.cpp  -  description
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

#include "constraintactivitypreferredstartingtimeform.h"
#include "addconstraintactivitypreferredstartingtimeform.h"
#include "modifyconstraintactivitypreferredstartingtimeform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

#include "lockunlock.h"

ConstraintActivityPreferredStartingTimeForm::ConstraintActivityPreferredStartingTimeForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivityPreferredStartingTimeForm_template", "Constraints activity preferred starting time"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivityPreferredStartingTimeForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivityPreferredStartingTimeForm::~ConstraintActivityPreferredStartingTimeForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredStartingTimeForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
		return false;
		
	ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();
		
	bool found=true;
	
	int id=c->activityId;
	Activity* act=NULL;
	foreach(Activity* a, gt.rules.activitiesList) {
		if(a->id==id) {
			act=a;
			break;
		}
	}

	found=true;		
		
	if(act!=NULL){
		//teacher
		if(tn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
				if(*it == tn){
					ok2=true;
					break;
				}
			if(!ok2)
				found=false;
		}

		//subject
		if(sbn!="" && sbn!=act->subjectName)
			found=false;
	
		//activity tag
		if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
			found=false;
	
		//students
		if(stn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
				if(*it == stn){
					ok2=true;
					break;
			}
			if(!ok2)
				found=false;
		}
	}
	
	if(found)
		return true;

	return false;
}

QDialog * ConstraintActivityPreferredStartingTimeForm::createAddDialog()
{
	return new AddConstraintActivityPreferredStartingTimeForm(this);
}

QDialog * ConstraintActivityPreferredStartingTimeForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivityPreferredStartingTimeForm(this, (ConstraintActivityPreferredStartingTime*)ctr);
}

void ConstraintActivityPreferredStartingTimeForm::afterRemoveConstraint()
{
	LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
	LockUnlock::increaseCommunicationSpinBox();
}
