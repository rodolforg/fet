//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2018 Liviu Lalescu <https://lalescu.ro/liviu/>
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

#include "studentscomboboxesstyleform.h"

StudentsComboBoxesStyleForm::StudentsComboBoxesStyleForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(wasAccepted()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(wasCanceled()));
	
	simpleRadioButton->setChecked(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_SIMPLE);
	iconsRadioButton->setChecked(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_ICONS);
	categorizedRadioButton->setChecked(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED);
}

StudentsComboBoxesStyleForm::~StudentsComboBoxesStyleForm()
{
	saveFETDialogGeometry(this);
}

void StudentsComboBoxesStyleForm::wasAccepted()
{
	if(simpleRadioButton->isChecked())
		STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_SIMPLE;
	else if(iconsRadioButton->isChecked())
		STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_ICONS;
	else if(categorizedRadioButton->isChecked())
		STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED;

	this->accept();
}

void StudentsComboBoxesStyleForm::wasCanceled()
{
	this->reject();
}
