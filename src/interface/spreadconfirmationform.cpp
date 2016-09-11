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

#include "spreadconfirmationform.h"

#include "timetable_defs.h"

SpreadConfirmationForm::SpreadConfirmationForm(QWidget* parent): QDialog(parent)
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
	s+=tr("This function is intended to be used after you introduced all activities and obtained a timetable, if you want "
	"now to spread the divided activities more evenly over the week. This function is useful if you have 5 days per week, "
	"maybe also if you have 6 days per week.");
	s+="\n\n";
	s+=tr("This improvement - optimization of spreading activities in a week - is a process of replacing current constraints of "
	"type min days between activities with others, which should provide you with a better timetable. Note: only min days "
	"constraints referring to components from the same split activity will be replaced. If you have constraints min days "
	"referring to activities in different components, these will be left untouched - this is a useful feature.");
	s+="\n\n";
	s+=tr("Please SAVE/BACKUP your current file and keep it safe, in case anything goes wrong, and only continue if you did that "
	"already. Current function might modify much your data");
	s+="\n\n";
	s+=tr("If you use constraints activities same starting time or same starting day then "
	"you must take care of this aspect: after applying this function and before generating a timetable, it is IMPORTANT to "
	"remove constraints min days between activities which are redundant, like described in Help/Important tips, advice 2) ."
	" If you don't do that, you might get too difficult timetables or even impossible ones.");
	s+="\n\n";
	s+=tr("It is important for you to choose good weights percentages, suitable for your institution. A good weighting might "
	"be values from 95.0% to 100.0% (ex.: 95.0%, 99.0%, 99.75%, 100.0%). You may want to select the check boxes for activities "
	"split into 2 or 3 components, so that they are not in consecutive days. If you obtain a timetable too difficult for FET, "
	"you might need to revert to your former data or lower weights of constraints. Note: you can use a progressive approach "
	"in choosing good weights");
	
	plainTextEdit->setPlainText(s);
}

SpreadConfirmationForm::~SpreadConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void SpreadConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
