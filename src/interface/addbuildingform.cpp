/***************************************************************************
                          addbuildingform.cpp  -  description
                             -------------------
    begin                : Feb 11 2008
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

#include "addbuildingform.h"

#include <QMessageBox>

AddBuildingForm::AddBuildingForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	addBuildingPushButton->setDefault(true);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(addBuildingPushButton, SIGNAL(clicked()), this, SLOT(addBuilding()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

AddBuildingForm::~AddBuildingForm()
{
	saveFETDialogGeometry(this);
}

void AddBuildingForm::addBuilding()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	Building* bu=new Building();
	bu->name=nameLineEdit->text();
	if(!gt.rules.addBuilding(bu)){
		QMessageBox::information(this, tr("Room insertion dialog"),
			tr("Could not insert item. Must be a duplicate"));
		delete bu;
	}
	else{
		QMessageBox::information(this, tr("Building insertion dialog"),
			tr("Building added"));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
