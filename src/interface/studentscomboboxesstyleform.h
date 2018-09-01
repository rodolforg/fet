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

#ifndef STUDENTSCOMBOBOXESSTYLEFORM_H
#define STUDENTSCOMBOBOXESSTYLEFORM_H

#include "ui_studentscomboboxesstyleform_template.h"

class StudentsComboBoxesStyleForm:public QDialog, Ui::StudentsComboBoxesStyleForm_template
{
	Q_OBJECT

public:
	StudentsComboBoxesStyleForm(QWidget* parent);
	~StudentsComboBoxesStyleForm();
	
public slots:
	void wasAccepted();
	void wasCanceled();
};

#endif
