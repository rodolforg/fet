//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITYTAGSFORM_H
#define ACTIVITYTAGSFORM_H

#include "ui_activitytagsform_template.h"

class ActivityTagsForm : public QDialog, Ui::ActivityTagsForm_template
{
	Q_OBJECT
	
public:
	ActivityTagsForm(QWidget* parent);

	~ActivityTagsForm();

public slots:
	void addActivityTag();
	void removeActivityTag();
	void renameActivityTag();

	void moveActivityTagUp();
	void moveActivityTagDown();

	void sortActivityTags();
	
	void activityTagChanged(int index);
	
	void activateActivityTag();
	void deactivateActivityTag();
	
	void comments();
	
	void help();
};

#endif
