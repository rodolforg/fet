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

#include "room.h"
#include "rules.h"

Room::Room()
{
	this->capacity=MAX_ROOM_CAPACITY;
	this->building=QString("");
	comments=QString("");
}

Room::~Room()
{
}

void Room::computeInternalStructure(Rules& r)
{
	if(building=="")
		buildingIndex=-1;
	else{
		buildingIndex=r.searchBuilding(building);
		assert(buildingIndex>=0 && buildingIndex<r.nInternalBuildings);
	}
}

QString Room::getDescription()
{
	QString s=tr("N:%1", "Name of room").arg(this->name);
	s+=", ";
	
	if(this->building!=""){
		s+=tr("B:%1", "Building").arg(this->building);
		s+=", ";
	}
	/*s+=tr("T:");
	s+=this->type;
	s+=",";*/
	s+=tr("C:%1", "Capacity").arg(CustomFETString::number(this->capacity));
	//s+=",";

	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="E:"+(*it)+", ";*/
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString Room::getDetailedDescription()
{
	QString s=tr("Room");
	s+="\n";
	s+=tr("Name=%1", "The name of the room").arg(this->name);
	s+="\n";

	if(this->building!=""){
		s+=tr("Building=%1").arg(this->building);
		s+="\n";
	}
	s+=tr("Capacity=%1").arg(CustomFETString::number(this->capacity));
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Room::getXmlDescription()
{
	QString s="<Room>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Building>"+protect(this->building)+"</Building>\n";
	//s+="	<Type>"+protect(this->type)+"</Type>\n";
	s+="	<Capacity>"+CustomFETString::number(this->capacity)+"</Capacity>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	
	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="	<Equipment>"+protect(*it)+"</Equipment>\n";*/
	
	s+="</Room>\n";

	return s;
}

QString Room::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this room:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToRoom(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int roomsAscending(const Room* r1, const Room* r2)
{
	return r1->name < r2->name;
}
