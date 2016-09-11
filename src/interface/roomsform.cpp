//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
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

#include "timetable_defs.h"
#include "fet.h"
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

RoomsForm::RoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentRoomTextEdit->setReadOnly(true);
	
	modifyRoomPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addRoomPushButton, SIGNAL(clicked()), this, SLOT(addRoom()));
	connect(removeRoomPushButton, SIGNAL(clicked()), this, SLOT(removeRoom()));
	connect(roomsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(roomChanged(int)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(modifyRoomPushButton, SIGNAL(clicked()), this, SLOT(modifyRoom()));

	connect(moveRoomUpPushButton, SIGNAL(clicked()), this, SLOT(moveRoomUp()));
	connect(moveRoomDownPushButton, SIGNAL(clicked()), this, SLOT(moveRoomDown()));

	connect(sortRoomsPushButton, SIGNAL(clicked()), this, SLOT(sortRooms()));
	connect(roomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyRoom()));

	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(comments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}


RoomsForm::~RoomsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool RoomsForm::filterOk(Room* rm)
{
	Q_UNUSED(rm);

	bool ok=true;
	return ok;
}

void RoomsForm::filterChanged()
{
	QString s;
	roomsListWidget->clear();
	visibleRoomsList.clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		if(this->filterOk(rm)){
			//s=rm->getDescription();
			//roomsListWidget->addItem(s);
			roomsListWidget->addItem(rm->name);
			visibleRoomsList.append(rm);
		}
	}
	
	if(roomsListWidget->count()>0)
		roomsListWidget->setCurrentRow(0);
	else
		roomChanged(-1);
}

void RoomsForm::addRoom()
{
	AddRoomForm addRoomForm(this);
	setParentAndOtherThings(&addRoomForm, this);
	addRoomForm.exec();
	
	filterChanged();
	
	roomsListWidget->setCurrentRow(roomsListWidget->count()-1);
}

void RoomsForm::removeRoom()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ind);
	assert(rm!=NULL);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this room and all related constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeRoom(rm->name);
	assert(tmp);

	visibleRoomsList.removeAt(ind);
	roomsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=roomsListWidget->takeItem(ind);
	delete item;

	if(ind>=roomsListWidget->count())
		ind=roomsListWidget->count()-1;
	if(ind>=0)
		roomsListWidget->setCurrentRow(ind);
	else
		currentRoomTextEdit->setPlainText(QString(""));
}

void RoomsForm::roomChanged(int index)
{
	if(index<0){
		currentRoomTextEdit->setPlainText("");
		return;
	}

	QString s;
	Room* room=visibleRoomsList.at(index);

	assert(room!=NULL);
	s=room->getDetailedDescriptionWithConstraints(gt.rules);
	currentRoomTextEdit->setPlainText(s);
}

void RoomsForm::moveRoomUp()
{
	if(roomsListWidget->count()<=1)
		return;
	int i=roomsListWidget->currentRow();
	if(i<0 || i>=roomsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=roomsListWidget->item(i)->text();
	QString s2=roomsListWidget->item(i-1)->text();
	
	Room* rm1=gt.rules.roomsList.at(i);
	Room* rm2=gt.rules.roomsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i-1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i-1]=rm1;
	
	roomsListWidget->setCurrentRow(i-1);
	roomChanged(i-1);
}

void RoomsForm::moveRoomDown()
{
	if(roomsListWidget->count()<=1)
		return;
	int i=roomsListWidget->currentRow();
	if(i<0 || i>=roomsListWidget->count())
		return;
	if(i==roomsListWidget->count()-1)
		return;
		
	QString s1=roomsListWidget->item(i)->text();
	QString s2=roomsListWidget->item(i+1)->text();
	
	Room* rm1=gt.rules.roomsList.at(i);
	Room* rm2=gt.rules.roomsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i+1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i+1]=rm1;
	
	roomsListWidget->setCurrentRow(i+1);
	roomChanged(i+1);
}

void RoomsForm::sortRooms()
{
	gt.rules.sortRoomsAlphabetically();

	filterChanged();
}

void RoomsForm::modifyRoom()
{
	int valv=roomsListWidget->verticalScrollBar()->value();
	int valh=roomsListWidget->horizontalScrollBar()->value();

	int ci=roomsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	ModifyRoomForm form(this, rm->name, rm->building, rm->capacity);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	roomsListWidget->verticalScrollBar()->setValue(valv);
	roomsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=roomsListWidget->count())
		ci=roomsListWidget->count()-1;

	if(ci>=0)
		roomsListWidget->setCurrentRow(ci);
}

void RoomsForm::comments()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=gt.rules.roomsList[ind];
	assert(rm!=NULL);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Room comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(accept()));
	connect(cancelPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(reject()));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QPlainTextEdit* commentsPT=new QPlainTextEdit();
	commentsPT->setPlainText(rm->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("RoomCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		rm->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		roomChanged(ind);
	}
}
