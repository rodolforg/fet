//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
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

#ifndef BUILDINGSFORM_H
#define BUILDINGSFORM_H

#include "building.h"

#include "ui_buildingsform_template.h"

class BuildingsForm : public QDialog, Ui::BuildingsForm_template
{
	Q_OBJECT
	
public:
	BuildingsList visibleBuildingsList;

	BuildingsForm(QWidget* parent);
	~BuildingsForm();

	bool filterOk(Building* bu);

public slots:
	void addBuilding();
	void removeBuilding();
	void buildingChanged(int index);

	void moveBuildingUp();
	void moveBuildingDown();

	void sortBuildings();
	void modifyBuilding();
	void filterChanged();
	
	void comments();
};

#endif
