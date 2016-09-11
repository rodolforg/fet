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

#include "savetimetableconfirmationform.h"

#include "timetable_defs.h"

SaveTimetableConfirmationForm::SaveTimetableConfirmationForm(QWidget* parent): QDialog(parent)
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

	s+=tr("This option is only useful if you need to lock current timetable into a file."
	 " Locking means that there will be added constraints activity preferred starting time and"
	 " activity preferred room with 100% importance for each activity to fix it at current place in current timetable."
	 " You can save this timetable as an ordinary .fet file; when you'll open it, you'll see all old inputted data (activities, teachers, etc.)" 
	 " and the locking constraints as the last time/space constraints."
	 " You can unlock some of these activities (by removing constraints) if small changes appear in the configuration, and generate again"
	 " and the remaining locking constraints will be respected.");
	s+="\n\n";
	s+=tr("The added constraints will have the 'permanently locked' tag set to false, so you can also unlock the activities from the "
	 "'Timetable' menu, without interfering with the initial constraints which are made by you 'permanently locked'");
	s+="\n\n";
	s+=tr("This option is useful for institutions where you obtain a timetable, then some small changes appear,"
	 " and you need to regenerate timetable, but respecting in a large proportion the old timetable");
	s+="\n\n";
	s+=tr("Current data file will not be affected by anything, locking constraints will only be added to the file you select to save"
	 " (you can save current datafile and open saved timetable file after that to check it)");
	
	plainTextEdit->setPlainText(s);
}

SaveTimetableConfirmationForm::~SaveTimetableConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void SaveTimetableConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
