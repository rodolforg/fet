//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
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
	printable=true;
}

ActivityTag::~ActivityTag()
{
}

QString ActivityTag::getXmlDescription() const
{
	QString s="<Activity_Tag>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	
	s+="	<Printable>";
	if(this->printable)
		s+="true";
	else
		s+="false";
	s+="</Printable>\n";
	
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</Activity_Tag>\n";

	return s;
}

QString ActivityTag::getDescription() const
{
	QString s=tr("N:%1", "The name of the activity tag").arg(name);
	
	s+=", ";
	QString printableYesNo;
	if(this->printable)
		printableYesNo=tr("yes");
	else
		printableYesNo=tr("no");
	s+=tr("P:%1", "Whether the activity tag is Printable - can be true or false").arg(printableYesNo);
	
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

	QString printableYesNo;
	if(this->printable)
		printableYesNo=tr("yes");
	else
		printableYesNo=tr("no");
	s+=tr("Printable=%1", "Whether the activity tag is Printable - can be true or false").arg(printableYesNo);
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
	for(TimeConstraintsList::const_iterator it = r.timeConstraintsList.constBegin(); it != r.timeConstraintsList.constEnd(); ++it){
		const TimeConstraint *c = *it;
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this activity tag:");
	s+="\n";
	for(SpaceConstraintsList::const_iterator it = r.spaceConstraintsList.constBegin(); it != r.spaceConstraintsList.constEnd(); ++it){
		const SpaceConstraint *c = *it;
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
	return st1->name.localeAwareCompare(st2->name) < 0;
}
