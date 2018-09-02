/***************************************************************************
                          addconstraintactivitypreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "addconstraintactivitypreferredtimeslotsform.h"
#include "timeconstraint.h"

#include "centerwidgetonscreen.h"
#include <QMessageBox>

#include "fetguisettings.h"
#include "studentscomboboxhelper.h"

AddConstraintActivityPreferredTimeSlotsForm::AddConstraintActivityPreferredTimeSlotsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(setAllAllowedPushButton, SIGNAL(clicked()), this, SLOT(setAllSlotsAllowed()));
	connect(setAllNotAllowedPushButton, SIGNAL(clicked()), this, SLOT(setAllSlotsNotAllowed()));

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
	
	QSize tmp5=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
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

	StudentsComboBoxHelper::populateStudentsComboBox(studentsComboBox, QString(""), true);
	/*studentsComboBox->addItem("");
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
	}*/
	studentsComboBox->setCurrentIndex(0);
	
	updateActivitiesComboBox();

	preferredTimesTable->setHeaders(gt.rules);
}

AddConstraintActivityPreferredTimeSlotsForm::~AddConstraintActivityPreferredTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintActivityPreferredTimeSlotsForm::setAllSlotsAllowed()
{
	preferredTimesTable->setAllUnmarked();
}

void AddConstraintActivityPreferredTimeSlotsForm::setAllSlotsNotAllowed()
{
	preferredTimesTable->setAllMarked();
}

bool AddConstraintActivityPreferredTimeSlotsForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void AddConstraintActivityPreferredTimeSlotsForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void AddConstraintActivityPreferredTimeSlotsForm::updateActivitiesComboBox(){
	activitiesComboBox->clear();
	activitiesList.clear();
	
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		
		if(filterOk(act)){
			activitiesComboBox->addItem(act->getDescription());
			this->activitiesList.append(act->id);
		}
	}
}

void AddConstraintActivityPreferredTimeSlotsForm::addConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int i=activitiesComboBox->currentIndex();
	assert(i<activitiesList.size());
	if(i<0 || activitiesComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	int id=activitiesList.at(i);
	//Activity* act=gt.rules.activitiesList.at(id);
	
	QList<int> days_L;
	QList<int> hours_L;
	//int days[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS];
	//int hours[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(i=0; i<gt.rules.nHoursPerDay; i++)
			if(!preferredTimesTable->isMarked(i, j)){
				/*if(n>=MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
					QString s=tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=tr("Please increase the variable MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS");
					s+="\n";
					s+=tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS);
					QMessageBox::warning(this, tr("FET information"), s);
					
					return;
				}*/
				
				days_L.append(j);
				hours_L.append(i);
				n++;
			}

	if(n<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Warning: 0 slots selected. Are you sure?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	/*foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_TIMES){
			ConstraintActivityPreferredTimes* c=(ConstraintActivityPreferredTimes*) tc;
			if(c->activityId==id){
				QMessageBox::warning(this, tr("FET information"),
				 tr("This activity id has other constraint of this type attached\n"
				 "Please remove the other constraints of type activity preferred times\n"
				 "referring to this activity before proceeding"));
				return;
			}
		}
	}*/

	ctr=new ConstraintActivityPreferredTimeSlots(weight, /*compulsory,*/ /*act->*/id, n, days_L, hours_L);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - duplicate?", "'Duplicate' means that the constraint is identical to an existing constraint and cannot be added."));
		delete ctr;
	}
}
