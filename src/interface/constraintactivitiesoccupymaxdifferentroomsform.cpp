/***************************************************************************
                          constraintactivitiesoccupymaxdifferentroomsform.cpp  -  description
                             -------------------
    begin                : Apr 29, 2012
    copyright            : (C) 2012 by Lalescu Liviu
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

#include "constraintactivitiesoccupymaxdifferentroomsform.h"
#include "addconstraintactivitiesoccupymaxdifferentroomsform.h"
#include "modifyconstraintactivitiesoccupymaxdifferentroomsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivitiesOccupyMaxDifferentRoomsForm::ConstraintActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintActivitiesOccupyMaxDifferentRoomsForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints activities occupy max different rooms"));

	QString s = QCoreApplication::translate(context, "IMPORTANT: Please be careful with this constraint, because the generation may become too slow or impossible if you overuse it. Press Help for more details.");
	setInstructionText(s);
	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged);

	restoreFETDialogGeometry(this);

	this->filterChanged();
}

ConstraintActivitiesOccupyMaxDifferentRoomsForm::~ConstraintActivitiesOccupyMaxDifferentRoomsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesOccupyMaxDifferentRoomsForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS)
		return false;

	ConstraintActivitiesOccupyMaxDifferentRooms* c=(ConstraintActivitiesOccupyMaxDifferentRooms*) ctr;

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

QDialog * ConstraintActivitiesOccupyMaxDifferentRoomsForm::createAddDialog()
{
	return new AddConstraintActivitiesOccupyMaxDifferentRoomsForm(this);
}

QDialog * ConstraintActivitiesOccupyMaxDifferentRoomsForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm(this, (ConstraintActivitiesOccupyMaxDifferentRooms*)ctr);
}

void ConstraintActivitiesOccupyMaxDifferentRoomsForm::setHelp()
{
	QString s;
	
	s+=tr("IMPORTANT NOTE: Please be careful with this constraint, as its overuse may lead to a slowdown of generation or even to an impossible timetable.");
	s+=QString("\n\n");
	s+=tr("This constraint type was added on 29 April 2012.");
	s+=QString("\n\n");
	s+=tr("To use this constraint, you need to specify a set of activities and the maximum number of different rooms which can be occupied by these "
	 "activities. Note that each activity can take place in a single room, and a single room can hold a single activity at a certain moment. "
	 "This constraint may be used for instance to constrain all activities of type Physics of a certain students set to be in the same room "
	 "throughout the week, by adding all Physics activities for this students set and making max different rooms = 1.");

	setHelpText(s);
}
