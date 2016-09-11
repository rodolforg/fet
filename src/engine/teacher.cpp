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

#include "teacher.h"
#include "rules.h"

Teacher::Teacher()
{
	targetNumberOfHours=0;
	comments=QString("");
	qualifiedSubjectsList.clear();
	qualifiedSubjectsHash.clear();
}

Teacher::~Teacher()
{
}

QString Teacher::getXmlDescription()
{
	QString s="<Teacher>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Target_Number_of_Hours>"+CustomFETString::number(targetNumberOfHours)+"</Target_Number_of_Hours>\n";
	s+="	<Qualified_Subjects>\n";
	foreach(QString sbj, qualifiedSubjectsList)
		s+="		<Qualified_Subject>"+sbj+"</Qualified_Subject>\n";
	s+="	</Qualified_Subjects>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</Teacher>\n";

	return s;
}

QString Teacher::getDescription()
{
	QString s=tr("N:%1", "The name of the teacher").arg(name);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString Teacher::getDetailedDescription()
{
	QString s=tr("Teacher");
	s+="\n";
	s+=tr("Name=%1", "The name of the teacher").arg(this->name);
	s+="\n";
	
	s+=tr("Target number of hours=%1", "The target number of hours for the teacher").arg(targetNumberOfHours);
	s+="\n";

	s+=tr("Qualified subjects:", "The list of qualified subjects for a teacher");
	s+="\n";
	foreach(QString sbj, qualifiedSubjectsList){
		s+=sbj;
		s+="\n";
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Teacher::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this teacher:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToTeacher(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this teacher:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToTeacher(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int teachersAscending(const Teacher* t1, const Teacher* t2)
{
	return t1->name < t2->name;
}
