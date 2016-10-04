/*
File spaceconstraint.cpp
*/

/***************************************************************************
                          spaceconstraint.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "spaceconstraint.h"
#include "rules.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "studentsset.h"
#include "room.h"
#include "solution.h"

#include "matrix.h"

#include "generate_pre.h"

#include <QString>

#include "messageboxes.h"

static QString trueFalse(bool x)
{
	if(!x)
		return QString("false");
	else
		return QString("true");
}

static QString yesNoTranslated(bool x)
{
	if(!x)
		return QCoreApplication::translate("SpaceConstraint", "no", "no - meaning negation");
	else
		return QCoreApplication::translate("SpaceConstraint", "yes", "yes - meaning affirmative");
}

//static qint8 roomsMatrix[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static Matrix3D<int> roomsMatrix;

static int rooms_conflicts=-1;

//extern QList<int> activitiesPreferredRoomsPreferredRooms[MAX_ACTIVITIES];

//static qint8 subgroupsBuildingsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
//static qint8 teachersBuildingsTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QString getActivityDetailedDescription(Rules& r, int id);

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

SpaceConstraint::SpaceConstraint()
{
	type=CONSTRAINT_GENERIC_SPACE;
	
	active=true;
	comments=QString("");
}

SpaceConstraint::~SpaceConstraint()
{
}

SpaceConstraint::SpaceConstraint(double wp)
{
	type=CONSTRAINT_GENERIC_SPACE;

	weightPercentage=wp;
	assert(wp<=100 && wp>=0);

	active=true;
	comments=QString("");
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
	this->weightPercentage=100;
}

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace(double wp)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
}

bool ConstraintBasicCompulsorySpace::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintBasicCompulsorySpace::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintBasicCompulsorySpace::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s = "<ConstraintBasicCompulsorySpace>\n";
	assert(this->weightPercentage==100.0);
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintBasicCompulsorySpace>\n";
	return s;
}

QString ConstraintBasicCompulsorySpace::getDescription(Rules& r)
{
	Q_UNUSED(r);
	
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s = tr("Basic compulsory constraints (space)");
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));
	
	return begin+s+end;
}

QString ConstraintBasicCompulsorySpace::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("These are the basic compulsory constraints (referring to rooms allocation) for any timetable");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("The basic space constraints try to avoid:");s+="\n";
	s+=QString("- "); s+=tr("rooms assigned to more than one activity simultaneously"); s+="\n";
	s+=QString("- "); s+=tr("activities with more students than the capacity of the room"); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintBasicCompulsorySpace::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int roomsConflicts;

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the rooms conflicts faster this way.
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = roomsConflicts = c.getRoomsMatrix(r, roomsMatrix);
		
		c.changedForMatrixCalculation=false;
	}
	else{
		assert(rooms_conflicts>=0);
		roomsConflicts=rooms_conflicts;
	}

	int i;

	qint64 unallocated; //unallocated activities
	int nre; //number of room exhaustions
	int nor; //number of overwhelmed rooms

	//part without logging....................................................................
	if(conflictsString==NULL){
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		/*nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							s+=" ";
							s+=tr("This increases the conflicts total by %1").arg(tmp*weightPercentage/100);
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString += s+"\n";
						}
						nre+=tmp;
					}
				}
		*/
		nre=roomsConflicts;
	}
	//part with logging....................................................................
	else{
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
				if(conflictsString!=NULL){
					QString s=tr("Space constraint basic compulsory broken: unallocated activity with id=%1 (%2)",
						"%2 is the detailed description of the activity").arg(r.internalActivitiesList[i].id).arg(getActivityDetailedDescription(r, r.internalActivitiesList[i].id));
					s+=QString(" - ");
					s+=tr("this increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100*10000));
					
					dl.append(s);
					cl.append(weightPercentage/100 * 10000);
					
					*conflictsString+=s+"\n";

					/*(*conflictsString) += tr("Space constraint basic compulsory: unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					(*conflictsString) += tr(" - this increases the conflicts total by %1")
						.arg(weight*10000);
					(*conflictsString) += "\n";*/
				}
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;

					if(conflictsString!=NULL){
						QString s;
						s=tr("Space constraint basic compulsory: room %1 has allocated activity with id %2 (%3) and the capacity of the room is overloaded",
							"%2 is act id, %3 is detailed description of activity")
						.arg(r.internalRoomsList[rm]->name)
						.arg(r.internalActivitiesList[i].id)
						.arg(getActivityDetailedDescription(r, r.internalActivitiesList[i].id));
						s+=". ";
						s+=tr("This increases conflicts total by %1").arg(CustomFETString::number(weightPercentage/100));
						
						dl.append(s);
						cl.append(weightPercentage/100);
						
						*conflictsString += s+"\n";
					}
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							s+=" ";
							s+=tr("This increases the conflicts total by %1").arg(CustomFETString::number(tmp*weightPercentage/100));
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString += s+"\n";
							/*(*conflictsString)+=tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							(*conflictsString)+=" ";
							(*conflictsString)+=tr("This increases the conflicts total by %1").arg(tmp*weight);
							(*conflictsString)+="\n";*/
						}
						nre+=tmp;
					}
				}
	}
	/*if(roomsConflicts!=-1)
		assert(nre==roomsConflicts);*/ //just a check, works only on logged fitness calculation
		
	if(this->weightPercentage==100){
		//assert(unallocated==0);
		assert(nre==0);
		assert(nor==0);
	}

	return weightPercentage/100 * (unallocated + qint64(nre) + qint64(nor)); //fitness factor
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintBasicCompulsorySpace::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintBasicCompulsorySpace::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintBasicCompulsorySpace::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes(double wp, const QString& rn, QList<int> d, QList<int> h)
	: SpaceConstraint(wp)
{
	this->room=rn;
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

bool ConstraintRoomNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintRoomNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintRoomNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Room>"+protect(this->room)+"</Room>\n";

	s+="	<Number_of_Not_Available_Times>"+CustomFETString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Not_Available_Time>\n";
	}

	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintRoomNotAvailableTimes>\n";
	return s;
}

QString ConstraintRoomNotAvailableTimes::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Room not available");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("R:%1", "Room").arg(this->room);s+=", ";

	s+=tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}

	return begin+s+end;
}

QString ConstraintRoomNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=tr("Space constraint");s+="\n";
	s+=tr("Room not available");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Not available at:", "It refers to a room");
	s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintRoomNotAvailableTimes::computeInternalStructure(QWidget* parent, Rules& r){
	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint room not available times is wrong because it refers to inexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint room not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint room not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	
	assert(this->room_ID>=0);
	
	return true;
}

double ConstraintRoomNotAvailableTimes::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrices roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the roomr is supposed to be occupied,
	//but it is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory constraint - we only
	//are allowed 1 weekly activity for a certain room at a certain hour) we calculate
	//5 broken constraints for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int rm=this->room_ID;

	int nbroken;

	nbroken=0;

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);
		
		if(roomsMatrix[rm][d][h]>0){
			nbroken+=roomsMatrix[rm][d][h];
	
			if(conflictsString!=NULL){
				QString s= tr("Space constraint room not available times broken for room: %1, on day %2, hour %3")
				 .arg(r.internalRoomsList[rm]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]);
				s += ". ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number(roomsMatrix[rm][d][h]*weightPercentage/100));
				 
				dl.append(s);
				cl.append(roomsMatrix[rm][d][h]*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintRoomNotAvailableTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintRoomNotAvailableTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintRoomNotAvailableTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
}

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom(double wp, int aid, const QString& room, bool perm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
	this->activityId=aid;
	this->roomName=room;
	this->permanentlyLocked=perm;
}

bool ConstraintActivityPreferredRoom::operator==(ConstraintActivityPreferredRoom& c){
	if(this->roomName!=c.roomName)
		return false;
	if(this->activityId!=c.activityId)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	if(this->active!=c.active)
		return false;
	//no need to care about permanently locked
	return true;
}

bool ConstraintActivityPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->_activity=-1;
	int ac=r.activitiesHash.value(activityId, -1);
	assert(ac>=0);
	_activity=ac;
	/*for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}
	if(ac==r.nInternalActivities){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}*/
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	//assert(_room>=0);

	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	assert(this->_room>=0);
	
	return true;
}

bool ConstraintActivityPreferredRoom::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;

	return false;
}

QString ConstraintActivityPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivityPreferredRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+="	<Permanently_Locked>";s+=trueFalse(this->permanentlyLocked);s+="</Permanently_Locked>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityPreferredRoom>\n";

	return s;
}

QString ConstraintActivityPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity preferred room"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("Id:%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+=", ";

	s+=tr("R:%1", "Room").arg(this->roomName);
	
	s+=", ";
	s+=tr("PL:%1", "Abbreviation for permanently locked").arg(yesNoTranslated(this->permanentlyLocked));

	return begin+s+end;
}

QString ConstraintActivityPreferredRoom::getDetailedDescription(Rules& r){
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	
	s+=tr("Activity id=%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+="\n";
	
	s+=tr("Room=%1").arg(this->roomName);s+="\n";
	
	if(this->permanentlyLocked){
		s+=tr("This activity is permanently locked, which means you cannot unlock it from the 'Timetable' menu"
		" (you can unlock this activity by removing the constraint from the constraints dialog or by setting the 'permanently"
		" locked' attribute false when editing this constraint)");
	}
	else{
		s+=tr("This activity is not permanently locked, which means you can unlock it from the 'Timetable' menu");
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(/*rm!=UNALLOCATED_SPACE &&*/ rm!=this->_room){
		if(rm!=UNALLOCATED_SPACE){
			ok=false;

			if(conflictsString!=NULL){
				QString s=tr("Space constraint activity preferred room broken for activity with id=%1 (%2), room=%3",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(this->activityId)
					.arg(getActivityDetailedDescription(r, this->activityId))
					.arg(this->roomName);
					s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
		
				dl.append(s);
				cl.append(1*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintActivityPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
}

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
	this->activityId=aid;
	this->roomsNames=roomsList;
}

bool ConstraintActivityPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	_activity=r.activitiesHash.value(activityId, r.nInternalActivities);
	int ac=_activity;

	/*this->_activity=-1;
	int ac;
	for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}*/
		
	if(ac==r.nInternalActivities){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	
	this->_rooms.clear();
	foreach(QString rm, this->roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);

		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}

		assert(t>=0);
		this->_rooms.append(t);
	}
		
	return true;
}

bool ConstraintActivityPreferredRooms::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;

	return false;
}

QString ConstraintActivityPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivityPreferredRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityPreferredRooms>\n";

	return s;
}

QString ConstraintActivityPreferredRooms::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity preferred rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("Id:%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintActivityPreferredRooms::getDetailedDescription(Rules& r){
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	
	s+=tr("Activity id=%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+="\n";
	
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(1 || rm!=UNALLOCATED_SPACE){
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm!=UNALLOCATED_SPACE){
				ok=false;
		
				if(conflictsString!=NULL){
					QString s=tr("Space constraint activity preferred rooms broken for activity with id=%1 (%2)"
						, "%1 is activity id, %2 is detailed description of activity")
						.arg(this->activityId)
						.arg(getActivityDetailedDescription(r, this->activityId));
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100 * 1));
				
					dl.append(s);
					cl.append(weightPercentage/100 * 1);
				
					*conflictsString += s+"\n";
				}

				nbroken++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintActivityPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
}

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom(double wp, QString st, QString rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
	this->studentsName=st;
	this->roomName=rm;
}

bool ConstraintStudentsSetHomeRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	//QStringList::iterator it;
	Activity* act;

	this->_activities.clear();
	
	/*QSet<int> set=r.activitiesForStudentsSetHash.value(studentsName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		if(act.studentsNames.count()==1){
			assert(act.studentsNames.at(0)==studentsName);
			_activities.append(i);
		}
	}
	qSort(_activities);*/
	
	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool commonStudents=false;
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				commonStudents=true;
	
		if(!commonStudents)
			continue;
		
		this->_activities.append(ac);
	}

	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintStudentsSetHomeRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetHomeRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetHomeRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetHomeRoom>\n";

	return s;
}

QString ConstraintStudentsSetHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set home room"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=tr("R:%1", "R means Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintStudentsSetHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Students set home room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetHomeRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		//if(rm!=this->_room)
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint students set home room broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintStudentsSetHomeRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintStudentsSetHomeRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintStudentsSetHomeRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;
}

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms(double wp, QString st, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;

	this->studentsName=st;

	this->roomsNames=rms;
}

bool ConstraintStudentsSetHomeRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForStudentsSetHash.value(studentsName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		if(act.studentsNames.count()==1){
			assert(act.studentsNames.at(0)==studentsName);
			_activities.append(i);
		}
	}
	qSort(_activities);*/

	//QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool commonStudents=false;
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				commonStudents=true;
	
		if(!commonStudents)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	foreach(QString rm, this->roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	return true;
}

bool ConstraintStudentsSetHomeRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetHomeRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetHomeRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetHomeRooms>\n";

	return s;
}

QString ConstraintStudentsSetHomeRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set home rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";

	s+=tr("St:%1", "St means students").arg(this->studentsName);

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "R means Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintStudentsSetHomeRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Students set home rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetHomeRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=tr("Space constraint students set home rooms broken for activity with id %1 (%2)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintStudentsSetHomeRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintStudentsSetHomeRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintStudentsSetHomeRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
}

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom(double wp, QString tc, QString rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
	this->teacherName=tc;
	this->roomName=rm;
}

bool ConstraintTeacherHomeRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	//QStringList::iterator it;
	Activity* act;

	this->_activities.clear();

	/*QSet<int> set=r.activitiesForTeacherHash.value(teacherName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		if(act.teachersNames.count()==1){
			assert(act.teachersNames.at(0)==teacherName);
			_activities.append(i);
		}
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintTeacherHomeRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherHomeRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherHomeRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherHomeRoom>\n";

	return s;
}

QString ConstraintTeacherHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher home room"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=tr("R:%1", "R means Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintTeacherHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Teacher home room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherHomeRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint teacher home room broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherHomeRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintTeacherHomeRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTeacherHomeRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintTeacherHomeRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;
}

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms(double wp, QString tc, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;

	this->teacherName=tc;

	this->roomsNames=rms;
}

bool ConstraintTeacherHomeRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForTeacherHash.value(teacherName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		if(act.teachersNames.count()==1){
			assert(act.teachersNames.at(0)==teacherName);
			_activities.append(i);
		}
	}
	qSort(_activities);*/

	//QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	foreach(QString rm, this->roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	return true;
}

bool ConstraintTeacherHomeRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherHomeRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherHomeRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherHomeRooms>\n";

	return s;
}

QString ConstraintTeacherHomeRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher home rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "R means Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintTeacherHomeRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Teacher home rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherHomeRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//	assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=tr("Space constraint teacher home rooms broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherHomeRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintTeacherHomeRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTeacherHomeRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintTeacherHomeRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
}

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
	this->subjectName=subj;
	this->roomName=rm;
}

bool ConstraintSubjectPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForSubjectHash.value(subjectName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.subjectName==subjectName);
		_activities.append(i);
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintSubjectPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintSubjectPreferredRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubjectPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject preferred room"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=", ";
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintSubjectPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint subject preferred room broken for activity with id %1 (%2)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivity(Activity* a)
{
	return a->subjectName==this->subjectName;
}

bool ConstraintSubjectPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintSubjectPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
}

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->roomsNames=rms;
}

bool ConstraintSubjectPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForSubjectHash.value(subjectName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.subjectName==subjectName);
		_activities.append(i);
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	this->_rooms.clear();
	foreach(QString rm, this->roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}

	return true;
}

bool ConstraintSubjectPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintSubjectPreferredRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubjectPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject preferred rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("S:%1", "Subject").arg(this->subjectName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintSubjectPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=tr("Space constraint subject preferred rooms broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivity(Activity* a)
{
	return a->subjectName==this->subjectName;
}

bool ConstraintSubjectPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintSubjectPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
}

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom(double wp, const QString& subj, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintSubjectActivityTagPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();
	
	/*QSet<int> set=r.activitiesForSubjectHash.value(subjectName, QSet<int>());
	QSet<int> set2=r.activitiesForActivityTagHash.value(activityTagName, QSet<int>());
	set.intersect(set2);
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.subjectName==subjectName);
		assert(act.activityTagsNames.contains(activityTagName));
		_activities.append(i);
	}
	qSort(_activities);*/
	
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
		 	this->_activities.append(ac);
		}
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintSubjectActivityTagPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectActivityTagPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintSubjectActivityTagPreferredRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubjectActivityTagPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject activity tag preferred room"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=", ";
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);s+=", ";
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject activity tag preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectActivityTagPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint subject activity tag preferred room broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	return this->subjectName==a->subjectName && a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintSubjectActivityTagPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectActivityTagPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
}

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms(double wp, const QString& subj, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomsNames=rms;
}

bool ConstraintSubjectActivityTagPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForSubjectHash.value(subjectName, QSet<int>());
	QSet<int> set2=r.activitiesForActivityTagHash.value(activityTagName, QSet<int>());
	set.intersect(set2);
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.subjectName==subjectName);
		assert(act.activityTagsNames.contains(activityTagName));
		_activities.append(i);
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
			this->_activities.append(ac);
		}

	this->_rooms.clear();
	foreach(QString rm, roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}
	
	return true;
}

bool ConstraintSubjectActivityTagPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectActivityTagPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintSubjectActivityTagPreferredRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubjectActivityTagPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject activity tag preferred rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=", ";
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject activity tag preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectActivityTagPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=tr("Space constraint subject activity tag preferred rooms broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	return this->subjectName==a->subjectName && a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintSubjectActivityTagPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectActivityTagPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityTagPreferredRoom::ConstraintActivityTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM;
}

ConstraintActivityTagPreferredRoom::ConstraintActivityTagPreferredRoom(double wp, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM;
	this->activityTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintActivityTagPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForActivityTagHash.value(activityTagName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.activityTagsNames.contains(activityTagName));
		_activities.append(i);
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
		 	this->_activities.append(ac);
		}
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintActivityTagPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintActivityTagPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivityTagPreferredRoom>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityTagPreferredRoom>\n";

	return s;
}

QString ConstraintActivityTagPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity tag preferred room"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);s+=", ";
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintActivityTagPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity tag preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityTagPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint activity tag preferred room broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	return a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintActivityTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintActivityTagPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityTagPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityTagPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityTagPreferredRooms::ConstraintActivityTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS;
}

ConstraintActivityTagPreferredRooms::ConstraintActivityTagPreferredRooms(double wp, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS;
	this->activityTagName=subjTag;
	this->roomsNames=rms;
}

bool ConstraintActivityTagPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	/*QSet<int> set=r.activitiesForActivityTagHash.value(activityTagName, QSet<int>());
	foreach(int i, set){
		const Activity& act=r.internalActivitiesList[i];
		assert(act.activityTagsNames.contains(activityTagName));
		_activities.append(i);
	}
	qSort(_activities);*/

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
			this->_activities.append(ac);
		}

	this->_rooms.clear();
	foreach(QString rm, roomsNames){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}
	
	return true;
}

bool ConstraintActivityTagPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintActivityTagPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivityTagPreferredRooms>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Number_of_Preferred_Rooms>"+CustomFETString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityTagPreferredRooms>\n";

	return s;
}

QString ConstraintActivityTagPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity tag preferred rooms"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintActivityTagPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity tag preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityTagPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=tr("Space constraint activity tag preferred rooms broken for activity with id %1 (%2) (activity tag of constraint=%3)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	return a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintActivityTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintActivityTagPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityTagPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityTagPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay(double wp, QString st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->iSubgroupsList.clear();
	
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, NULL);
			
	if(ss==NULL){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per day is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per day"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
						
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=tr("Space constraint students set max building changes per day broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per day"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
						
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=tr("Space constraint students max building changes per day broken for students=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek(double wp, QString st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
	this->studentsName=st;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->iSubgroupsList.clear();
	
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, NULL);
			
	if(ss==NULL){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per week"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
						
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint students set max building changes per week broken for students=%1")
					.arg(this->studentsName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per week"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
						
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint students max building changes per week broken for students=%1")
					.arg(r.internalSubgroupsList[sbg]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges(double wp, QString st, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->studentsName=st;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->iSubgroupsList.clear();
	
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, NULL);
			
	if(ss==NULL){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set min gaps between building changes is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set min gaps between building changes"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=NULL){
								QString s=tr("Space constraint students set min gaps between building changes broken for students=%1 on day %2")
									.arg(this->studentsName)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
		
	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students min gaps between building changes"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, sts->activitiesForSubgroup)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=NULL){
								QString s=tr("Space constraint students min gaps between building changes broken for students=%1 on day %2")
									.arg(r.internalSubgroupsList[sbg]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay(double wp, QString tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per day is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per day"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
			
	foreach(int ai, tchpointer->activitiesForTeacher)
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){			
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d2][h2];
				}
			}
		}
					
		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;
	
			if(conflictsString!=NULL){
				QString s=tr("Space constraint teacher max building changes per day broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per day"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, tchpointer->activitiesForTeacher)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
					
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=tr("Space constraint teachers max building changes per day broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek(double wp, QString tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
	this->teacherName=tc;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per week is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per week"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
			
	foreach(int ai, tchpointer->activitiesForTeacher)
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	int n_changes=0;

	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_building=-1;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d2][h2];
				}
			}
		}
	}
					
	if(n_changes>this->maxBuildingChangesPerWeek){
		nbroken+=n_changes-this->maxBuildingChangesPerWeek;
	
		if(conflictsString!=NULL){
			QString s=tr("Space constraint teacher max building changes per week broken for teacher=%1")
				.arg(this->teacherName);
			s += ". ";
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek)));
			
			dl.append(s);
			cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
		
			*conflictsString+=s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per week"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, tchpointer->activitiesForTeacher)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
					
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=n_changes-this->maxBuildingChangesPerWeek;
		
			if(conflictsString!=NULL){
				QString s=tr("Space constraint teachers max building changes per week broken for teacher=%1")
					.arg(r.internalTeachersList[tch]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges(double wp, QString tc, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->teacherName=tc;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher min gaps between building changes is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher min gaps between building changes"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
			
	foreach(int ai, tchpointer->activitiesForTeacher)
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int h2;
		for(h2=0; h2<r.nHoursPerDay; h2++)
			if(crtBuildingsTimetable[d2][h2]!=-1)
				break;

		int crt_building=-1;					
		if(h2<r.nHoursPerDay)
			crt_building=crtBuildingsTimetable[d2][h2];
		
		int cnt_gaps=0;
		
		for(h2++; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crtBuildingsTimetable[d2][h2]==crt_building)
					cnt_gaps=0;
				else{
					if(cnt_gaps<this->minGapsBetweenBuildingChanges){
						nbroken++;
					
						if(conflictsString!=NULL){
							QString s=tr("Space constraint teacher min gaps between building changes broken for teacher=%1 on day %2")
								.arg(this->teacherName)
								.arg(r.daysOfTheWeek[d2]);
							s += ". ";
							s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100*1));
				
							dl.append(s);
							cl.append(weightPercentage/100*1);
					
							*conflictsString+=s+"\n";
						}
					}
					
					crt_building=crtBuildingsTimetable[d2][h2];
					cnt_gaps=0;
				}
			}
			else
				cnt_gaps++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers min gaps between building changes"); s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		int crtBuildingsTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		foreach(int ai, tchpointer->activitiesForTeacher)
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
				
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
					
							if(conflictsString!=NULL){
								QString s=tr("Space constraint teachers min gaps between building changes broken for teacher=%1 on day %2")
									.arg(r.internalTeachersList[tch]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
					
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesOccupyMaxDifferentRooms::ConstraintActivitiesOccupyMaxDifferentRooms()
	: SpaceConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS;
}

ConstraintActivitiesOccupyMaxDifferentRooms::ConstraintActivitiesOccupyMaxDifferentRooms(double wp,
	QList<int> a_L, int max_different_rooms)
	: SpaceConstraint(wp)
{
	this->activitiesIds=a_L;
	this->maxDifferentRooms=max_different_rooms;
	
	this->type=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->_activitiesIndices.clear();
	
/*	QSet<int> req=this->activitiesIds.toSet();
	assert(req.count()==this->activitiesIds.count());
	
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	int i;
	for(i=0; i<r.nInternalActivities; i++)
		if(req.contains(r.internalActivitiesList[i].id))
			this->_activitiesIndices.append(i);*/
			
	foreach(int id, activitiesIds){
		int index=r.activitiesHash.value(id, -1);
		//assert(index>=0);
		if(index>=0) //take care for inactive activities
			_activitiesIndices.append(index);
	}
			
	///////////////////////
	
	if(this->_activitiesIndices.count()<2){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong (refers to less than two activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	else{
		assert(this->_activitiesIndices.count()>=2);
		return true;
	}
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::hasInactiveActivities(Rules& r)
{
	//returns true if all or all but one activities are inactive
	
	int cnt=0;
	foreach(int aid, this->activitiesIds)
		if(r.inactiveActivities.contains(aid))
			cnt++;
			
	if(this->activitiesIds.count()>=2 && (cnt==this->activitiesIds.count() || cnt==this->activitiesIds.count()-1) )
		return true;
	else
		return false;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesOccupyMaxDifferentRooms>\n";
	
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Number_of_Activities>"+CustomFETString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	foreach(int aid, this->activitiesIds)
		s+="	<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+="	<Max_Number_of_Different_Rooms>"+CustomFETString::number(this->maxDifferentRooms)+"</Max_Number_of_Different_Rooms>\n";
	
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesOccupyMaxDifferentRooms>\n";
	return s;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString s=tr("Activities occupy max different rooms, WP:%1%, NA:%2, A: %3, MDR:%4", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list, MDR means max different rooms")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(CustomFETString::number(this->activitiesIds.count()))
	 .arg(actids)
	 .arg(CustomFETString::number(this->maxDifferentRooms));
	
	return begin+s+end;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getDetailedDescription(Rules& r)
{
	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activities occupy max different rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(CustomFETString::number(this->activitiesIds.count())); s+="\n";
	foreach(int id, this->activitiesIds){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	s+=tr("Maximum number of different rooms=%1").arg(CustomFETString::number(this->maxDifferentRooms)); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesOccupyMaxDifferentRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken=0;
	
	QSet<int> usedRooms;
	
	foreach(int ai, this->_activitiesIndices){
		if(c.rooms[ai]!=UNALLOCATED_SPACE && c.rooms[ai]!=UNSPECIFIED_ROOM)
			if(!usedRooms.contains(c.rooms[ai]))
				usedRooms.insert(c.rooms[ai]);
	}
	
	if(usedRooms.count() > this->maxDifferentRooms){
		nbroken=1;

		if(conflictsString!=NULL){
			QString s=tr("Space constraint activities occupy max different rooms broken");
			s += QString(". ");
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(nbroken*weightPercentage/100));
	
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return nbroken*weightPercentage/100;
}

void ConstraintActivitiesOccupyMaxDifferentRooms::removeUseless(Rules& r)
{
	/*QSet<int> validActs;
	
	foreach(Activity* act, r.activitiesList)
		validActs.insert(act->id);*/
		
	QList<int> newActs;
	
	foreach(int aid, activitiesIds){
		Activity* act=r.activitiesPointerHash.value(aid, NULL);
		if(act!=NULL)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
			
	activitiesIds=newActs;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToActivity(Activity* a)
{
	return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameRoomIfConsecutive::ConstraintActivitiesSameRoomIfConsecutive()
	: SpaceConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE;
}

ConstraintActivitiesSameRoomIfConsecutive::ConstraintActivitiesSameRoomIfConsecutive(double wp,
	QList<int> a_L)
	: SpaceConstraint(wp)
{
	this->activitiesIds=a_L;
	
	this->type=CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE;
}

bool ConstraintActivitiesSameRoomIfConsecutive::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	_activitiesIndices.clear();
	foreach(int id, activitiesIds){
		int i=r.activitiesHash.value(id, -1);
		if(i>=0)
			_activitiesIndices.append(i);
	}

	/*this->_activitiesIndices.clear();
	
	QSet<int> req=this->activitiesIds.toSet();
	assert(req.count()==this->activitiesIds.count());
	
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	int i;
	for(i=0; i<r.nInternalActivities; i++)
		if(req.contains(r.internalActivitiesList[i].id))
			this->_activitiesIndices.append(i);*/
			
	///////////////////////
	
	if(this->_activitiesIndices.count()<2){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong (refers to less than two activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	else{
		assert(this->_activitiesIndices.count()>=2);
		return true;
	}
}

bool ConstraintActivitiesSameRoomIfConsecutive::hasInactiveActivities(Rules& r)
{
	//returns true if all or all but one activities are inactive
	
	int cnt=0;
	foreach(int aid, this->activitiesIds)
		if(r.inactiveActivities.contains(aid))
			cnt++;
			
	if(this->activitiesIds.count()>=2 && (cnt==this->activitiesIds.count() || cnt==this->activitiesIds.count()-1) )
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesSameRoomIfConsecutive>\n";
	
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Number_of_Activities>"+CustomFETString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	foreach(int aid, this->activitiesIds)
		s+="	<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesSameRoomIfConsecutive>\n";
	return s;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString s=tr("Activities same room if consecutive, WP:%1%, NA:%2, A: %3", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(CustomFETString::number(this->activitiesIds.count()))
	 .arg(actids);
	
	return begin+s+end;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getDetailedDescription(Rules& r)
{
	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activities same room if consecutive"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(CustomFETString::number(this->activitiesIds.count())); s+="\n";
	foreach(int id, this->activitiesIds){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesSameRoomIfConsecutive::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken=0;
	
	for(int i=0; i<_activitiesIndices.count(); i++){
		int ai=_activitiesIndices.at(i);
		for(int j=i+1; j<_activitiesIndices.count(); j++){
			int ai2=_activitiesIndices.at(j);
			
			if(c.times[ai]!=UNALLOCATED_TIME && c.times[ai2]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek;
				int h=c.times[ai]/r.nDaysPerWeek;
				int d2=c.times[ai2]%r.nDaysPerWeek;
				int h2=c.times[ai2]/r.nDaysPerWeek;
			
				if( (d==d2) && (h+r.internalActivitiesList[ai].duration==h2 || h2+r.internalActivitiesList[ai2].duration==h) )
					if(c.rooms[ai]!=UNALLOCATED_SPACE && c.rooms[ai]!=UNSPECIFIED_ROOM)
						if(c.rooms[ai2]!=UNALLOCATED_SPACE && c.rooms[ai2]!=UNSPECIFIED_ROOM)
							if(c.rooms[ai]!=c.rooms[ai2])
								nbroken++;
			}
		}
	}
	
	if(nbroken>0){
		if(conflictsString!=NULL){
			QString s=tr("Space constraint activities same rooms if consecutive broken");
			s += QString(". ");
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::number(nbroken*weightPercentage/100));
	
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return nbroken*weightPercentage/100;
}

void ConstraintActivitiesSameRoomIfConsecutive::removeUseless(Rules& r)
{
	/*QSet<int> validActs;
	
	foreach(Activity* act, r.activitiesList)
		validActs.insert(act->id);*/
		
	QList<int> newActs;
	
	foreach(int aid, activitiesIds){
		Activity* act=r.activitiesPointerHash.value(aid, NULL);
		if(act!=NULL)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
			
	activitiesIds=newActs;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToActivity(Activity* a)
{
	return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivitiesSameRoomIfConsecutive::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
