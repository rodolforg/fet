/***************************************************************************
                          helpaboutform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include "helpaboutform.h"

#include "timetable_defs.h"

HelpAboutForm::HelpAboutForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	aboutTextBrowser->setReadOnly(true);
	authorsTextBrowser->setReadOnly(true);
	translatorsTextBrowser->setReadOnly(true);
	referencesTextBrowser->setReadOnly(true);
	thanksToTextBrowser->setReadOnly(true);
	
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	tabWidget->setCurrentIndex(0);
	
	aboutTextBrowser->setOpenExternalLinks(true);
	referencesTextBrowser->setOpenExternalLinks(true);
}

HelpAboutForm::~HelpAboutForm()
{
	saveFETDialogGeometry(this);
}
