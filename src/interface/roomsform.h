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

#ifndef ROOMSFORM_H
#define ROOMSFORM_H

#include "room.h"

#include "ui_roomsform_template.h"

class RoomsForm : public QDialog, Ui::RoomsForm_template
{
	Q_OBJECT
public:
	RoomsList visibleRoomsList;

	RoomsForm(QWidget* parent);
	~RoomsForm();

	bool filterOk(Room* rm);

public slots:
	void addRoom();
	void removeRoom();
	void modifyRoom();

	void moveRoomUp();
	void moveRoomDown();

	void sortRooms();
	void roomChanged(int index);
	void filterChanged();
	
	void comments();
};

#endif
