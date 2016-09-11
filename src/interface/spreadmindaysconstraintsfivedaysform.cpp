//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QHash>
#include <QList>

#include <QMessageBox>

#include <QPushButton>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "matrix.h"

#include "spreadmindaysconstraintsfivedaysform.h"

#include "longtextmessagebox.h"

#include "timetable.h"

#include <algorithm>
using namespace std;

extern Timetable gt;

SpreadMinDaysConstraintsFiveDaysForm::SpreadMinDaysConstraintsFiveDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(wasAccepted()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(wasCanceled()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	
	spread2CheckBox->setChecked(false);
	spread3CheckBox->setChecked(false);
}

SpreadMinDaysConstraintsFiveDaysForm::~SpreadMinDaysConstraintsFiveDaysForm()
{
	saveFETDialogGeometry(this);

}

void SpreadMinDaysConstraintsFiveDaysForm::wasAccepted()
{
	double weight4;
	QString tmp=weight4LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight4);
	if(weight4<0.0 || weight4>100.0){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for all split activities - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight2;
	tmp=weight2LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight2);
	if(spread2CheckBox->isChecked() && (weight2<0.0 || weight2>100.0)){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for activities split into 2 components - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight3;
	tmp=weight3LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight3);
	if(spread3CheckBox->isChecked() && (weight3<0.0 || weight3>100.0)){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for activities split into 3 components - must be real number >=0.0 and <=100.0"));
		return;
	}


	bool spread2=spread2CheckBox->isChecked();
	bool spread3=spread3CheckBox->isChecked();
	bool spread4OrMore=spread4OrMoreCheckBox->isChecked();
	
	if(!spread4OrMore){
		QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - min 1 day should be selected automatically for "
		 "all split activities. Please report error. FET will now abort current operation"));
		return;
	}
	assert(spread4OrMore);
	
	QHash<int, int> activitiesRepresentantIds; //first integer is the id, second is the index in the lists

	//QList<int> activitiesForRepresentant[MAX_ACTIVITIES];
	Matrix1D<QList<int> > activitiesForRepresentant;
	activitiesForRepresentant.resize(gt.rules.activitiesList.count());
	
	int nActs=0;
	
	foreach(Activity* act, gt.rules.activitiesList){
		if(act->activityGroupId==0){
			assert(!activitiesRepresentantIds.contains(act->id));
			activitiesRepresentantIds.insert(act->id, nActs);
			activitiesForRepresentant[nActs].clear();
			activitiesForRepresentant[nActs].append(act->id);
			
			nActs++;
		}
		else{
			if(activitiesRepresentantIds.contains(act->activityGroupId)){
				int k=activitiesRepresentantIds.value(act->activityGroupId);
				assert(!activitiesForRepresentant[k].contains(act->id));
				activitiesForRepresentant[k].append(act->id);
			}
			else{
				activitiesRepresentantIds.insert(act->activityGroupId, nActs);
				activitiesForRepresentant[nActs].clear();
				activitiesForRepresentant[nActs].append(act->id);
				
				nActs++;
			}
		}
	}
	
	QHash<int, int> activityGroupIdHash;
	
	foreach(Activity* act, gt.rules.activitiesList)
		activityGroupIdHash.insert(act->id, act->activityGroupId);
	
	for(int i=0; i<nActs; i++){
		//qSort(activitiesForRepresentant[i]);
		std::stable_sort(activitiesForRepresentant[i].begin(), activitiesForRepresentant[i].end());
		int fid=activitiesForRepresentant[i].at(0);
		assert(activityGroupIdHash.contains(fid));
		int gid=activityGroupIdHash.value(fid);
		if(gid>0){
			assert(activitiesRepresentantIds.contains(gid));
			assert(activitiesRepresentantIds.value(gid)==i);
		}
		else
			assert(activitiesForRepresentant[i].count()==1);
	}
	
	QList<ConstraintMinDaysBetweenActivities*> constraintsToBeRemoved;
	
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* mdc=(ConstraintMinDaysBetweenActivities*) tc;
			
			//find representant
			int reprIndex=-1;
			
			bool toBeRemoved=true;
			
			for(int i=0; i<mdc->n_activities; i++){
				if(!activityGroupIdHash.contains(mdc->activitiesId[i])){
					QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - constraint %1\ncontains invalid activity id %2\n"
					 "\nPlease report error. FET will now abort current operation").arg(mdc->getDetailedDescription(gt.rules)).arg(mdc->activitiesId[i]));
					return;
				}
				assert(activityGroupIdHash.contains(mdc->activitiesId[i]));
				if(reprIndex==-1)
					reprIndex=activityGroupIdHash.value(mdc->activitiesId[i]);
				else if(reprIndex!=activityGroupIdHash.value(mdc->activitiesId[i])){
					toBeRemoved=false;
					break;
				}
			}
			
			if(reprIndex==0)
				toBeRemoved=false;
			
			if(toBeRemoved)
				constraintsToBeRemoved.append(mdc);
		}
	}
	
	bool consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
	
	QList<ConstraintMinDaysBetweenActivities*> addedConstraints;
	
	for(int i=0; i<nActs; i++){
		ConstraintMinDaysBetweenActivities* c1;
		ConstraintMinDaysBetweenActivities* c2;
		ConstraintMinDaysBetweenActivities* c3;
		c1=NULL;
		c2=NULL;
		c3=NULL;
		
		QList<int> cl=activitiesForRepresentant[i];
		assert(cl.count()>=1);
		
		if(cl.count()>=2){
			assert(spread4OrMore);

			int n_acts;
			QList<int> acts;
			//int acts[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];
			
			n_acts=cl.count();
			acts.clear();
			for(int k=0; k<cl.count(); k++){
				//acts[k]=cl.at(k);
				acts.append(cl.at(k));
			}
			c1=new ConstraintMinDaysBetweenActivities(weight4, consecutiveIfSameDay, n_acts, acts, 1);
		}
		if(cl.count()==3 && spread3){
			int aloneComponent=-1, notAloneComp1=-1, notAloneComp2=-1;
			if(type123RadioButton->isChecked()){
				aloneComponent=1;
				notAloneComp1=2;
				notAloneComp2=3;
			}
			else if(type213RadioButton->isChecked()){
				aloneComponent=2;
				notAloneComp1=1;
				notAloneComp2=3;
			}
			else if(type312RadioButton->isChecked()){
				aloneComponent=3;
				notAloneComp1=1;
				notAloneComp2=2;
			}
			else{
				QMessageBox::information(this, tr("FET information"), tr("Please select the isolated component"));
				assert(c1!=NULL);
				delete c1;
				return;
			}
				
			aloneComponent--;
			notAloneComp1--;
			notAloneComp2--;
		
			int n_acts;
			//int acts[10];
			QList<int> acts;
			
			n_acts=2;			
			acts.clear();
			//acts[0]=cl.at(aloneComponent);
			acts.append(cl.at(aloneComponent));
			//acts[1]=cl.at(notAloneComp1);
			acts.append(cl.at(notAloneComp1));
				
			c2=new ConstraintMinDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);

			//////////

			n_acts=2;			
			acts.clear();
			//acts[0]=cl.at(aloneComponent);
			acts.append(cl.at(aloneComponent));
			//acts[1]=cl.at(notAloneComp2);
			acts.append(cl.at(notAloneComp2));
				
			c3=new ConstraintMinDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);
		}
		if(cl.count()==2 && spread2){
			int n_acts;
			
			QList<int> acts;
			//int acts[10];
			
			n_acts=2;
			acts.clear();
			//acts[0]=cl.at(0);
			acts.append(cl.at(0));
			//acts[1]=cl.at(1);
			acts.append(cl.at(1));
			
			assert(c2==NULL);
			c2=new ConstraintMinDaysBetweenActivities(weight2, consecutiveIfSameDay, n_acts, acts, 2);
		}
	
		if(c1!=NULL)
			addedConstraints.append(c1);
		if(c2!=NULL)
			addedConstraints.append(c2);
		if(c3!=NULL)
			addedConstraints.append(c3);
	}
	
	///////////
	QDialog dialog(this);
	dialog.setWindowTitle(tr("Last confirmation needed"));
	
	QVBoxLayout* top=new QVBoxLayout(&dialog);
	QLabel* topLabel=new QLabel();
	topLabel->setText(tr("Operations that will be done:"));
	top->addWidget(topLabel);
	
	QPushButton* acceptPB=new QPushButton(tr("Accept"));
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(acceptPB);
	hl->addWidget(cancelPB);
	
	QObject::connect(acceptPB, SIGNAL(clicked()), &dialog, SLOT(accept()));
	QObject::connect(cancelPB, SIGNAL(clicked()), &dialog, SLOT(reject()));
	
	QPlainTextEdit* removedText=new QPlainTextEdit();
	QPlainTextEdit* addedText=new QPlainTextEdit();
	
	QString s=tr("The following time constraints will be removed:");
	s+="\n\n";
	foreach(ConstraintMinDaysBetweenActivities* ctr, constraintsToBeRemoved){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	removedText->setPlainText(s);
	removedText->setReadOnly(true);
	
	s=tr("The following time constraints will be added:");
	s+="\n\n";
	foreach(ConstraintMinDaysBetweenActivities* ctr, addedConstraints){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	addedText->setPlainText(s);
	addedText->setReadOnly(true);
	
	top->addWidget(removedText);
	top->addWidget(addedText);
	
	top->addLayout(hl);
	
	//dialog.addLayout(top);
	
	const QString settingsName=QString("SpreadMinDaysBetweenActivitiesConstraintsLastConfirmationForm");

	dialog.resize(600, 500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	acceptPB->setFocus();
	acceptPB->setDefault(true);
	
	setParentAndOtherThings(&dialog, this);
	int res=dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
	
	if(res==QDialog::Rejected){
		constraintsToBeRemoved.clear();

		foreach(ConstraintMinDaysBetweenActivities* ctr, addedConstraints){
			delete ctr;
		}
		addedConstraints.clear();
		
		return;
	}

	assert(res==QDialog::Accepted);
	
	//better
	QList<TimeConstraint*> removedList;
	foreach(ConstraintMinDaysBetweenActivities* mdc, constraintsToBeRemoved)
		removedList.append((TimeConstraint*)mdc);
	bool t=gt.rules.removeTimeConstraints(removedList);
	assert(t);
	removedList.clear();
	constraintsToBeRemoved.clear();
	
	/*foreach(ConstraintMinDaysBetweenActivities* mdc, constraintsToBeRemoved){
		int t=gt.rules.timeConstraintsList.removeAll(mdc);
		assert(t==1);
	}
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	foreach(ConstraintMinDaysBetweenActivities* mdc, constraintsToBeRemoved)
		delete mdc;
		
	constraintsToBeRemoved.clear();*/
		
	foreach(ConstraintMinDaysBetweenActivities* tc, addedConstraints){
		bool t=gt.rules.addTimeConstraint(tc);
		if(!t){
			QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - trying to add constraint %1, "
			 "but it is already existing. Please report error. FET will now continue operation").arg(tc->getDetailedDescription(gt.rules)));
		}
	}
		
	addedConstraints.clear();
	
	QString s2=tr("Spreading of activities operation completed successfully");
	s2+="\n\n";
	s2+=tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day, it is important"
	 " (after current operation) to apply the operation of removing redundant constraints.")
	 +" "+tr("Read Help/Important tips - tip 2) for details.");
	QMessageBox::information(this, tr("FET information"), s2);
	
	this->accept();
}

void SpreadMinDaysConstraintsFiveDaysForm::wasCanceled()
{
	this->reject();
}

void SpreadMinDaysConstraintsFiveDaysForm::help()
{
	QString s;
	
	s+=tr("Help on spreading the activities over the week:");
	
	s+="\n\n";
	
	s+=tr("How to choose the weights in this dialog:");
	
	s+="\n\n";

	s+=tr("Weights (percentages) of newly added constraints min days between activities - recommended between 95.0%-100.0% "
	"(maybe lower on those split into 3). Make weights 100.0% if the constraints need to be respected all the time."
	" It is recommended to enable the check boxes for activities split into 2 or 3 components (not to be in consecutive days), "
	"if your data is still possible to solve. You may use a progressive approach. Example of weights: 90.0%, 95.0%, 99.0%, 99.75%, 100.0%.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void SpreadMinDaysConstraintsFiveDaysForm::on_spread2CheckBox_toggled()
{
	weight2LineEdit->setEnabled(spread2CheckBox->isChecked());
	weight2Label->setEnabled(spread2CheckBox->isChecked());
}

void SpreadMinDaysConstraintsFiveDaysForm::on_spread3CheckBox_toggled()
{
	weight3LineEdit->setEnabled(spread3CheckBox->isChecked());
	weight3Label->setEnabled(spread3CheckBox->isChecked());
	aloneGroupBox->setEnabled(spread3CheckBox->isChecked());
}

void SpreadMinDaysConstraintsFiveDaysForm::on_spread4OrMoreCheckBox_toggled()
{
	int k=spread4OrMoreCheckBox->isChecked();
	if(!k){
		spread4OrMoreCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that split activities"
		 " are not in the same day (with the probability you write below)"));
	}
}
