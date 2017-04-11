/***************************************************************************
                          constraintactivitiesmaxsimultaneousinselectedtimeslotsform.cpp  -  description
                             -------------------
    begin                : Sept 26, 2011
    copyright            : (C) 2011 by Lalescu Liviu
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

#include "constraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"
#include "addconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"
#include "modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm_template", "Constraints activities max simultaneous in selected time slots"));

	setInstructionText(QCoreApplication::translate("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm_template", "IMPORTANT: Please do not overuse this constraint, because the generation may be slowed down very much. Please read Help for more details."));
	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::~ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS)
		return false;

	const ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* c=(const ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
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

QDialog * ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::createAddDialog()
{
	return new AddConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(this);
}

QDialog * ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm(this, (ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)ctr);
}

void ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm::setHelp()
{
	QString s;

	s+=tr("IMPORTANT NOTE: Please use this constraint ONLY when strictly necessary, when it is really useful, when it has effect, "
	 "when it is not implied by the other constraints, and in a non-redundant way. Otherwise it may slow down very much the generation.");
	s+=QString("\n\n");
	s+=tr("Important note: if you want the maximum number of simultaneous activities to be 1, then it is more efficient to use "
	 "constraint activities not overlapping and the result is the same. (The underlying data structures make FET compute better the initial order "
	 "of placing the activities and also the generation speed is faster, if using constraints of type activities not overlapping.)");
	s+=QString("\n\n");
	s+=tr("This constraint type was added on 30 September 2011.");
	s+=QString("\n\n");
	s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
	 "of activities from those selected which can be simultaneous in each of the selected time slots.");
	s+=QString("\n\n");
	s+=tr("Uses of this constraint are probably wide. The main reason it was added was to enable users to specify that each time slot needs to have "
	 "a specified number of free teachers, for supervision of students.");
	s+=QString(" ");
	s+=tr("Note that if you have activities with two or more teachers, you might need to use a trick for that: add one (or more) dummy activities, "
	 "no teachers/no students, same starting time with the initial activity.");

	setHelpText(s);
}
