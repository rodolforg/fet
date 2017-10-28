/***************************************************************************
                          modifyconstraintroomnotavailabletimesform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "modifyconstraintroomnotavailabletimesform.h"
#include "spaceconstraint.h"

ModifyConstraintRoomNotAvailableTimesForm::ModifyConstraintRoomNotAvailableTimesForm(QWidget* parent, ConstraintRoomNotAvailableTimes* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(setAllAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllAvailable()));
	connect(setAllNotAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllNotAvailable()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	updateRoomsComboBox();

	notAllowedTimesTable->setHeaders(gt.rules);
	assert(ctr->days.count()==ctr->hours.count());	
	for(int k=0; k<ctr->days.count(); k++){
		if(ctr->hours.at(k)==-1 || ctr->days.at(k)==-1)
			assert(0);
		int i=ctr->hours.at(k);
		int j=ctr->days.at(k);
		if(i>=0 && i<gt.rules.nHoursPerDay && j>=0 && j<gt.rules.nDaysPerWeek)
			notAllowedTimesTable->setMarked(i, j, true);
	}
}

ModifyConstraintRoomNotAvailableTimesForm::~ModifyConstraintRoomNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintRoomNotAvailableTimesForm::setAllAvailable()
{
	notAllowedTimesTable->setAllUnmarked();
}

void ModifyConstraintRoomNotAvailableTimesForm::setAllNotAvailable()
{
	notAllowedTimesTable->setAllMarked();
}

void ModifyConstraintRoomNotAvailableTimesForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* room=gt.rules.roomsList[k];
		//roomsComboBox->addItem(room->getDescription());
		roomsComboBox->addItem(room->name);
		if(room->name==this->_ctr->room)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentIndex(j);
}

void ModifyConstraintRoomNotAvailableTimesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<=0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	
	Room* room=gt.rules.roomsList.at(i);

	this->_ctr->weightPercentage=weight;
	this->_ctr->room=room->name;

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
