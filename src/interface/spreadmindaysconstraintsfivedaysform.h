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

#ifndef SPREADMINDAYSCONSTRAINTSFIVEDAYSFORM_H
#define SPREADMINDAYSCONSTRAINTSFIVEDAYSFORM_H

#include "ui_spreadmindaysconstraintsfivedaysform_template.h"

class SpreadMinDaysConstraintsFiveDaysForm:public QDialog, Ui::SpreadMinDaysConstraintsFiveDaysForm_template
{
	Q_OBJECT

public:
	SpreadMinDaysConstraintsFiveDaysForm(QWidget* parent);
	~SpreadMinDaysConstraintsFiveDaysForm();
	
public slots:
	void wasAccepted();
	void wasCanceled();
	void help();
	
	void on_spread2CheckBox_toggled();
	void on_spread3CheckBox_toggled();
	void on_spread4OrMoreCheckBox_toggled();
};

#endif
