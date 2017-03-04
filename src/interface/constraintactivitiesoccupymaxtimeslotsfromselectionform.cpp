/***************************************************************************
                          constraintactivitiesoccupymaxtimeslotsfromselectionform.cpp  -  description
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

#include "constraintactivitiesoccupymaxtimeslotsfromselectionform.h"
#include "addconstraintactivitiesoccupymaxtimeslotsfromselectionform.h"
#include "modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm_template", "Constraints activities occupy max time slots from selection"));

	setInstructionText(QCoreApplication::translate("ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm_template", "IMPORTANT: Please do not overuse this constraint, because the generation may be slowed down very much. Please read Help for more details."));
	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::~ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm()
{
	saveFETDialogGeometry(this);
}


bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION)
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

QDialog * ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::createAddDialog()
{
	return new AddConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(this);
}

QDialog * ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(this, (ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)ctr);
}

void ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm::setHelp()
{
	QString s;
	
	s+=tr("IMPORTANT NOTE: Please use this constraint ONLY when strictly necessary, when it is really useful, when it has effect, "
	 "when it is not implied by the other constraints, and in a non-redundant way. Otherwise it may slow down very much the generation.");
	s+=QString("\n\n");
	s+=tr("This constraint type was added on 25 September 2011.");
	s+=QString("\n\n");
	s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
	 "of time slots from those selected which can be occupied by the specified activities.");
	s+=QString("\n\n");
	s+=tr("Uses of this constraint are probably very wide. The main reason it was added was to enable users to specify that a teacher "
	 "should have activities in certain time slots. Example: we have a week with 30 hours. Teacher T has 20 hours of activities, and "
	 "we would like him to have activities Monday and Tuesday on the first two hours. Add a constraint of this type, with the set of activities "
	 "being all the activities of teacher T, selected time slots being the remaining 30-2*2=26 slots of the week, and the maximum number of "
	 "selected time slots which can be occupied by these activities being 20-2*2=16.");
	
	setHelpText(s);
}
