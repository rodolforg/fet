/*
File lockunlock.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          lockunlock.h  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (https://lalescu.ro/liviu/) and Volker Dirr (http://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef LOCKUNLOCK_H
#define LOCKUNLOCK_H

#include <QObject>
#include "errorcode.h"

class Rules;
class ConstraintActivityPreferredStartingTime;
class ConstraintActivityPreferredRoom;

class CommunicationSpinBox: public QObject{
	Q_OBJECT
	
public:
	CommunicationSpinBox();
	
signals:
	void valueChanged();
	
public slots:
	void increaseValue();
};


class LockUnlock{
public:
	static CommunicationSpinBox communicationSpinBox; // small hint to sync the forms

	static void computeLockedUnlockedActivitiesTimeSpace();
	static void computeLockedUnlockedActivitiesOnlyTime();
	static void computeLockedUnlockedActivitiesOnlySpace();

	/// Reports whether an activity has any kind of time lock
	static bool isActivityTimeLocked(int activityId);
	/// Reports whether an activity time is permanently locked
	static bool isActivityTimePermLocked(int activityId);

	/// Reports whether an activity has any kind of space lock
	static bool isActivitySpaceLocked(int activityId);
	/// Reports whether an activity space is permanently locked
	static bool isActivitySpacePermLocked(int activityId);

	/// Return a not-NULL pointer if activity time-lock is successfully added (not repeated)
	static ConstraintActivityPreferredStartingTime* lockTime(Rules* rules, int activityId, int day, int hour);

	/** Remove a time-lock for an activity.
	 * @param[in] rules The timetable rules
	 * @param[in] activityId The activity id
	 * @param[out] nUnlocked The number of undone locks. There should be only one time-lock per activityID, but...
	 * @return list of errors or messages. @see ErrorList::hasError() and ErrorRenderer
	 */
	static ErrorList unlockTime(Rules* rules, int activityId, int& nUnlocked);

	/// Return a not-NULL pointer if activity space-lock is successfully added (not repeated)
	static ConstraintActivityPreferredRoom* lockSpace(Rules* rules, int activityId, const QString& roomName);

	/** Remove a space-lock for an activity.
	 * @param[in] rules The timetable rules
	 * @param[in] activityId The activity id
	 * @param[out] nUnlocked The number of undone locks. There should be only one space-lock per activityID, but...
	 * @return list of errors or messages. @see ErrorList::hasError() and ErrorRenderer
	 */
	static ErrorList unlockSpace(Rules* rules, int activityId, int& nUnlocked);

	static void increaseCommunicationSpinBox();

	static QString getActivityLockTipString(int activityId);
	static QString getActivityLockDetailsString(int activityId);

	static void assertIsUpdated();

private:
	static QSet<int> idsOfLockedTime;
	static QSet<int> idsOfLockedSpace;
	static QSet<int> idsOfPermanentlyLockedTime;
	static QSet<int> idsOfPermanentlyLockedSpace;
};

#endif
