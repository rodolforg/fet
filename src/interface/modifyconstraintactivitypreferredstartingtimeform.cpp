/***************************************************************************
                          modifyconstraintactivitypreferredstartingtimeform.cpp  -  description
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

#include "modifyconstraintactivitypreferredstartingtimeform.h"
#include "timeconstraint.h"

#include "lockunlock.h"

ModifyConstraintActivityPreferredStartingTimeForm::ModifyConstraintActivityPreferredStartingTimeForm(QWidget* parent, ConstraintActivityPreferredStartingTime* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));

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
	QSize tmp6=dayComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	QSize tmp7=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp7);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

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
	
	updatePeriodGroupBox();
	updateActivitiesComboBox();

	if(ctr->day>=0 && ctr->day<gt.rules.nDaysPerWeek)
		dayComboBox->setCurrentIndex(ctr->day);
	else
		dayComboBox->setCurrentIndex(-1);
		
	if(ctr->hour>=0 && ctr->hour<gt.rules.nHoursPerDay)
		startHourComboBox->setCurrentIndex(ctr->hour);
	else
		startHourComboBox->setCurrentIndex(-1);
	
	permLockedCheckBox->setChecked(this->_ctr->permanentlyLocked);
	
	constraintChanged();
}

ModifyConstraintActivityPreferredStartingTimeForm::~ModifyConstraintActivityPreferredStartingTimeForm()
{
	saveFETDialogGeometry(this);
}

bool ModifyConstraintActivityPreferredStartingTimeForm::filterOk(Activity* act)
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

void ModifyConstraintActivityPreferredStartingTimeForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraintActivityPreferredStartingTimeForm::updateActivitiesComboBox(){
	activitiesComboBox->clear();
	activitiesList.clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			activitiesComboBox->addItem(act->getDescription(gt.rules));
			this->activitiesList.append(act->id);
			if(act->id==this->_ctr->activityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	activitiesComboBox->setCurrentIndex(j);

	constraintChanged();
}

void ModifyConstraintActivityPreferredStartingTimeForm::updatePeriodGroupBox(){
	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);

	dayComboBox->clear();
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		dayComboBox->addItem(gt.rules.daysOfTheWeek[i]);
}

void ModifyConstraintActivityPreferredStartingTimeForm::constraintChanged()
{
}

void ModifyConstraintActivityPreferredStartingTimeForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int day=dayComboBox->currentIndex();
	if(day<0 || day>=gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid day"));
		return;
	}
	int startHour=startHourComboBox->currentIndex();
	if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid start hour"));
		return;
	}

	int tmp2=activitiesComboBox->currentIndex();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<activitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	int id=activitiesList.at(tmp2);
	
	bool permanentlyLocked=permLockedCheckBox->isChecked();
	
	if(1){
		ConstraintActivityPreferredStartingTime apst;

		apst.activityId=id;
		apst.weightPercentage=weight;
		apst.day=day;
		apst.hour=startHour;
		apst.permanentlyLocked=permanentlyLocked;
		
		bool duplicate=false;
		
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
			if(tc!=this->_ctr && tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
				if( ( *((ConstraintActivityPreferredStartingTime*)tc) ) == apst){
					duplicate=true;
					break;
				}
				
		if(duplicate){
			QMessageBox::warning(this, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
			return;
		}
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->day=day;
	this->_ctr->hour=startHour;

	if(_ctr->activityId!=id){
		int oldId=_ctr->activityId;
		int newId=id;

		QSet<ConstraintActivityPreferredStartingTime*> cs=gt.rules.apstHash.value(oldId, QSet<ConstraintActivityPreferredStartingTime*>());
		assert(cs.contains(_ctr));
		cs.remove(_ctr);
		gt.rules.apstHash.insert(oldId, cs);

		cs=gt.rules.apstHash.value(newId, QSet<ConstraintActivityPreferredStartingTime*>());
		assert(!cs.contains(_ctr));
		cs.insert(_ctr);
		gt.rules.apstHash.insert(newId, cs);

		this->_ctr->activityId=id;
	}
	
	this->_ctr->permanentlyLocked=permanentlyLocked;
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
	LockUnlock::increaseCommunicationSpinBox();

	this->close();
}

void ModifyConstraintActivityPreferredStartingTimeForm::cancel()
{
	this->close();
}
