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

#include "building.h"
#include "timetable_defs.h" // for protect*()

Building::Building()
{
}

Building::~Building()
{
}

void Building::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
}

QString Building::getDescription() const
{
	QString s=tr("N:%1", "The name of the building").arg(name);
	
	if(!comments.isEmpty())
		s+=", "+tr("C: %1", "Comments").arg(comments);
	
	return s;
}

QString Building::getDetailedDescription() const
{
	QString s=tr("Building");
	s+="\n";
	s+=tr("Name=%1", "The name of the building").arg(name);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Building::getXmlDescription() const
{
	QString s="<Building>\n";
	s+="	<Name>"+protect(name)+"</Name>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</Building>\n";

	return s;
}

QString Building::getDetailedDescriptionWithConstraints(const Rules &r) const
{
	Q_UNUSED(r);

	QString s=this->getDetailedDescription();

	/*s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this building:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToBuilding(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";*/

	return s;
}

bool buildingsAscending(const Building* b1, const Building* b2)
{
	return b1->name.localeAwareCompare(b2->name);
}
