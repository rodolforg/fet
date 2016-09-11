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

#ifndef SUBGROUPSFORM_H
#define SUBGROUPSFORM_H

#include "ui_subgroupsform_template.h"

class SubgroupsForm : public QDialog, Ui::SubgroupsForm_template
{
	Q_OBJECT
public:
	SubgroupsForm(QWidget* parent);
	~SubgroupsForm();

public slots:
	void addSubgroup();
	void removeSubgroup();
	void purgeSubgroup();
	void modifySubgroup();

	void moveSubgroupUp();
	void moveSubgroupDown();

	void sortSubgroups();
	void yearChanged(const QString &yearName);
	void groupChanged(const QString &groupName);
	void subgroupChanged(const QString &subgroupName);
	
	void activateStudents();
	void deactivateStudents();
	
	void comments();
};

#endif
