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

#include "studentsset.h"
#include "rules.h"

StudentsSet::StudentsSet() :
	numberOfStudents(0),
	type(STUDENTS_SET)
{
}

StudentsSet::~StudentsSet()
{
}

StudentsYear::StudentsYear()
	: indexInAugmentedYearsList(-1)
{
	this->type = STUDENTS_YEAR;
}

StudentsYear::~StudentsYear()
{
	//it is possible that the removed group to be in another year

	/*while(!groupsList.isEmpty()){
		StudentsGroup* g=groupsList[0];
		
		foreach(StudentsYear* year, gt.rules.yearsList)
			if(year!=this)
				for(int i=0; i<year->groupsList.size(); i++)
					if(year->groupsList[i]==g)
						year->groupsList[i]=NULL;
	
		if(g!=NULL){
			delete groupsList.takeFirst();
		}
		else
			groupsList.removeFirst();
	}*/
}

QString StudentsYear::getXmlDescription() const
{
	QString s="";
	s+="<Year>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	for(int i=0; i<this->groupsList.size(); i++){
		StudentsGroup* stg=this->groupsList[i];
		s+=stg->getXmlDescription();
	}
	s+="</Year>\n";

	return s;
}

QString StudentsYear::getDescription() const
{
	QString s;
	s+=tr("YN:%1", "Year name").arg(this->name);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsYear::getDetailedDescription() const
{
	QString s="";
	s+=tr("Students set - year");
	s+="\n";
	s+=tr("Year name=%1").arg(this->name);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsYear::getDetailedDescriptionWithConstraints(const Rules &r) const
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students year:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students year:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}


StudentsGroup::StudentsGroup()
	: indexInInternalGroupsList(-1)
{
	this->type = STUDENTS_GROUP;
}

StudentsGroup::~StudentsGroup()
{
	/*while(!subgroupsList.isEmpty()){
		StudentsSubgroup* s=subgroupsList[0];
		
		foreach(StudentsYear* year, gt.rules.yearsList)
			foreach(StudentsGroup* group, year->groupsList)
				if(group!=this)
					for(int i=0; i<group->subgroupsList.size(); i++)
						if(group->subgroupsList[i]==s){
							cout<<"making NULL group->subgroupsList[i]->name=="<<qPrintable(group->subgroupsList[i]->name)<<endl;
							group->subgroupsList[i]=NULL;
						}
						else
							cout<<"ignoring group->subgroupsList[i]->name=="<<qPrintable(group->subgroupsList[i]->name)<<endl;
	
		if(s!=NULL){
			assert(subgroupsList[0]!=NULL);
			delete subgroupsList.takeFirst();
		}
		else
			subgroupsList.removeFirst();
	}*/
}

QString StudentsGroup::getXmlDescription() const
{
	QString s="";
	s+="	<Group>\n";
	s+="		<Name>"+protect(this->name)+"</Name>\n";
	s+="		<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="		<Comments>"+protect(comments)+"</Comments>\n";
	for(int i=0; i<this->subgroupsList.size(); i++){
		StudentsSubgroup* sts=this->subgroupsList[i];
		s+=sts->getXmlDescription();
	}
	s+="	</Group>\n";

	return s;
}

QString StudentsGroup::getDescription() const
{
	QString s="";
	s+=tr("GN:%1", "Group name").arg(this->name);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);

	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsGroup::getDetailedDescription() const
{
	QString s="";
	s+=tr("Students set - group");
	s+="\n";
	s+=tr("Group name=%1").arg(this->name);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsGroup::getDetailedDescriptionWithConstraints(Rules& r) const
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students group:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students group:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}


StudentsSubgroup::StudentsSubgroup()
	: indexInInternalSubgroupsList(-1)
{
	this->type = STUDENTS_SUBGROUP;
}

StudentsSubgroup::~StudentsSubgroup()
{
}

QString StudentsSubgroup::getXmlDescription() const
{
	QString s="";
	s+="		<Subgroup>\n";
	s+="			<Name>"+protect(this->name)+"</Name>\n";
	s+="			<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="			<Comments>"+protect(comments)+"</Comments>\n";
	s+="		</Subgroup>\n";

	return s;
}

QString StudentsSubgroup::getDescription() const
{
	QString s="";
	s+=tr("SgN:%1", "Subgroup name").arg(this->name);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);

	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsSubgroup::getDetailedDescription() const
{
	QString s="";
	s+=tr("Students set - subgroup");
	s+="\n";
	s+=tr("Subgroup name=%1").arg(this->name);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsSubgroup::getDetailedDescriptionWithConstraints(const Rules &r) const
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students subgroup:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students subgroup:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

bool yearsAscending(const StudentsYear* y1, const StudentsYear* y2)
{
	return y1->name < y2->name;
}

bool groupsAscending(const StudentsGroup* g1, const StudentsGroup* g2)
{
	return g1->name < g2->name;
}

bool subgroupsAscending(const StudentsSubgroup* s1, const StudentsSubgroup* s2)
{
	return s1->name < s2->name;
}
