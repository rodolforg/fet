/***************************************************************************
                          modifyconstraintactivitypreferredroomform.cpp  -  description
                             -------------------
    begin                : 13 Feb 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "modifyconstraintactivitypreferredroomform.h"
#include "spaceconstraint.h"

#include "lockunlock.h"

ModifyConstraintActivityPreferredRoomForm::ModifyConstraintActivityPreferredRoomForm(QWidget* parent, ConstraintActivityPreferredRoom* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	permLockedCheckBox->setChecked(this->_ctr->permanentlyLocked);

	updateActivitiesComboBox();
	updateRoomsComboBox();
}

ModifyConstraintActivityPreferredRoomForm::~ModifyConstraintActivityPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivityPreferredRoomForm::updateActivitiesComboBox()
{
	int i=0, j=-1;
	activitiesComboBox->clear();
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		activitiesComboBox->addItem(act->getDescription(gt.rules));
		if(act->id==this->_ctr->activityId)
			j=i;
		i++;
	}
	assert(j>=0);
	activitiesComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivityPreferredRoomForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* rm=gt.rules.roomsList[k];
		roomsComboBox->addItem(rm->name);
		if(rm->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivityPreferredRoomForm::cancel()
{
	this->close();
}

void ModifyConstraintActivityPreferredRoomForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int i=activitiesComboBox->currentIndex();
	if(i<0 || activitiesComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	Activity* act=gt.rules.activitiesList.at(i);

	i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	this->_ctr->weightPercentage=weight;
	this->_ctr->roomName=room;
	
	if(_ctr->activityId!=act->id){
		int oldId=_ctr->activityId;
		int newId=act->id;
		
		QSet<ConstraintActivityPreferredRoom*> cs=gt.rules.aprHash.value(oldId, QSet<ConstraintActivityPreferredRoom*>());
		assert(cs.contains(_ctr));
		cs.remove(_ctr);
		gt.rules.aprHash.insert(oldId, cs);
		
		cs=gt.rules.aprHash.value(newId, QSet<ConstraintActivityPreferredRoom*>());
		assert(!cs.contains(_ctr));
		cs.insert(_ctr);
		gt.rules.aprHash.insert(newId, cs);
	
		this->_ctr->activityId=act->id;
	}
	
	this->_ctr->permanentlyLocked=permLockedCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	this->close();
}
