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

#ifndef ROOM_H
#define ROOM_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Room;
class Rules;

typedef QList<Room*> RoomsList;

/**
This class represents a room

@author Liviu Lalescu
*/
class Room{ /*classroom :-)*/
	Q_DECLARE_TR_FUNCTIONS(Room)

public:
	QString name;
	int capacity;
	
	/**
	If empty string, it is ignored
	*/
	QString building;
	
	QString comments;
	
	/**
	Internal. If -1, it is ignored
	*/
	int buildingIndex;

	Room();
	~Room();
	
	void computeInternalStructure(Rules& r);

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int roomsAscending(const Room* r1, const Room* r2);

#endif
