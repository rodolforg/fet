/***************************************************************************
                          constrainttwoactivitiesorderedform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constrainttwoactivitiesorderedform.h"
#include "addconstrainttwoactivitiesorderedform.h"
#include "modifyconstrainttwoactivitiesorderedform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintTwoActivitiesOrderedForm::ConstraintTwoActivitiesOrderedForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintTwoActivitiesOrderedForm_template", "Constraints two activies ordered"));

	QString instruction = QCoreApplication::translate("ConstraintTwoActivitiesOrderedForm_template", "This constraint forces two activities A1 and A2: A2 to be after A1, separated by any time interval in the week");
	setInstructionText(instruction);

	restoreFETDialogGeometry(this);

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintTwoActivitiesOrderedForm::filterChanged);

	this->filterChanged();
}

ConstraintTwoActivitiesOrderedForm::~ConstraintTwoActivitiesOrderedForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTwoActivitiesOrderedForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_TWO_ACTIVITIES_ORDERED)
		return false;

	ConstraintTwoActivitiesOrdered* c=(ConstraintTwoActivitiesOrdered*) ctr;
	const TeacherStudentSetSubjectActivityTag_FilterWidget * filterWidget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	QString tn=filterWidget->teacher();
	QString sbn=filterWidget->subject();
	QString sbtn=filterWidget->activityTag();
	QString stn=filterWidget->studentsSet();

	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<2; i++){
		//bool found=true;
	
		int id=-1;
		
		if(i==0)
			id=c->firstActivityId;
		else if(i==1)
			id=c->secondActivityId;
			
		assert(id>=0);

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

QDialog * ConstraintTwoActivitiesOrderedForm::createAddDialog()
{
	return new AddConstraintTwoActivitiesOrderedForm(this);
}

QDialog * ConstraintTwoActivitiesOrderedForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintTwoActivitiesOrderedForm(this, (ConstraintTwoActivitiesOrdered*)ctr);
}
