/***************************************************************************
                          constraintmindaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "constraintmindaysbetweenactivitiesform.h"
#include "addconstraintmindaysbetweenactivitiesform.h"
#include "modifyconstraintmindaysbetweenactivitiesform.h"

#include "changemindaysselectivelyform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintMinDaysBetweenActivitiesForm::ConstraintMinDaysBetweenActivitiesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged(int)));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(changeSelectivelyPushButton, SIGNAL(clicked()), this, SLOT(changeSelectively()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
/////////////
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

	studentsComboBox->addItem("");
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->addItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->addItem(stg->name);
			if(SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->addItem(sts->name);
			}
		}
	}
	studentsComboBox->setCurrentIndex(0);
///////////////

	this->filterChanged();
}

ConstraintMinDaysBetweenActivitiesForm::~ConstraintMinDaysBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintMinDaysBetweenActivitiesForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
		return false;
		
	ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
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

void ConstraintMinDaysBetweenActivitiesForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
	}
	
	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		currentConstraintTextEdit->setPlainText(QString(""));
}

void ConstraintMinDaysBetweenActivitiesForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
	
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintMinDaysBetweenActivitiesForm::addConstraint()
{
	AddConstraintMinDaysBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintMinDaysBetweenActivitiesForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintMinDaysBetweenActivitiesForm form(this, (ConstraintMinDaysBetweenActivities*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintMinDaysBetweenActivitiesForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	QListWidgetItem* item;

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		gt.rules.removeTimeConstraint(ctr);

		visibleConstraintsList.removeAt(i);
		constraintsListWidget->setCurrentRow(-1);
		item=constraintsListWidget->takeItem(i);
		delete item;

		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintMinDaysBetweenActivitiesForm::changeSelectively()
{
	ChangeMinDaysSelectivelyForm dialog(this);
	
	setParentAndOtherThings(&dialog, this);
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

		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
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
		setRulesModifiedAndOtherThings(&gt.rules);

		this->filterChanged();
	}
}
