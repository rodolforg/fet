//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
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

#include "removeredundantconfirmationform.h"

#include "timetable_defs.h"

RemoveRedundantConfirmationForm::RemoveRedundantConfirmationForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	continuePushButton->setDefault(true);
	
	connect(continuePushButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(dontShowAgainCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dontShowAgainCheckBoxToggled()));

	dontShowAgain=dontShowAgainCheckBox->isChecked();
	
	plainTextEdit->setReadOnly(true);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";
	s+=tr("This function is intended to be used after you inputted all data or after you used the advanced function "
	"of spreading the activities over the week. This function will automatically remove the redundant constraints of "
	"type min days between activities, so that your data is more correct and the timetable easier for FET to find");
	s+="\n\n";
	s+=tr("Removing means making their weight percentage 0.0%. This is done so you can still activate them again, if "
	"needed, and also for you to be able to see when they are broken, in the soft conflicts list");
	s+="\n\n";
	s+=tr("This function might be usable if you have constraints of type activities same starting time and/or "
	"constraints activities same starting day");
	s+="\n\n";
	s+=tr("Please SAVE/BACKUP your current file and keep it safe, in case anything goes wrong, and only continue if "
	"you did that already. Current function might modify much your data");
	
	plainTextEdit->setPlainText(s);
}

RemoveRedundantConfirmationForm::~RemoveRedundantConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void RemoveRedundantConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
