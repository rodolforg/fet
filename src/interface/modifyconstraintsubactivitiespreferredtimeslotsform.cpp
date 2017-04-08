/***************************************************************************
                          modifyconstraintsubactivitiespreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : 2008
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

#include <QMessageBox>

#include "modifyconstraintsubactivitiespreferredtimeslotsform.h"
#include "timeconstraint.h"

ModifyConstraintSubactivitiesPreferredTimeSlotsForm::ModifyConstraintSubactivitiesPreferredTimeSlotsForm(QWidget* parent, ConstraintSubactivitiesPreferredTimeSlots* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
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
	
	this->_ctr=ctr;

	updateTeachersComboBox();
	updateStudentsComboBox(parent);
	updateSubjectsComboBox();
	updateActivityTagsComboBox();
	
	componentNumberSpinBox->setMinimum(1);
	componentNumberSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
	componentNumberSpinBox->setValue(this->_ctr->componentNumber);
	
	preferredTimesTable->setHeaders(gt.rules);

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setMarked(i, j, true);
	for(int k=0; k<ctr->p_nPreferredTimeSlots_L; k++){
		if(ctr->p_hours_L[k]==-1 || ctr->p_days_L[k]==-1)
			assert(0);
		int i=ctr->p_hours_L[k];
		int j=ctr->p_days_L[k];
		if(i>=0 && i<gt.rules.nHoursPerDay && j>=0 && j<gt.rules.nDaysPerWeek)
			preferredTimesTable->setMarked(i, j, false);
	}

	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
}

ModifyConstraintSubactivitiesPreferredTimeSlotsForm::~ModifyConstraintSubactivitiesPreferredTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsAllowed()
{
	preferredTimesTable->setAllUnmarked();
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsNotAllowed()
{
	preferredTimesTable->setAllMarked();
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->addItem(t->name);
	}
	teachersComboBox->setCurrentText(this->_ctr->p_teacherName);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateStudentsComboBox(QWidget* parent){
	studentsComboBox->clear();
	studentsComboBox->addItem("");
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->addItem(sty->name);
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->addItem(stg->name);
			if(SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->addItem(sts->name);
			}
		}
	}
	if (studentsComboBox->findText(this->_ctr->p_studentsName) < 0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->p_studentsName);
	studentsComboBox->setCurrentText(this->_ctr->p_studentsName);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(s->name);
	}
	subjectsComboBox->setCurrentText(this->_ctr->p_subjectName);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateActivityTagsComboBox(){
	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* s=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(s->name);
	}
	activityTagsComboBox->setCurrentText(this->_ctr->p_activityTagName);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->p_studentsName);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	QString teacher=teachersComboBox->currentText();
	if(teacher!="")
		assert(gt.rules.searchTeacher(teacher)>=0);

	QString students=studentsComboBox->currentText();
	if(students!="")
		assert(gt.rules.searchStudentsSet(students)!=NULL);

	QString subject=subjectsComboBox->currentText();
	if(subject!="")
		assert(gt.rules.searchSubject(subject)>=0);
		
	QString activityTag=activityTagsComboBox->currentText();
	if(activityTag!="")
		assert(gt.rules.searchActivityTag(activityTag)>=0);
		
	QList<int> days_L;
	QList<int> hours_L;
	//int days[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS];
	//int hours[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(!preferredTimesTable->isMarked(i, j)){
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

	this->_ctr->weightPercentage=weight;

	this->_ctr->componentNumber=componentNumberSpinBox->value();
	
	this->_ctr->p_teacherName=teacher;
	this->_ctr->p_studentsName=students;
	this->_ctr->p_subjectName=subject;
	this->_ctr->p_activityTagName=activityTag;
	this->_ctr->p_nPreferredTimeSlots_L=n;
	this->_ctr->p_days_L=days_L;
	this->_ctr->p_hours_L=hours_L;

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
