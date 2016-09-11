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

#include "timetableshowconflictsform.h"

#include "timetable.h"

#include <QString>

extern Timetable gt;
extern QString conflictsStringTitle;
extern QString conflictsString;

TimetableShowConflictsForm::TimetableShowConflictsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);

	conflictsTextEdit->setReadOnly(true);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setWindowTitle(conflictsStringTitle);
	conflictsTextEdit->setPlainText(conflictsString);
}

TimetableShowConflictsForm::~TimetableShowConflictsForm()
{
	saveFETDialogGeometry(this);
}
