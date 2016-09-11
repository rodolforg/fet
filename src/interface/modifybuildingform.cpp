/***************************************************************************
                          modifybuildingform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2008
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

#include "modifybuildingform.h"

#include <QMessageBox>

ModifyBuildingForm::ModifyBuildingForm(QWidget* parent, const QString& initialBuildingName): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_initialBuildingName=initialBuildingName;

	nameLineEdit->setText(initialBuildingName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyBuildingForm::~ModifyBuildingForm()
{
	saveFETDialogGeometry(this);
}

void ModifyBuildingForm::cancel()
{
	this->close();
}

void ModifyBuildingForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(this->_initialBuildingName!=nameLineEdit->text() && gt.rules.searchBuilding(nameLineEdit->text())>=0){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyBuilding(this->_initialBuildingName, nameLineEdit->text());
	assert(t);
	
	this->close();
}
