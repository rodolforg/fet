//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef SPREADCONFIRMATIONFORM_H
#define SPREADCONFIRMATIONFORM_H

#include "ui_spreadconfirmationform_template.h"

class SpreadConfirmationForm: public QDialog, Ui::SpreadConfirmationForm_template
{
	Q_OBJECT

public:
	bool dontShowAgain;
	
	SpreadConfirmationForm(QWidget* parent);
	~SpreadConfirmationForm();

public slots:
	void dontShowAgainCheckBoxToggled();
};

#endif
