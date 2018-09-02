/***************************************************************************
                          addconstraintsubactivitiespreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "longtextmessagebox.h"
#include "centerwidgetonscreen.h"

#include "addconstraintsubactivitiespreferredtimeslotsform.h"
#include "timeconstraint.h"

#include "fetguisettings.h"
#include "studentscomboboxhelper.h"

AddConstraintSubactivitiesPreferredTimeSlotsForm::AddConstraintSubactivitiesPreferredTimeSlotsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
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

	updateTeachersComboBox();
	updateStudentsComboBox();
	updateSubjectsComboBox();
	updateActivityTagsComboBox();

	preferredTimesTable->setHeaders(gt.rules);
			
	componentNumberSpinBox->setMinimum(1);
	componentNumberSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
	componentNumberSpinBox->setValue(1);
}

AddConstraintSubactivitiesPreferredTimeSlotsForm::~AddConstraintSubactivitiesPreferredTimeSlotsForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsAllowed()
{
	preferredTimesTable->setAllUnmarked();
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsNotAllowed()
{
	preferredTimesTable->setAllMarked();
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->addItem(t->name);
	}
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::updateStudentsComboBox(){
	StudentsComboBoxHelper::populateStudentsComboBox(studentsComboBox, QString(""), true);
	/*studentsComboBox->clear();
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
	}*/
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(s->name);
	}
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::updateActivityTagsComboBox(){
	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* s=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(s->name);
	}
}

void AddConstraintSubactivitiesPreferredTimeSlotsForm::addConstraint()
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

	ctr=new ConstraintSubactivitiesPreferredTimeSlots(weight, componentNumberSpinBox->value(),/*compulsory,*/ teacher, students, subject, activityTag, n, days_L, hours_L);

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
