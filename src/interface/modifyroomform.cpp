/***************************************************************************
                          modifyroomform.cpp  -  description
                             -------------------
    begin                : Feb 12, 2005
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

#include "modifyroomform.h"

#include <QMessageBox>

ModifyRoomForm::ModifyRoomForm(QWidget* parent, const QString& initialRoomName, const QString& initialRoomBuilding, int initialRoomCapacity): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=buildingsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	capacitySpinBox->setMinimum(1);
	capacitySpinBox->setMaximum(MAX_ROOM_CAPACITY);
	capacitySpinBox->setValue(MAX_ROOM_CAPACITY);

	this->_initialRoomName=initialRoomName;
	this->_initialRoomBuilding=initialRoomBuilding;
	this->_initialRoomCapacity=initialRoomCapacity;
	capacitySpinBox->setValue(initialRoomCapacity);

	nameLineEdit->setText(initialRoomName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	int i=0;
	int j=-1;
	buildingsComboBox->clear();
	buildingsComboBox->addItem("");
	if(initialRoomBuilding=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.buildingsList.size(); k++, i++){
		buildingsComboBox->addItem(gt.rules.buildingsList.at(k)->name);
		if(gt.rules.buildingsList.at(k)->name==initialRoomBuilding)
			j=i;
	}
	assert(j>=0);
	buildingsComboBox->setCurrentIndex(j);
}

ModifyRoomForm::~ModifyRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyRoomForm::cancel()
{
	this->close();
}

void ModifyRoomForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(buildingsComboBox->currentIndex()<0){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect building"));
		return;
	}
	if(this->_initialRoomName!=nameLineEdit->text() && gt.rules.searchRoom(nameLineEdit->text())>=0){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyRoom(this->_initialRoomName, nameLineEdit->text()/*, typesComboBox->currentText()*/, buildingsComboBox->currentText(), capacitySpinBox->value());
	assert(t);
	
	this->close();
}
