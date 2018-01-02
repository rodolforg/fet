/***************************************************************************
                          modifyconstraintteachernotavailabletimesform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "modifyconstraintteachernotavailabletimesform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherNotAvailableTimesForm::ModifyConstraintTeacherNotAvailableTimesForm(QWidget* parent, ConstraintTeacherNotAvailableTimes* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(setAllAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllAvailable()));
	connect(setAllNotAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllNotAvailable()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateTeachersComboBox();

	notAllowedTimesTable->setHeaders(gt.rules);

	assert(ctr->days.count()==ctr->hours.count());	
	for(int k=0; k<ctr->days.count(); k++){
		if(ctr->hours.at(k)==-1 || ctr->days.at(k)==-1)
			assert(0);
		int i=ctr->hours.at(k);
		int j=ctr->days.at(k);
		if(i>=0 && i<gt.rules.nHoursPerDay && j>=0 && j<gt.rules.nDaysPerWeek)
			notAllowedTimesTable->setMarked(i,j, true);
	}
}

ModifyConstraintTeacherNotAvailableTimesForm::~ModifyConstraintTeacherNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherNotAvailableTimesForm::setAllAvailable()
{
	notAllowedTimesTable->setAllUnmarked();
}

void ModifyConstraintTeacherNotAvailableTimesForm::setAllNotAvailable()
{
	notAllowedTimesTable->setAllMarked();
}

void ModifyConstraintTeacherNotAvailableTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->addItem(t->name);
	}
	teachersComboBox->setCurrentText(this->_ctr->teacher);
}

void ModifyConstraintTeacherNotAvailableTimesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	foreach(TimeConstraint* c, gt.rules.timeConstraintsList)
		if(c!=this->_ctr && c->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* cc=(ConstraintTeacherNotAvailableTimes*)c;
			if(cc->teacher==teacher_name){
				QMessageBox::warning(this, tr("FET information"),
					tr("A constraint of this type exists for the same teacher - cannot proceed"));
				return;
			}
		}
			
	this->_ctr->weightPercentage=weight;

	if(_ctr->teacher!=teacher_name){
		QString oldName=_ctr->teacher;
		QString newName=teacher_name;

		QSet<ConstraintTeacherNotAvailableTimes*> cs=gt.rules.tnatHash.value(oldName, QSet<ConstraintTeacherNotAvailableTimes*>());
		assert(cs.contains(_ctr));
		cs.remove(_ctr);
		gt.rules.tnatHash.insert(oldName, cs);

		cs=gt.rules.tnatHash.value(newName, QSet<ConstraintTeacherNotAvailableTimes*>());
		assert(!cs.contains(_ctr));
		cs.insert(_ctr);
		gt.rules.tnatHash.insert(newName, cs);
		
		this->_ctr->teacher=teacher_name;
	}

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->isMarked(i, j)){
				days.append(j);
				hours.append(i);
			}

	this->_ctr->days=days;
	this->_ctr->hours=hours;

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
