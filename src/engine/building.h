//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
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

#ifndef BUILDING_H
#define BUILDING_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Building;
class Rules;

typedef QList<Building*> BuildingsList;

/**
This class represents a building

@author Liviu Lalescu
*/
class Building{
	Q_DECLARE_TR_FUNCTIONS(Building)

public:
	QString name;
	
	QString comments;

	Building();
	~Building();
	
	void computeInternalStructure(Rules& r);
	
	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int buildingsAscending(const Building* b1, const Building* b2);

#endif
