/*
File lockunlock.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          lockunlock.cpp  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (https://lalescu.ro/liviu/) and Volker Dirr (https://www.timetabling.de/)
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

QSet<int> LockUnlock::idsOfLockedTime;
QSet<int> LockUnlock::idsOfLockedSpace;
QSet<int> LockUnlock::idsOfPermanentlyLockedTime;
QSet<int> LockUnlock::idsOfPermanentlyLockedSpace;

CommunicationSpinBox LockUnlock::communicationSpinBox;


CommunicationSpinBox::CommunicationSpinBox()
{
}

void CommunicationSpinBox::increaseValue()
{
	emit(valueChanged());
}


void LockUnlock::computeLockedUnlockedActivitiesTimeSpace(const Rules* rules)
{
	computeLockedUnlockedActivitiesOnlyTime(rules);
	computeLockedUnlockedActivitiesOnlySpace(rules);
}

void LockUnlock::computeLockedUnlockedActivitiesOnlyTime(const Rules* rules)
{
	//by Volker Dirr
	idsOfLockedTime.clear();
	idsOfPermanentlyLockedTime.clear();

	for(TimeConstraint* tc : qAsConst(rules->timeConstraintsList)){
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

void LockUnlock::computeLockedUnlockedActivitiesOnlySpace(const Rules* rules)
{
	//by Volker Dirr
	idsOfLockedSpace.clear();
	idsOfPermanentlyLockedSpace.clear();

	for(const SpaceConstraint* sc : qAsConst(rules->spaceConstraintsList)){
		if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM && sc->weightPercentage==100.0 && sc->active){
			const ConstraintActivityPreferredRoom* c=(const ConstraintActivityPreferredRoom*) sc;

			if(c->permanentlyLocked)
				idsOfPermanentlyLockedSpace.insert(c->activityId);
			else
				idsOfLockedSpace.insert(c->activityId);
		}
	}
}

bool LockUnlock::isActivityTimeLocked(int activityId)
{
	return idsOfLockedTime.contains(activityId) || idsOfPermanentlyLockedTime.contains(activityId);
}

bool LockUnlock::isActivityTimePermLocked(int activityId)
{
	return idsOfPermanentlyLockedTime.contains(activityId);
}

bool LockUnlock::isActivitySpaceLocked(int activityId)
{
	return idsOfLockedSpace.contains(activityId) || idsOfPermanentlyLockedSpace.contains(activityId);
}

bool LockUnlock::isActivitySpacePermLocked(int activityId)
{
	return idsOfPermanentlyLockedSpace.contains(activityId);
}

ConstraintActivityPreferredStartingTime* LockUnlock::lockTime(Rules* rules, int activityId, int day, int hour)
{
	bool ruleWasAlreadyComputed = rules->internalStructureComputed;

	ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, activityId, day, hour, false);
	bool added = rules->addTimeConstraint(ctr);
	if (!added) {
		delete ctr;
		return NULL;
	}
	idsOfLockedTime.insert(activityId);

	if (ruleWasAlreadyComputed && !rules->internalStructureComputed) {
		int nComputedItems = 0;
		bool canceled = false;
		rules->computeInternalTimeConstraintList(nComputedItems, canceled);
		rules->internalStructureComputed = true;
	}
	return ctr;
}

ErrorList LockUnlock::unlockTime(Rules* rules, int activityId, int& nUnlocked)
{
	ErrorList errors;

	QList<TimeConstraint*> tmptc;
	tmptc.clear();
	int count=0;

	for(ConstraintActivityPreferredStartingTime* c : rules->apstHash.value(activityId, QSet<ConstraintActivityPreferredStartingTime*>())){
		assert(c->activityId == activityId);
		if(c->activityId==activityId && c->weightPercentage==100.0 && c->active && c->day>=0 && c->hour>=0){
			count++;
			if(c->permanentlyLocked){
				if(idsOfLockedTime.contains(c->activityId) || !idsOfPermanentlyLockedTime.contains(c->activityId)){
					errors << ErrorCode(ErrorCode::Warning,
										QCoreApplication::translate("TimetableViewForm", "Small problem detected")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(activityId)
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "Please report possible bug")
					);
				}
				else{
					errors << ErrorCode(ErrorCode::Warning,
										QCoreApplication::translate("TimetableViewForm",
																	"Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(*rules)+"\n"));
				}
			}
			else{
				if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
					errors << ErrorCode(ErrorCode::Warning,
										 QCoreApplication::translate("TimetableViewForm", "Small problem detected")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(activityId)
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "Please report possible bug")
					);
				}
				else{
					tmptc.append((TimeConstraint*)c);
				}
			}
		}
	}
	if(count!=1)
		errors << ErrorCode(ErrorCode::Warning,
							QCoreApplication::translate("TimetableViewForm", "You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

	nUnlocked = 0;
	for(TimeConstraint* deltc : qAsConst(tmptc)){
		errors << ErrorCode(ErrorCode::Info,
							QCoreApplication::translate("TimetableViewForm", "The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(*rules)+"\n");
		bool ruleWasAlreadyComputed = rules->internalStructureComputed;
		rules->removeTimeConstraint(deltc);
		idsOfLockedTime.remove(activityId);
		nUnlocked++;
		if (ruleWasAlreadyComputed && !rules->internalStructureComputed) {
			int nComputedItems = 0;
			bool canceled = false;
			rules->computeInternalTimeConstraintList(nComputedItems, canceled);
			rules->internalStructureComputed = true;
		}
	}

	return errors;
}

ConstraintActivityPreferredRoom* LockUnlock::lockSpace(Rules* rules, int activityId, const QString& roomName)
{
	bool ruleWasAlreadyComputed = rules->internalStructureComputed;

	ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, activityId, roomName, false);
	bool added = rules->addSpaceConstraint(ctr);
	if(!added){
		delete ctr;
		return NULL;
	}
	idsOfLockedSpace.insert(activityId);

	if (ruleWasAlreadyComputed && !rules->internalStructureComputed) {
		int nComputedItems = 0;
		bool canceled = false;
		rules->computeInternalSpaceConstraintList(nComputedItems, canceled);
		rules->internalStructureComputed = true;
	}
	return ctr;
}

ErrorList LockUnlock::unlockSpace(Rules* rules, int activityId, int& nUnlocked)
{
	ErrorList errors;

	QList<SpaceConstraint*> tmpsc;
	tmpsc.clear();
	int count=0;

	for(ConstraintActivityPreferredRoom* c : rules->aprHash.value(activityId, QSet<ConstraintActivityPreferredRoom*>())){
		assert(c->activityId==activityId);
		if(c->activityId==activityId && c->weightPercentage==100.0 && c->active){
			count++;
			if(c->permanentlyLocked){
				if(idsOfLockedSpace.contains(c->activityId) || !idsOfPermanentlyLockedSpace.contains(c->activityId)){
					errors << ErrorCode(ErrorCode::Warning,
										QCoreApplication::translate("TimetableViewForm", "Small problem detected")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(activityId)
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "Please report possible bug")
					);
				}
				else{
					errors << ErrorCode(ErrorCode::Warning,
										QCoreApplication::translate("TimetableViewForm",
																	"Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(*rules)+"\n"));
				}
			}
			else{
				if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
					errors << ErrorCode(ErrorCode::Warning,
										QCoreApplication::translate("TimetableViewForm", "Small problem detected")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(activityId)
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
					  +"\n\n"+QCoreApplication::translate("TimetableViewForm", "Please report possible bug")
					);
				}
				else{
					tmpsc.append((SpaceConstraint*)c);
				}
			}
		}
	}
	if(count!=1)
		errors << ErrorCode(ErrorCode::Warning,
							QCoreApplication::translate("TimetableViewForm", "You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

	nUnlocked = 0;
	for(SpaceConstraint* delsc : tmpsc){
		errors << ErrorCode(ErrorCode::Info,
							QCoreApplication::translate("TimetableViewForm", "The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(*rules)+"\n");
		bool ruleWasAlreadyComputed = rules->internalStructureComputed;
		rules->removeSpaceConstraint(delsc);
		idsOfLockedSpace.remove(activityId);
		nUnlocked++;
		if (ruleWasAlreadyComputed && !rules->internalStructureComputed) {
			int nComputedItems = 0;
			bool canceled = false;
			rules->computeInternalSpaceConstraintList(nComputedItems, canceled);
			rules->internalStructureComputed = true;
		}
	}

	return errors;
}

void LockUnlock::increaseCommunicationSpinBox()
{
	communicationSpinBox.increaseValue();
}

QString LockUnlock::getActivityLockTipString(int activityId)
{
	//added by Volker Dirr (start)
	QString descr="";
	QString t="";
	if(idsOfPermanentlyLockedTime.contains(activityId)){
		descr+=QCoreApplication::translate("TimetableViewForm", "PLT", "Abbreviation for permanently locked time. There are 4 string: permanently locked time, permanently locked space, "
			"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
			" These abbreviations may appear also in other places, please use the same abbreviations.");
		t=", ";
	}
	else if(idsOfLockedTime.contains(activityId)){
		descr+=QCoreApplication::translate("TimetableViewForm", "LT", "Abbreviation for locked time. There are 4 string: permanently locked time, permanently locked space, "
			"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
			" These abbreviations may appear also in other places, please use the same abbreviations.");
		t=", ";
	}
	if(idsOfPermanentlyLockedSpace.contains(activityId)){
		descr+=t+QCoreApplication::translate("TimetableViewForm", "PLS", "Abbreviation for permanently locked space. There are 4 string: permanently locked time, permanently locked space, "
			"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
			" These abbreviations may appear also in other places, please use the same abbreviations.");
	}
	else if(idsOfLockedSpace.contains(activityId)){
		descr+=t+QCoreApplication::translate("TimetableViewForm", "LS", "Abbreviation for locked space. There are 4 string: permanently locked time, permanently locked space, "
			"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
			" These abbreviations may appear also in other places, please use the same abbreviations.");
	}
	if(descr!=""){
		descr.prepend("\n(");
		descr.append(")");
	}
	return descr;
	//added by Volker Dirr (end)
}

QString LockUnlock::getActivityLockDetailsString(int activityId)
{
	//added by Volker Dirr (start)
	QString descr="";
	QString t="";
	if(idsOfPermanentlyLockedTime.contains(activityId)){
		descr+=QCoreApplication::translate("TimetableViewForm", "permanently locked time", "refers to activity");
		t=", ";
	}
	else if(idsOfLockedTime.contains(activityId)){
		descr+=QCoreApplication::translate("TimetableViewForm", "locked time", "refers to activity");
		t=", ";
	}
	if(idsOfPermanentlyLockedSpace.contains(activityId)){
		descr+=t+QCoreApplication::translate("TimetableViewForm", "permanently locked space", "refers to activity");
	}
	else if(idsOfLockedSpace.contains(activityId)){
		descr+=t+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
	}
	if(descr!=""){
		descr.prepend("\n(");
		descr.append(")");
	}
	return descr;
	//added by Volker Dirr (end)
}

void LockUnlock::assertIsUpdated(const Rules* rules)
{
	////////// just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(rules); //not needed, just for testing

	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
	///////////
}
