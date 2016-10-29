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

#include "activitytag.h"
#include "rules.h"

ActivityTag::ActivityTag()
{
}

ActivityTag::~ActivityTag()
{
}

QString ActivityTag::getXmlDescription() const
{
	QString s="<Activity_Tag>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</Activity_Tag>\n";

	return s;
}

QString ActivityTag::getDescription() const
{
	QString s=tr("N:%1", "The name of the activity tag").arg(name);
	
	if(!comments.isEmpty())
		s+=", "+tr("C: %1", "Comments").arg(comments);
	
	return s;
}

QString ActivityTag::getDetailedDescription() const
{
	QString s=tr("Activity tag");
	s+="\n";
	s+=tr("Name=%1", "The name of the activity tag").arg(this->name);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString ActivityTag::getDetailedDescriptionWithConstraints(const Rules &r) const
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this activity tag:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this activity tag:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

bool activityTagsAscending(const ActivityTag* st1, const ActivityTag* st2)
{
	return st1->name < st2->name;
}
