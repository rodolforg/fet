/*
File spaceconstraint.h
*/

/***************************************************************************
                          spaceconstraint.h  -  description
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

#ifndef SPACECONSTRAINT_H
#define SPACECONSTRAINT_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Rules;
class SpaceConstraint;
class Activity;
class Teacher;
class Subject;
class ActivityTag;
class StudentsSet;
class Equipment;
class Building;
class Room;
class Solution;

class QWidget;

typedef QList<SpaceConstraint*> SpaceConstraintsList;

const int CONSTRAINT_GENERIC_SPACE										=1000; //time constraints are beginning from 1

const int CONSTRAINT_BASIC_COMPULSORY_SPACE								=1001; //space constraints from 1001
const int CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES							=1002;

const int CONSTRAINT_ACTIVITY_PREFERRED_ROOM							=1003;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOMS							=1004;

const int CONSTRAINT_STUDENTS_SET_HOME_ROOM								=1005;
const int CONSTRAINT_STUDENTS_SET_HOME_ROOMS							=1006;

const int CONSTRAINT_TEACHER_HOME_ROOM									=1007;
const int CONSTRAINT_TEACHER_HOME_ROOMS									=1008;

const int CONSTRAINT_SUBJECT_PREFERRED_ROOM								=1009;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOMS							=1010;
const int CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM				=1011;
const int CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS				=1012;

const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY				=1013;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY			=1014;
const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK				=1015;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK			=1016;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1017;
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES		=1018;

const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY				=1019;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY				=1020;
const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK				=1021;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK				=1022;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1023;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1024;

const int CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM						=1025;
const int CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS						=1026;

const int CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS				=1027;
const int CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE				=1028;

/**
This class represents a space constraint
*/
class SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(SpaceConstraint)
	
public:
	/**
	The weight (percentage) of this constraint
	*/
	double weightPercentage;
	
	bool active;
	
	QString comments;

	/**
	Specifies the type of this constraint (using the above constants).
	*/
	int type;

	/**
	Dummy constructor - needed for the static array of constraints.
	Use of this function must be avoided.
	*/
	SpaceConstraint();
	
	virtual ~SpaceConstraint()=0;

	/**
	Constructor - please note that the maximum allowed weight is 100.0
	The reason: unallocated activities must have very big conflict weight,
	and any other restrictions must have much more lower weight,
	so that the timetable can evolve when starting with uninitialized activities
	*/
	SpaceConstraint(double wp);

	/**
	The function that calculates the fitness of a solution, according to this
	constraint. We need the rules to compute this fitness factor.
	We need also the allocation of the activities on days and hours.
	If conflictsString!=NULL,
	it will be initialized with a text explaining where this restriction is broken.
	*/
	virtual double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL)=0;

	/**
	Returns an XML description of this constraint
	*/
	virtual QString getXmlDescription(Rules& r)=0;

	/**
	Computes the internal structure for this constraint
	*/
	virtual bool computeInternalStructure(QWidget* parent, Rules& r)=0;
	
	virtual bool hasInactiveActivities(Rules& r)=0;

	/**
	Returns a small description string for this constraint
	*/
	virtual QString getDescription(Rules& r)=0;

	/**
	Returns a detailed description string for this constraint
	*/
	virtual QString getDetailedDescription(Rules& r)=0;
	
	/**
	Returns true if this constraint is related to this activity
	*/
	virtual bool isRelatedToActivity(Activity* a)=0;

	/**
	Returns true if this constraint is related to this teacher
	*/
	virtual bool isRelatedToTeacher(Teacher* t)=0;

	/**
	Returns true if this constraint is related to this subject
	*/
	virtual bool isRelatedToSubject(Subject* s)=0;

	/**
	Returns true if this constraint is related to this activity tag
	*/
	virtual bool isRelatedToActivityTag(ActivityTag* s)=0;

	/**
	Returns true if this constraint is related to this students set
	*/
	virtual bool isRelatedToStudentsSet(Rules& r, StudentsSet* s)=0;

	/**
	Returns true if this constraint is related to this room
	*/
	virtual bool isRelatedToRoom(Room* r)=0;

	virtual bool hasWrongDayOrHour(Rules& r)=0;
	virtual bool canRepairWrongDayOrHour(Rules& r)=0;
	virtual bool repairWrongDayOrHour(Rules& r)=0;
};

/**
This class comprises all the basic compulsory constraints (constraints
which must be fulfilled for any timetable) - the space allocation part
*/
class ConstraintBasicCompulsorySpace: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintBasicCompulsorySpace)
	
public:
	ConstraintBasicCompulsorySpace();

	ConstraintBasicCompulsorySpace(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintRoomNotAvailableTimes: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintRoomNotAvailableTimes)
	
public:

	QList<int> days;
	QList<int> hours;

	/**
	The room's name
	*/
	QString room;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;

	ConstraintRoomNotAvailableTimes();

	ConstraintRoomNotAvailableTimes(double wp, const QString& rn, QList<int> d, QList<int> h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: an activity must take part in
the preferred room.
*/
class ConstraintActivityPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredRoom)
	
public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The index of the room
	int _room;

	//----------------------------------------------------------

	int activityId;

	QString roomName;
	
	bool permanentlyLocked;

	ConstraintActivityPreferredRoom();

	ConstraintActivityPreferredRoom(double wp, int aid, const QString& room, bool perm);
	
	/**
	Comparison operator - to be sure we do not introduce duplicates
	*/
	bool operator==(ConstraintActivityPreferredRoom& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: an activity must take part in
the preferred rooms.
*/
class ConstraintActivityPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredRooms)

public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The indexes of the rooms
	QList<int> _rooms;

	//----------------------------------------------------------

	int activityId;

	QStringList roomsNames;

	ConstraintActivityPreferredRooms();

	ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetHomeRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetHomeRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
public:

	QString studentsName;

	QString roomName;

	ConstraintStudentsSetHomeRoom();

	ConstraintStudentsSetHomeRoom(double wp, QString st, QString rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetHomeRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetHomeRooms)

public:
	
	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	QList<int> _activities;
	
	//The indexes of the rooms
	QList<int> _rooms;
	
public:

	QString studentsName;

	QStringList roomsNames;

	ConstraintStudentsSetHomeRooms();

	ConstraintStudentsSetHomeRooms(double wp, QString st, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherHomeRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherHomeRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
public:

	QString teacherName;

	QString roomName;

	ConstraintTeacherHomeRoom();

	ConstraintTeacherHomeRoom(double wp, QString tc, QString rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherHomeRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherHomeRooms)

public:
	
	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	QList<int> _activities;
	
	//The indexes of the rooms
	QList<int> _rooms;
	
public:

	QString teacherName;

	QStringList roomsNames;

	ConstraintTeacherHomeRooms();

	ConstraintTeacherHomeRooms(double wp, QString st, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
a certain room.
*/
class ConstraintSubjectPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectPreferredRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
public:

	QString subjectName;

	QString roomName;

	ConstraintSubjectPreferredRoom();

	ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
certain rooms.
*/
class ConstraintSubjectPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectPreferredRooms)

public:

	QList<int> _activities;
	
	QList<int> _rooms;
	
public:

	QString subjectName;

	QStringList roomsNames;

	ConstraintSubjectPreferredRooms();

	ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject+subject tag must be taught in
a certain room.
*/
class ConstraintSubjectActivityTagPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectActivityTagPreferredRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
public:

	QString subjectName;

	QString activityTagName;

	QString roomName;

	ConstraintSubjectActivityTagPreferredRoom();

	ConstraintSubjectActivityTagPreferredRoom(double wp, const QString& subj, const QString& actTag, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject+subject tag must be taught in
certain rooms.
*/
class ConstraintSubjectActivityTagPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectActivityTagPreferredRooms)

public:
	
	QList<int> _activities;
	
	QList<int> _rooms;

public:

	QString subjectName;

	QString activityTagName;

	QStringList roomsNames;

	ConstraintSubjectActivityTagPreferredRooms();

	ConstraintSubjectActivityTagPreferredRooms(double wp, const QString& subj, const QString& actTag, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//addded on 6 apr 2009
class ConstraintActivityTagPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityTagPreferredRoom)
	
public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
public:

	QString activityTagName;

	QString roomName;

	ConstraintActivityTagPreferredRoom();

	ConstraintActivityTagPreferredRoom(double wp, const QString& actTag, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityTagPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityTagPreferredRooms)

public:
	
	QList<int> _activities;
	
	QList<int> _rooms;

public:

	QString activityTagName;

	QStringList roomsNames;

	ConstraintActivityTagPreferredRooms();

	ConstraintActivityTagPreferredRooms(double wp, const QString& actTag, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
///////

class ConstraintStudentsSetMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerDay)

public:
	//internal variables
	QList<int> iSubgroupsList;

public:

	int maxBuildingChangesPerDay;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerDay();

	ConstraintStudentsSetMaxBuildingChangesPerDay(double wp, QString st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintStudentsMaxBuildingChangesPerDay();

	ConstraintStudentsMaxBuildingChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerWeek)

public:
	//internal variables
	QList<int> iSubgroupsList;

public:

	int maxBuildingChangesPerWeek;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerWeek();

	ConstraintStudentsSetMaxBuildingChangesPerWeek(double wp, QString st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerWeek)

public:

	int maxBuildingChangesPerWeek;

	ConstraintStudentsMaxBuildingChangesPerWeek();

	ConstraintStudentsMaxBuildingChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenBuildingChanges)

public:
	//internal variables
	QList<int> iSubgroupsList;

public:

	int minGapsBetweenBuildingChanges;

	QString studentsName;

	ConstraintStudentsSetMinGapsBetweenBuildingChanges();

	ConstraintStudentsSetMinGapsBetweenBuildingChanges(double wp, QString st, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenBuildingChanges)

public:

	int minGapsBetweenBuildingChanges;

	ConstraintStudentsMinGapsBetweenBuildingChanges();

	ConstraintStudentsMinGapsBetweenBuildingChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerDay)

public:
	//internal variables
	int teacher_ID;

public:

	int maxBuildingChangesPerDay;

	QString teacherName;

	ConstraintTeacherMaxBuildingChangesPerDay();

	ConstraintTeacherMaxBuildingChangesPerDay(double wp, QString tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintTeachersMaxBuildingChangesPerDay();

	ConstraintTeachersMaxBuildingChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerWeek)

public:
	//internal variables
	int teacher_ID;

public:

	int maxBuildingChangesPerWeek;

	QString teacherName;

	ConstraintTeacherMaxBuildingChangesPerWeek();

	ConstraintTeacherMaxBuildingChangesPerWeek(double wp, QString tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerWeek)

public:

	int maxBuildingChangesPerWeek;

	ConstraintTeachersMaxBuildingChangesPerWeek();

	ConstraintTeachersMaxBuildingChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenBuildingChanges)

public:
	//internal variables
	int teacher_ID;

public:

	int minGapsBetweenBuildingChanges;

	QString teacherName;

	ConstraintTeacherMinGapsBetweenBuildingChanges();

	ConstraintTeacherMinGapsBetweenBuildingChanges(double wp, QString tc, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenBuildingChanges)

public:

	int minGapsBetweenBuildingChanges;

	ConstraintTeachersMinGapsBetweenBuildingChanges();

	ConstraintTeachersMinGapsBetweenBuildingChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMaxDifferentRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMaxDifferentRooms)

public:
	QList<int> activitiesIds;
	
	int maxDifferentRooms;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOccupyMaxDifferentRooms();

	ConstraintActivitiesOccupyMaxDifferentRooms(double wp, QList<int> a_L, int max_rooms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);

	void removeUseless(Rules& r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesSameRoomIfConsecutive: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesSameRoomIfConsecutive)

public:
	QList<int> activitiesIds;
	
	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesSameRoomIfConsecutive();

	ConstraintActivitiesSameRoomIfConsecutive(double wp, QList<int> a_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);

	void removeUseless(Rules& r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

#endif
