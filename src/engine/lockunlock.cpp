/*
File lockunlock.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          lockunlock.cpp  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (http://lalescu.ro/liviu/) and Volker Dirr (http://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "lockunlock.h"
#include "timetable.h"

extern Timetable gt;

QSet<int> idsOfLockedTime;
QSet<int> idsOfLockedSpace;
QSet<int> idsOfPermanentlyLockedTime;
QSet<int> idsOfPermanentlyLockedSpace;

CommunicationSpinBox communicationSpinBox;


CommunicationSpinBox::CommunicationSpinBox()
{
}

void CommunicationSpinBox::increaseValue()
{
	emit(valueChanged());
}


void LockUnlock::computeLockedUnlockedActivitiesTimeSpace()
{
	computeLockedUnlockedActivitiesOnlyTime();
	computeLockedUnlockedActivitiesOnlySpace();
}

void LockUnlock::computeLockedUnlockedActivitiesOnlyTime()
{
	//by Volker Dirr
	idsOfLockedTime.clear();
	idsOfPermanentlyLockedTime.clear();

	foreach(const TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME && tc->weightPercentage==100.0 && tc->active){
			const ConstraintActivityPreferredStartingTime* c=(const ConstraintActivityPreferredStartingTime*) tc;
			if(c->day >= 0  &&  c->hour >= 0) {
				if(c->permanentlyLocked)
					idsOfPermanentlyLockedTime.insert(c->activityId);
				else
					idsOfLockedTime.insert(c->activityId);
			}
		}
	}
}

void LockUnlock::computeLockedUnlockedActivitiesOnlySpace()
{
	//by Volker Dirr
	idsOfLockedSpace.clear();
	idsOfPermanentlyLockedSpace.clear();

	foreach(const SpaceConstraint* sc, gt.rules.spaceConstraintsList){
		if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM && sc->weightPercentage==100.0 && sc->active){
			const ConstraintActivityPreferredRoom* c=(const ConstraintActivityPreferredRoom*) sc;

			if(c->permanentlyLocked)
				idsOfPermanentlyLockedSpace.insert(c->activityId);
			else
				idsOfLockedSpace.insert(c->activityId);
		}
	}
}

void LockUnlock::increaseCommunicationSpinBox()
{
	communicationSpinBox.increaseValue();
}
