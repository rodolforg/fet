/***************************************************************************
                          constraintmindaysbetweenactivitiesform.cpp  -  description
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

#include <QMessageBox>
#include "centerwidgetonscreen.h"

#include "constraintmindaysbetweenactivitiesform.h"
#include "addconstraintmindaysbetweenactivitiesform.h"
#include "modifyconstraintmindaysbetweenactivitiesform.h"

#include "changemindaysselectivelyform.h"

#include "teacherstudentsetsubjectactivitytag_filterwidget.h"

ConstraintMinDaysBetweenActivitiesForm::ConstraintMinDaysBetweenActivitiesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintMinDaysBetweenActivitiesForm_template", "Constraints min days between activities"));

	QString label = QCoreApplication::translate("ConstraintMinDaysBetweenActivitiesForm_template", "Modify multiple constraints at once");
	QPushButton *changeSelectivelyPushButton = new QPushButton(label);
	buttons2HorizontalLayout->addWidget(changeSelectivelyPushButton);
	connect(changeSelectivelyPushButton, SIGNAL(clicked()), this, SLOT(changeSelectively()));

	TeacherStudentSetSubjectActivityTag_FilterWidget *filterWidget = new TeacherStudentSetSubjectActivityTag_FilterWidget(gt.rules);
	filterWidget->setTeachersVisible(true);
	filterWidget->setStudentSetsVisible(true);
	filterWidget->setSubjectsVisible(true);
	filterWidget->setActivityTagsVisible(true);
	setFilterWidget(filterWidget);
	connect(filterWidget, &TeacherStudentSetSubjectActivityTag_FilterWidget::FilterChanged, this, &ConstraintMinDaysBetweenActivitiesForm::filterChanged);

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintMinDaysBetweenActivitiesForm::~ConstraintMinDaysBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintMinDaysBetweenActivitiesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type!=CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
		return false;
		
	const ConstraintMinDaysBetweenActivities* c=(const ConstraintMinDaysBetweenActivities*) ctr;

	QSet<const Activity *> activities;
	for(int id : qAsConst(c->activitiesId)){
		for(const Activity* a : qAsConst(gt.rules.activitiesList)) {
			if(a->id==id) {
				activities << a;
				break;
			}
		}
	}

	TeacherStudentSetSubjectActivityTag_FilterWidget *filter_widget = static_cast<TeacherStudentSetSubjectActivityTag_FilterWidget*>(getFilterWidget());
	return filter_widget->filterActivitySet(activities);
}

QDialog * ConstraintMinDaysBetweenActivitiesForm::createAddDialog()
{
	return new AddConstraintMinDaysBetweenActivitiesForm(this);
}

QDialog * ConstraintMinDaysBetweenActivitiesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintMinDaysBetweenActivitiesForm(this, (ConstraintMinDaysBetweenActivities*)ctr);
}

void ConstraintMinDaysBetweenActivitiesForm::changeSelectively()
{
	ChangeMinDaysSelectivelyForm dialog(this);
	
	bool result=dialog.exec();

	if(result==QDialog::Accepted){
		double oldWeight=dialog.oldWeight;
		double newWeight=dialog.newWeight;
		int oldConsecutive=dialog.oldConsecutive;
		int newConsecutive=dialog.newConsecutive;
		int oldDays=dialog.oldDays;
		int newDays=dialog.newDays;
		int oldNActs=dialog.oldNActs;
		if(oldWeight==-1){
		}
		else if(oldWeight>=0 && oldWeight<=100.0){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has meet a critical error - aborting current operation, please report bug (old weight is not -1 and not (>=0.0 and <=100.0))"));
			return;
		}

		if(newWeight==-1){
		}
		else if(newWeight>=0 && newWeight<=100.0){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new weight is not -1 and not (>=0.0 and <=100.0))"));
			return;
		}
		
		enum {ANY=0, YES=1, NO=2};
		enum {NOCHANGE=0};
		
		if(oldConsecutive<0 || oldConsecutive>2){
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old consecutive is not any, yes or no)"));
			return;
		}
		
		if(newConsecutive<0 || newConsecutive>2){
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new consecutive is not no_change, yes or no)"));
			return;
		}
		
		if(oldDays==-1){
		}
		else if(oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old min days is not -1 or 1..ndaysperweek)"));
			return;
		}
		
		if(newDays==-1){
		}
		else if(newDays>=1 && newDays<=gt.rules.nDaysPerWeek){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new min days is not -1 or 1..ndaysperweek)"));
			return;
		}
		
		if(oldNActs==-1){
		}
		else if(oldNActs>=1){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old nActivities is not -1 or >=1)"));
			return;
		}
		
		int count=0;

		for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList))
			if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* mc=(ConstraintMinDaysBetweenActivities*)tc;
				bool okw, okd, okc, okn;
				if(oldWeight==-1)
					okw=true;
				else if(oldWeight==mc->weightPercentage)
					okw=true;
				else
					okw=false;
					
				if(oldConsecutive==ANY)
					okc=true;
				else if(oldConsecutive==YES && mc->consecutiveIfSameDay==true)
					okc=true;
				else if(oldConsecutive==NO && mc->consecutiveIfSameDay==false)
					okc=true;
				else
					okc=false;
					
				if(oldDays==-1)
					okd=true;
				else if(oldDays==mc->minDays)
					okd=true;
				else
					okd=false;
					
				if(oldNActs==-1)
					okn=true;
				else if(mc->n_activities==oldNActs)
					okn=true;
				else
					okn=false;
					
				if(okw && okc && okd && okn){
					if(newWeight>=0)
						mc->weightPercentage=newWeight;
						
					if(newConsecutive==YES)
						mc->consecutiveIfSameDay=true;
					else if(newConsecutive==NO)
						mc->consecutiveIfSameDay=false;
						
					if(newDays>=0)
						mc->minDays=newDays;
					
					count++;
				}
			}

		QMessageBox::information(this, tr("FET information"), tr("There were inspected (and possibly modified) %1 constraints min days between activities matching your criteria")
		 .arg(count)+"\n\n"+
		 	tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day, it is important"
		 	 " (after current operation) to apply the operation of removing redundant constraints.")
		 	+" "+tr("Read Help/Important tips - tip 2) for details.")
		 );

		gt.rules.internalStructureComputed=false;
		gt.rules.setModified(true);

		this->filterChanged();
	}
}
