/***************************************************************************
                          constraintmingapsbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : July 10, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "constraintmingapsbetweenactivitiesform.h"
#include "addconstraintmingapsbetweenactivitiesform.h"
#include "modifyconstraintmingapsbetweenactivitiesform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintMinGapsBetweenActivitiesForm::ConstraintMinGapsBetweenActivitiesForm(QWidget* parent): ConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintMinGapsBetweenActivitiesForm_template", "Constraints min gaps between activities"));

	QString instruction = QCoreApplication::translate("ConstraintMinGapsBetweenActivitiesForm_template", "This constraint ensures, for a set of activities: if any pair of activities is in the same day, they must be separated by at least min gaps (hours).");
	instruction += "\n\n" + QCoreApplication::translate("ConstraintMinGapsBetweenActivitiesForm_template", "Note: please click Help for important advice");
	setInstructionText(instruction);

	setHelp();

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintMinGapsBetweenActivitiesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintMinGapsBetweenActivitiesForm::~ConstraintMinGapsBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintMinGapsBetweenActivitiesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES)
		return false;
		
	ConstraintMinGapsBetweenActivities* c=(ConstraintMinGapsBetweenActivities*) ctr;
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

QDialog * ConstraintMinGapsBetweenActivitiesForm::createAddDialog()
{
	return new AddConstraintMinGapsBetweenActivitiesForm(this);
}

QDialog * ConstraintMinGapsBetweenActivitiesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintMinGapsBetweenActivitiesForm(this, (ConstraintMinGapsBetweenActivities*)ctr);
}

void ConstraintMinGapsBetweenActivitiesForm::setHelp()
{
	QString s=tr("Please make sure that the selected activities are not forced to be"
		" consecutive by some other constraint 'min days between activities' (with"
		" 'consecutive if same day' true), by a constraint 'two activities consecutive',"
		" or by a constraint 'two activities grouped' (also, if you have a constraint"
		" 'three activities grouped' related to the selected activities, make sure that the"
		" constraints do not contradict).");
		
	setHelpText(s);
}
