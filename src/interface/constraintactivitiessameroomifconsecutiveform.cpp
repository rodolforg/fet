/***************************************************************************
                          constraintactivitiessameroomifconsecutiveform.cpp  -  description
                             -------------------
    begin                : Sept 14, 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#include "constraintactivitiessameroomifconsecutiveform.h"
#include "addconstraintactivitiessameroomifconsecutiveform.h"
#include "modifyconstraintactivitiessameroomifconsecutiveform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivitiesSameRoomIfConsecutiveForm::ConstraintActivitiesSameRoomIfConsecutiveForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintActivitiesSameRoomIfConsecutiveForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints activities same room if consecutive"));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesSameRoomIfConsecutiveForm::filterChanged);

	restoreFETDialogGeometry(this);

	this->filterChanged();
}
ConstraintActivitiesSameRoomIfConsecutiveForm::~ConstraintActivitiesSameRoomIfConsecutiveForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesSameRoomIfConsecutiveForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS)
		return false;

	ConstraintActivitiesSameRoomIfConsecutive* c=(ConstraintActivitiesSameRoomIfConsecutive*) ctr;

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());

	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();

	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;

	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;

	for(int i=0; i<c->activitiesIds.count(); i++){
		int id=c->activitiesIds.at(i);
		Activity* act=NULL;
		foreach(Activity* a, gt.rules.activitiesList)
			if(a->id==id)
				act=a;

		if(act!=NULL){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				foundSubject=true;

			//activity tag
			if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
				;
			else
				foundActivityTag=true;

			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
					if(*it == stn){
						ok2=true;
						break;
				}
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
	}

	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

QDialog * ConstraintActivitiesSameRoomIfConsecutiveForm::createAddDialog()
{
	return new AddConstraintActivitiesSameRoomIfConsecutiveForm(this);
}

QDialog * ConstraintActivitiesSameRoomIfConsecutiveForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintActivitiesSameRoomIfConsecutiveForm(this, (ConstraintActivitiesSameRoomIfConsecutive*)ctr);
}
