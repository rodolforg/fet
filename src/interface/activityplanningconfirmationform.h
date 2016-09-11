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

#ifndef ACTIVITY_PLANNING_CONFIRMATION_FORM_H
#define ACTIVITY_PLANNING_CONFIRMATION_FORM_H

#include "ui_activityplanningconfirmationform_template.h"

class ActivityPlanningConfirmationForm: public QDialog, Ui::ActivityPlanningConfirmationForm_template
{
	Q_OBJECT

public:
	bool dontShowAgain;

	ActivityPlanningConfirmationForm(QWidget* parent);
	~ActivityPlanningConfirmationForm();

public slots:
	void dontShowAgainCheckBoxToggled();
};

#endif
