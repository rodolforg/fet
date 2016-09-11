//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <http://lalescu.ro/liviu/>
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
#include "timetable.h"
#include "fet.h"

#include "institutionnameform.h"

#include <QMessageBox>

extern Timetable gt;

extern bool simulation_running;

InstitutionNameForm::InstitutionNameForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	institutionNameLineEdit->setText(gt.rules.institutionName);
	institutionNameLineEdit->selectAll();
	institutionNameLineEdit->setFocus();
}

InstitutionNameForm::~InstitutionNameForm()
{
	saveFETDialogGeometry(this);
}

void InstitutionNameForm::ok()
{
	if(!simulation_running)
		gt.rules.setInstitutionName(institutionNameLineEdit->text());
	else{
		QMessageBox::information(this, tr("FET information"),
			tr("Cannot update institution name during simulation\n"
			"Please stop simulation before this"));
		return;
	}

	this->close();
}

void InstitutionNameForm::cancel()
{
	this->close();
}
