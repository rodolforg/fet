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

#include "timetable_defs.h"

#include "settingstimetablehtmllevelform.h"

extern QApplication* pqapplication;

#include <QMessageBox>

SettingsTimetableHtmlLevelForm::SettingsTimetableHtmlLevelForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
		
	if(TIMETABLE_HTML_LEVEL==0)
		level0RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==1)
		level1RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==2)
		level2RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==3)
		level3RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==4)
		level4RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==5)
		level5RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==6)
		level6RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==7)
		level7RadioButton->setChecked(true);
}

SettingsTimetableHtmlLevelForm::~SettingsTimetableHtmlLevelForm()
{
	saveFETDialogGeometry(this);
}

void SettingsTimetableHtmlLevelForm::ok()
{
	int level=-1;
	if(level0RadioButton->isChecked())
		level=0;
	else if(level1RadioButton->isChecked())
		level=1;
	else if(level2RadioButton->isChecked())
		level=2;
	else if(level3RadioButton->isChecked())
		level=3;
	else if(level4RadioButton->isChecked())
		level=4;
	else if(level5RadioButton->isChecked())
		level=5;
	else if(level6RadioButton->isChecked())
		level=6;
	else if(level7RadioButton->isChecked())
		level=7;
	else{
		QMessageBox::warning(this, tr("FET warning"), tr("No level selected"));
		return;
	}
	
	assert(level>=0);

	if(level>=3){
		int t=QMessageBox::information(this, tr("FET information"), tr("This level might generate very large timetables, maybe 1 MB per file"
		 " and 20 MB for all files of a timetable or even more."
		 " Are you sure you have enough disk space?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
		
		if(t==QMessageBox::Cancel)
			return;
	}
	
	TIMETABLE_HTML_LEVEL=level;
	
	this->close();
}

void SettingsTimetableHtmlLevelForm::cancel()
{
	this->close();
}
