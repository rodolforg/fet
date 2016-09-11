/*
File rules.h
*/

/***************************************************************************
                          rules.h  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef RULES_H
#define RULES_H

#include "timetable_defs.h"
#include "timeconstraint.h"
#include "spaceconstraint.h"
#include "activity.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "room.h"
#include "building.h"

#include "matrix.h"

#include <QHash>
#include <QSet>
#include <QList>
#include <QStringList>
#include <QString>

#include <QCoreApplication>

class QXmlStreamReader;

class QWidget;

class FakeString
/*
fake string, so that the output log is not too large
*/
{
public:
	FakeString();

	void operator=(const QString& other);
	void operator=(const char* str);
	void operator+=(const QString& other);
	void operator+=(const char* str);
};

/**
This class contains all the information regarding
the institution: teachers, students, activities, constraints, etc.
*/
class Rules{
	Q_DECLARE_TR_FUNCTIONS(Rules)

public:
	bool modified;

	/**
	The name of the institution
	*/
	QString institutionName;
	
	/**
	The comments
	*/
	QString comments;

	/**
	The number of hours per day
	*/
	int nHoursPerDay;

	/**
	The number of days per week
	*/
	int nDaysPerWeek;

	/**
	The days of the week (names)
	*/
	QString daysOfTheWeek[MAX_DAYS_PER_WEEK];

	/**
	The hours of the day (names).
	*/
	QString hoursOfTheDay[MAX_HOURS_PER_DAY];

	/**
	The number of hours per week
	*/
	int nHoursPerWeek;

	/**
	The list of teachers
	*/
	TeachersList teachersList;

	/**
	The list of subjects
	*/
	SubjectsList subjectsList;

	/**
	The list of activity tags
	*/
	ActivityTagsList activityTagsList;

	/**
	The list of students (groups and subgroups included).
	Remember that every identifier (year, group or subgroup) must be UNIQUE.
	*/
	StudentsYearsList yearsList;

	/**
	The list of activities
	*/
	ActivitiesList activitiesList;

	/**
	The list of rooms
	*/
	RoomsList roomsList;

	/**
	The list of buildings
	*/
	BuildingsList buildingsList;

	/**
	The list of time constraints
	*/
	TimeConstraintsList timeConstraintsList;

	/**
	The list of space constraints
	*/
	SpaceConstraintsList spaceConstraintsList;
	
	GroupActivitiesInInitialOrderList groupActivitiesInInitialOrderList;
	
	//For faster operation
	//not internal, based on activity id / teacher name / students set name and constraints list
	QHash<int, Activity*> activitiesPointerHash; //first is id, second is pointer to Rules::activitiesList
	QSet<ConstraintBasicCompulsoryTime*> bctSet;
	QSet<ConstraintBreakTimes*> btSet;
	QSet<ConstraintBasicCompulsorySpace*> bcsSet;
	QHash<int, QSet<ConstraintActivityPreferredStartingTime*> > apstHash;
	QHash<int, QSet<ConstraintActivityPreferredRoom*> > aprHash;
	QHash<int, QSet<ConstraintMinDaysBetweenActivities*> > mdbaHash;
	QHash<QString, QSet<ConstraintTeacherNotAvailableTimes*> > tnatHash;
	QHash<QString, QSet<ConstraintStudentsSetNotAvailableTimes*> > ssnatHash;
	
	//not internal
	QHash<QString, StudentsSet*> permanentStudentsHash;
	
	//internal
	QHash<QString, int> teachersHash;
	QHash<QString, int> subjectsHash;
	QHash<QString, int> activityTagsHash;
	QHash<QString, StudentsSet*> studentsHash;
	QHash<QString, int> buildingsHash;
	QHash<QString, int> roomsHash;
	QHash<int, int> activitiesHash; //first is id, second is index in internal list
	//using activity index in internal activities
	/*QHash<QString, QSet<int> > activitiesForTeacherHash;
	QHash<QString, QSet<int> > activitiesForSubjectHash;
	QHash<QString, QSet<int> > activitiesForActivityTagHash;
	QHash<QString, QSet<int> > activitiesForStudentsSetHash;*/

	/*
	The following variables contain redundant data and are used internally
	*/
	////////////////////////////////////////////////////////////////////////
	int nInternalTeachers;
	Matrix1D<Teacher*> internalTeachersList;

	int nInternalSubjects;
	Matrix1D<Subject*> internalSubjectsList;

	int nInternalActivityTags;
	Matrix1D<ActivityTag*> internalActivityTagsList;

	int nInternalSubgroups;
	Matrix1D<StudentsSubgroup*> internalSubgroupsList;
	
	StudentsGroupsList internalGroupsList;
	
	StudentsYearsList augmentedYearsList;

	/**
	Here will be only the active activities.
	
	For speed, I used here not pointers, but static copies. (old comment.)
	*/
	int nInternalActivities;
	Matrix1D<Activity> internalActivitiesList;
	
	QSet<int> inactiveActivities;
	
	Matrix1D<QList<int> > activitiesForSubject;
	Matrix1D<QList<int> > activitiesForActivityTagList;
	Matrix1D<QSet<int> > activitiesForActivityTagSet;

	int nInternalRooms;
	Matrix1D<Room*> internalRoomsList;

	int nInternalBuildings;
	Matrix1D<Building*> internalBuildingsList;

	int nInternalTimeConstraints;
	Matrix1D<TimeConstraint*> internalTimeConstraintsList;

	int nInternalSpaceConstraints;
	Matrix1D<SpaceConstraint*> internalSpaceConstraintsList;

	/*
	///////////////////////////////////////////////////////////////////////
	*/

	/**
	True if the rules have been initialized in some way (new or loaded).
	*/
	bool initialized;

	/**
	True if the internal structure was computed.
	*/
	bool internalStructureComputed;

	/**
	Initializes the rules (empty)
	*/
	void init();

	/**
	Internal structure initializer.
	<p>
	After any modification of the activities or students or teachers
	or constraints, you need to call this subroutine
	*/
	bool computeInternalStructure(QWidget* parent);

	/**
	Terminator - basically clears the memory for the constraints.
	*/
	void kill();

	Rules();

	~Rules();
	
	void setInstitutionName(const QString& newInstitutionName);
	
	void setComments(const QString& newComments);

	/**
	Adds a new teacher
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addTeacher(Teacher* teacher);

	/*when reading rules, faster*/
	bool addTeacherFast(Teacher* teacher);

	/**
	Returns the index of this teacher in the teachersList,
	or -1 for inexistent teacher.
	*/
	int searchTeacher(const QString& teacherName);

	/**
	Removes this teacher and all related activities and constraints.
	It returns false on failure. If successful, returns true.
	*/
	bool removeTeacher(const QString& teacherName);

	/**
	Modifies (renames) this teacher and takes care of all related activities and constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyTeacher(const QString& initialTeacherName, const QString& finalTeacherName);

	/**
	A function to sort the teachers alphabetically
	*/
	void sortTeachersAlphabetically();

	/**
	Adds a new subject
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addSubject(Subject* subject);

	/*
	When reading rules, faster
	*/
	bool addSubjectFast(Subject* subject);

	/**
	Returns the index of this subject in the subjectsList,
	or -1 if not found.
	*/
	int searchSubject(const QString& subjectName);

	/**
	Removes this subject and all related activities and constraints.
	It returns false on failure.
	If successful, returns true.
	*/
	bool removeSubject(const QString& subjectName);

	/**
	Modifies (renames) this subject and takes care of all related activities and constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifySubject(const QString& initialSubjectName, const QString& finalSubjectName);

	/**
	A function to sort the subjects alphabetically
	*/
	void sortSubjectsAlphabetically();

	/**
	Adds a new activity tag to the list of activity tags
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addActivityTag(ActivityTag* activityTag);

	/*
	When reading rules, faster
	*/
	bool addActivityTagFast(ActivityTag* activityTag);

	/**
	Returns the index of this activity tag in the activityTagsList,
	or -1 if not found.
	*/
	int searchActivityTag(const QString& activityTagName);

	/**
	Removes this activity tag. In the list of activities, the activity tag will 
	be removed from all activities which posess it.
	It returns false on failure.
	If successful, returns true.
	*/
	bool removeActivityTag(const QString& activityTagName);

	/**
	Modifies (renames) this activity tag and takes care of all related activities.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyActivityTag(const QString& initialActivityTagName, const QString& finalActivityTagName);

	/**
	A function to sort the activity tags alphabetically
	*/
	void sortActivityTagsAlphabetically();
	
	void computePermanentStudentsHash();

	/**
	Returns a pointer to the structure containing this student set
	(year, group or subgroup) or NULL.
	*/
	StudentsSet* searchStudentsSet(const QString& setName);
	
	StudentsSet* searchAugmentedStudentsSet(const QString& setName);
	
	/**
	True if the students sets contain one common subgroup.
	This function is used in constraints isRelatedToStudentsSet
	*/
	bool setsShareStudents(const QString& studentsSet1, const QString& studentsSet2);

	//Internal
	bool augmentedSetsShareStudentsFaster(const QString& studentsSet1, const QString& studentsSet2);

	/**
	Adds a new year of study to the academic structure
	*/
	bool addYear(StudentsYear* year);
	
	/*
	When reading rules, faster
	*/
	bool addYearFast(StudentsYear* year);

//	bool emptyYear(const QString& yearName);
	bool removeYear(const QString& yearName);
//	bool removeYear(const QString& yearName, bool removeAlsoThisYear);
	
	bool removeYearPointerAfterSplit(StudentsYear* yearPointer);

	/**
	Returns -1 if not found or the index of this year in the years list
	*/
	int searchYear(const QString& yearName);

	int searchAugmentedYear(const QString& yearName);

	/**
	Modifies this students set (name, number of students) and takes care of all related
	activities and constraints. Returns true on success, false on failure (if not found)
	*/
	bool modifyStudentsSet(const QString& initialStudentsSetName, const QString& finalStudentsSetName, int finalNumberOfStudents);
	
	bool modifyStudentsSets(const QHash<QString, QString>& oldAndNewStudentsSetNames);
	
	/**
	A function to sort the years alphabetically
	*/
	void sortYearsAlphabetically();

	/**
	Adds a new group in a certain year of study to the academic structure
	*/
	bool addGroup(const QString& yearName, StudentsGroup* group);
	
	/*
	When reading rules, faster
	*/
	bool addGroupFast(StudentsYear* year, StudentsGroup* group);

	bool removeGroup(const QString& yearName, const QString& groupName);

	//Remove this group from all the years in which it exists
	bool purgeGroup(const QString& groupName);

	/**
	Returns -1 if not found or the index of this group in the groups list
	of this year.
	*/
	int searchGroup(const QString& yearName, const QString& groupName);

	int searchAugmentedGroup(const QString& yearName, const QString& groupName);

	/**
	A function to sort the groups of this year alphabetically
	*/
	void sortGroupsAlphabetically(const QString& yearName);

	/**
	Adds a new subgroup to a certain group in a certain year of study to
	the academic structure
	*/
	bool addSubgroup(const QString& yearName, const QString& groupName, StudentsSubgroup* subgroup);

	/*
	When reading rules, faster
	*/
	bool addSubgroupFast(StudentsYear* year, StudentsGroup* group, StudentsSubgroup* subgroup);

	bool removeSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	//Remove this subgroup from all the groups in which it exists
	bool purgeSubgroup(const QString& subgroupName);

	/**
	Returns -1 if not found or the index of the subgroup in the list of subgroups of this group
	*/
	int searchSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	int searchAugmentedSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	/**
	A function to sort the subgroups of this group alphabetically
	*/
	void sortSubgroupsAlphabetically(const QString& yearName, const QString& groupName);
	
	/**
	Adds a new indivisible activity (not split) to the list of activities.
	(It can add a subactivity of a split activity)
	Returns true if successful or false if the maximum
	number of activities was reached.
	*/
	/*bool addSimpleActivity(
		QWidget* parent,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents);*/

	/*
	Faster (no need to recompute the number of students in activity constructor)
	*/
	bool addSimpleActivityFast(
		QWidget* parent,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		int _computedNumberOfStudents);

	/**
	Adds a new split activity to the list of activities.
	Returns true if successful or false if the maximum
	number of activities was reached.
	If _minDayDistance>0, there will automatically added a compulsory
	ConstraintMinDaysBetweenActivities.
	*/
	/*bool addSplitActivity(
		QWidget* parent,
		int _firstActivityId,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _nSplits,
		int _totalDuration,
		int _durations[],
		bool _active[],
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		bool _computeNTotalStudents,
		int _nTotalStudents);*/

	bool addSplitActivityFast(
		QWidget* parent,
		int _firstActivityId,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _nSplits,
		int _totalDuration,
		int _durations[],
		bool _active[],
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		int _computedNumberOfStudents);

	/**
	Removes only the activity with this id.
	*/
	//void removeActivity(int _id);

	/**
	If _activityGroupId==0, then this is a non-split activity
	(if >0, then this is a single sub-activity from a split activity.
	Removes this activity from the list of activities.
	For split activities, it removes all the sub-activities that are contained in it.
	*/
	void removeActivity(int _id, int _activityGroupId);
	
	void removeActivities(const QList<int>& _idsList, bool updateConstraints);
	
	/**
	A function to modify the information of a certain activity.
	If this is a sub-activity of a split activity,
	all the sub-activities will be modified.
	*/
	void modifyActivity(
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
	 	int _nSplits,
		int _totalDuration,
		int _durations[],
		bool _active[],
		bool _computeNTotalStudents,
		int nTotalStudents);

	void modifySubactivity(
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		bool _active,
		bool _computeNTotalStudents,
		int nTotalStudents);

	/**
	Adds a new room (already allocated).
	Returns true on success, false for already existing rooms (same name).
	*/
	bool addRoom(Room* rm);

	/*
	Faster, when reading
	*/
	bool addRoomFast(Room* rm);

	/**
	Returns -1 if not found or the index in the rooms list if found.
	*/
	int searchRoom(const QString& roomName);

	/**
	Removes the room with this name.
	Returns true on success, false on failure (not found).
	*/
	bool removeRoom(const QString& roomName);
	
	/**
	Modifies this room and takes care of all related constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyRoom(const QString& initialRoomName, const QString& finalRoomName, const QString& building, int capacity);

	/**
	A function to sort the room alphabetically, by name
	*/
	void sortRoomsAlphabetically();

	/**
	Adds a new building (already allocated).
	Returns true on success, false for already existing buildings (same name).
	*/
	bool addBuilding(Building* rm);

	/*
	Faster, when reading
	*/
	bool addBuildingFast(Building* rm);

	/**
	Returns -1 if not found or the index in the buildings list if found.
	*/
	int searchBuilding(const QString& buildingName);

	/**
	Removes the building with this name.
	Returns true on success, false on failure (not found).
	*/
	bool removeBuilding(const QString& buildingName);
	
	/**
	Modifies this building and takes care of all related constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyBuilding(const QString& initialBuildingName, const QString& finalBuildingName);

	/**
	A function to sort the buildings alphabetically, by name
	*/
	void sortBuildingsAlphabetically();

	/**
	Adds a new time constraint (already allocated).
	Returns true on success, false for already existing constraints.
	*/
	bool addTimeConstraint(TimeConstraint* ctr);

	/**
	Removes this time constraint.
	Returns true on success, false on failure (not found).
	*/
	bool removeTimeConstraint(TimeConstraint* ctr);

	/**
	Adds a new space constraint (already allocated).
	Returns true on success, false for already existing constraints.
	*/
	bool addSpaceConstraint(SpaceConstraint* ctr);
	
	/**
	Removes this space constraint.
	Returns true on success, false on failure (not found).
	*/
	bool removeSpaceConstraint(SpaceConstraint* ctr);
	
	bool removeTimeConstraints(QList<TimeConstraint*> _tcl);
	bool removeSpaceConstraints(QList<SpaceConstraint*> _scl);

	int xmlReaderNumberOfUnrecognizedFields;

	/**
	Reads the rules from the xml input file "filename".
	Returns true on success, false on failure (inexistent file or wrong format)
	*/
	bool read(QWidget* parent, const QString& fileName, bool commandLine=false, QString commandLineDirectory=QString());

	/**
	Write the rules to the xml input file "inputfile".
	*/
	bool write(QWidget* parent, const QString& filename);
	
	int activateTeacher(const QString& teacherName);
	
	int activateStudents(const QString& studentsName);
	
	int activateSubject(const QString& subjectName);
	
	int activateActivityTag(const QString& activityTagName);

	int deactivateTeacher(const QString& teacherName);
	
	int deactivateStudents(const QString& studentsName);
	
	int deactivateSubject(const QString& subjectName);
	
	int deactivateActivityTag(const QString& activityTagName);
	
	void updateActivitiesWhenRemovingStudents(const QSet<StudentsSet*>& studentsSets, bool updateConstraints);
	void updateGroupActivitiesInInitialOrderAfterRemoval();
	void updateConstraintsAfterRemoval();
	
private:
	TimeConstraint* readBasicCompulsoryTime(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinNDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMaxDaysBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinGapsBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesNotOverlapping(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingTime(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivityPreferredTime(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime);
	TimeConstraint* readActivityPreferredStartingTime(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime);

	TimeConstraint* readActivityEndsStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesEndStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	/*old, with 2 and 3*/
	TimeConstraint* read2ActivitiesConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read2ActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read3ActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read2ActivitiesOrdered(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	/*end old*/
	
	TimeConstraint* readTwoActivitiesConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readThreeActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoActivitiesOrdered(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readActivityPreferredTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivityPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivityPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readBreak(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readBreakTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readTeachersNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readStudentsNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsEarly(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetEarly(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesPreferredTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readSubactivitiesPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readSubactivitiesPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesOccupyMaxTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesMaxSimultaneousInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readBasicCompulsorySpace(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivityPreferredRoom(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedSpace);
	SpaceConstraint* readActivityPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectSubjectTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectSubjectTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectActivityTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectActivityTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivityTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivityTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readStudentsSetHomeRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetHomeRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherHomeRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherHomeRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readTeacherMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readStudentsSetMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readActivitiesOccupyMaxDifferentRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivitiesSameRoomIfConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);
};

#endif
