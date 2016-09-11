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

#ifndef GROUPSFORM_H
#define GROUPSFORM_H

#include "ui_groupsform_template.h"

class GroupsForm : public QDialog, Ui::GroupsForm_template
{
	Q_OBJECT
public:
	GroupsForm(QWidget* parent);
	~GroupsForm();

public slots:
	void addGroup();
	void removeGroup();
	void purgeGroup();
	void modifyGroup();

	void moveGroupUp();
	void moveGroupDown();
	
	void sortGroups();
	
	void yearChanged(const QString &yearName);
	void groupChanged(const QString &groupName);
	
	void activateStudents();
	void deactivateStudents();
	
	void comments();
};

#endif
