/***************************************************************************
                          addconstraintroomnotavailabletimesform.cpp  -  description
                             -------------------
    begin                : Thu Jan 8 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addconstraintroomnotavailabletimesform.h"
#include "spaceconstraint.h"

AddConstraintRoomNotAvailableTimesForm::AddConstraintRoomNotAvailableTimesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(setAllAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllAvailable()));
	connect(setAllNotAvailablePushButton, SIGNAL(clicked()), this, SLOT(setAllNotAvailable()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	updateRoomsComboBox();

	notAllowedTimesTable->setHeaders(gt.rules);
}

AddConstraintRoomNotAvailableTimesForm::~AddConstraintRoomNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintRoomNotAvailableTimesForm::setAllAvailable()
{
	notAllowedTimesTable->setAllUnmarked();
}

void AddConstraintRoomNotAvailableTimesForm::setAllNotAvailable()
{
	notAllowedTimesTable->setAllMarked();
}

void AddConstraintRoomNotAvailableTimesForm::updateRoomsComboBox()
{
	roomsComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* room=gt.rules.roomsList[i];
		//roomsComboBox->addItem(room->getDescription());
		roomsComboBox->addItem(room->name);
	}
}

void AddConstraintRoomNotAvailableTimesForm::addConstraint()
{
	SpaceConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
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

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->isMarked(i, j)){
				days.append(j);
				hours.append(i);
			}

	ctr=new ConstraintRoomNotAvailableTimes(weight, room->name, days, hours);

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		/*QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - there must be another constraint of this "
			  "type referring to the same room. Please edit that one"));*/
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
