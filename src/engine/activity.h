/*
File activity.h
*/

/***************************************************************************
                          activity.h  -  description
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

#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QSet>
#include <QStringList>

class Rules;
class Activity;

typedef QList<Activity*> ActivitiesList;

class GroupActivitiesInInitialOrderItem{
	Q_DECLARE_TR_FUNCTIONS(GroupActivitiesInInitialOrderItem)

public:
	GroupActivitiesInInitialOrderItem();
	~GroupActivitiesInInitialOrderItem();

	QString comments;
	bool active;

	QList<int> ids;
	QList<int> indices; //in the rules internal activities list

	QString getXmlDescription(Rules& r);
	QString getDescription(Rules& r);
	QString getDetailedDescription(Rules& r);
	
	void removeUseless(Rules& r);
};

typedef QList<GroupActivitiesInInitialOrderItem*> GroupActivitiesInInitialOrderList;

/**
This class represents an activity.
<p>
An activity is a certain course (lecture), taught by a certain teacher (or more),
to a certain year (or group, or subgroup) of students (or more).
*/

class Activity{
	Q_DECLARE_TR_FUNCTIONS(Activity)

public:
	QString comments;

	/**
	The teachers' names.
	*/
	QStringList teachersNames;

	/**
	The name of the subject.
	*/
	QString subjectName;

	/**
	The name of the activity tag.
	*/
	//QString activityTagName;
	QStringList activityTagsNames;

	/**
	The names of the sets of students involved in this activity (years, groups or subgroups).
	*/
	QStringList studentsNames;

	/**
	The duration, in hours.
	*/
	int duration;

	/**
	The parity: weekly (PARITY_WEEKLY) or once at two weeks (PARITY_FORTNIGHTLY).
	*/
	//int parity;

	/**
	This value is used only for split activities (for high-schools).
	If totalDuration==duration, then this activity is not split.
	If totalDuration>duration, then this activity is split.
	*/
	int totalDuration;

	/**
	A unique ID for any activity. This is NOT the index (activities might be erased,
	but this ID remains the same).
	*/
	int id;

	/**
	The activities generated from a split activity have the same activityGroupId.
	For non-split activities, activityGroupId==0
	*/
	int activityGroupId;

	/**
	The number of students who are attending this activity.
	If computeNTotalStudentsFromSets is false, this number
	is given. If it is true, this number should be calculated
	from the sets involved.
	*/
	int nTotalStudents;
	
	/**
	If true, we will have to compute the number of total students from the 
	involved students sets. If false, it means that nTotalStudents is given
	and must not be recalculated.
	*/
	bool computeNTotalStudents;
	
	/**
	True if this activity is active, that is it will be taken into consideration
	when generating the timetable.
	*/
	bool active;
	
	/**
	If the teachers, subject, activity tag, students, duration are identical
	and the activity group id of both of them is 0 or of both of them is != 0, returns true.
	TODO: add a more intelligent comparison
	*/
	bool operator==(Activity &a);

	//internal structure
	
	/**
	The number of teachers who are teaching this activity
	*/
	//int nTeachers;
	
	/**
	The indices of the teachers who are teaching this activity.
	*/
	//int teachers[MAX_TEACHERS_PER_ACTIVITY];
	QList<int> iTeachersList;

	/**
	The index of the subject.
	*/
	int subjectIndex;

	/**
	The indices of the activity tags.
	*/
	QSet<int> iActivityTagsSet;
	//int activityTagIndex;

	/**
	The number of subgroups implied in this activity.
	*/
	//int nSubgroups;

	/**
	The indices of the subgroups implied in this activity.
	*/
	//int subgroups[MAX_SUBGROUPS_PER_ACTIVITY];
	QList<int> iSubgroupsList;
	
	/**
	Simple constructor, used only indirectly by the static variable
	"Activity internalActivitiesList[MAX_ACTIVITIES]".
	Any other use of this function should be avoided.
	*/
	Activity();

	/**
	Complete constructor.
	If _totalDuration!=duration, then this activity is a part of a bigger (split)
	activity.
	<p>
	As a must, for non-split activities, _activityGroupId==0.
	For the split ones, it is >0
	*/
	/*Activity(
		Rules& r,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		//int _parity,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents);*/
		
	//deprecated comment below - this function is used in more places.
	//this is used only when reading a file (Rules), so that the computed number of students is known faster
	Activity(
		Rules& r,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		//int _parity,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		int _computedNumberOfStudents);
		
	bool searchTeacher(const QString& teacherName);

	/**
	Removes this teacher from the list of teachers
	*/
	bool removeTeacher(const QString& teacherName);

	/**
	Renames this teacher in the list of teachers
	*/
	void renameTeacher(const QString& initialTeacherName, const QString& finalTeacherName);
	
	bool searchStudents(const QString& studentsName);

	/**
	Removes this students set from the list of students
	*/
	bool removeStudents(Rules& r, const QString& studentsName, int nStudents);

	/**
	Renames this students set in the list of students and possibly modifies the number of students for the activity, if initialNumberOfStudents!=finalNumberOfStudents
	*/
	void renameStudents(Rules& r, const QString& initialStudentsName, const QString& finalStudentsName, int initialNumberOfStudents, int finalNumberOfStudents);

	/**
	Computes the internal structure
	*/
	void computeInternalStructure(Rules& r);

	/**
	Returns a representation of this activity (xml format).
	*/
	QString getXmlDescription(Rules& r);

	/**
	Returns a representation of this activity.
	*/
	QString getDescription(Rules& r);

	/**
	Returns a representation of this activity (more detailed).
	*/
	QString getDetailedDescription(Rules& r);

	/**
	Returns a representation of this activity (detailed),
	together with the constraints related to this activity.
	*/
	QString getDetailedDescriptionWithConstraints(Rules& r);

	/**
	Returns true if this activity is split into more lessons per week.
	*/
	bool isSplit();
	
	bool representsComponentNumber(int compNumber);
};

#endif
