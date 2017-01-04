/*
File generate.h
*/

/***************************************************************************
                          generate.h  -  description
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

#ifndef GENERATE_H
#define GENERATE_H

#include "timetable.h"
#include "timetable_defs.h"
#include "solution.h"

#include <QTextStream>
#include <QDateTime>
#include <QSemaphore>

class Activity;

class QWidget;

/**
This class incorporates the routines for time and space allocation of activities
*/
class Generate: public QObject{
	Q_OBJECT

public:
	Generate(const Timetable & gt);
	~Generate();
private:
	inline void addAiToNewTimetable(int ai, const Activity* act, int d, int h);
	inline void removeAiFromNewTimetable(int ai, const Activity* act, int d, int h);
	
	inline void getTchTimetable(int tch, const QList<int>& conflActivities);
	inline void getSbgTimetable(int sbg, const QList<int>& conflActivities);
	
	inline void removeAi2FromTchTimetable(int ai2);
	inline void removeAi2FromSbgTimetable(int ai2);

	inline void updateTeachersNHoursGaps(Activity* act, int ai, int d);
	inline void updateSubgroupsNHoursGaps(Activity* act, int ai, int d);
	
	inline void updateTchNHoursGaps(int tch, int d);
	inline void updateSbgNHoursGaps(int sbg, int d);
	
	inline void tchGetNHoursGaps(int tch);
	inline void teacherGetNHoursGaps(int tch);
	inline bool teacherRemoveAnActivityFromBeginOrEnd(int tch, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhere(int tch, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromBeginOrEndCertainDay(int tch, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhereCertainDay(int tch, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int tch, int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline void sbgGetNHoursGaps(int sbg);
	inline void subgroupGetNHoursGaps(int sbg);
	inline bool subgroupRemoveAnActivityFromBegin(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEnd(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginOrEnd(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhere(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhereCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int sbg, int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	
	inline bool checkActivitiesOccupyMaxDifferentRooms(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list);
	inline bool checkActivitiesSameRoomIfConsecutive(const QList<int>& globalConflActivities, int rm, int level, int ai, int d, int h, QList<int>& tmp_list);

	//only one out of sbg and tch is >=0, other one is -1
	inline bool checkBuildingChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);
	inline bool chooseRoom(const QList<int>& listOfRooms, const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities);
	inline bool getHomeRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities);
	inline bool getPreferredRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, bool& canBeUnspecifiedPreferredRoom);
	inline bool getRoom(int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& conflActivities, int& nConflActivities);
public:
	Solution c;
	
	int nPlacedActivities;
	
	//difficult activities
	int nDifficultActivities;
	int difficultActivities[MAX_ACTIVITIES];

	int searchTime; //seconds
	
	int timeToHighestStage; //seconds

	bool abortOptimization;

	bool precompute(QWidget* parent, QTextStream* maxPlacedActivityStream=NULL);

	void generate(int maxSeconds, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream=NULL);

	int getMaxActivitiesPlaced() const;

	Solution& getHighestStageSolution();

	QSemaphore semaphorePlacedActivity;
	QSemaphore finishedSemaphore;

private:
	void moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom);
	
	void randomSwap(int ai, int level);

signals:
	void activityPlaced(int);
	
	void simulationFinished();
	
	void impossibleToSolve();
	
private:
	const Timetable &gt;
	int maxActivitiesPlaced;
	Solution highestStageSolution;

	QDateTime generationStartDateTime;
	QDateTime generationHighestStageDateTime;

	bool isThreaded;

	bool swappedActivities[MAX_ACTIVITIES];

	bool foundGoodSwap;

	//not sure, it might be necessary 2*... or even more
	int restoreActIndex[4*MAX_ACTIVITIES]; //the index of the act. to restore
	int restoreTime[4*MAX_ACTIVITIES]; //the time when to restore
	int restoreRoom[4*MAX_ACTIVITIES]; //the time when to restore
	int nRestore;

	int limitcallsrandomswap;


	int level_limit;

	int ncallsrandomswap;
	int maxncallsrandomswap;
	//if level==0, choose best position with lowest number
	//of conflicting activities
	QList<int> conflActivitiesTimeSlot;
	int timeSlot;
	int roomSlot;


	//int triedRemovals[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];
	Matrix2D<int> triedRemovals;

	bool impossibleActivity;

	int invPermutation[MAX_ACTIVITIES];

	////////tabu list of tried removals (circular)
	//const int MAX_TABU=MAX_ACTIVITIES*MAX_HOURS_PER_WEEK;
	int tabu_size;
	int crt_tabu_index;
	/*qint16 tabu_activities[MAX_TABU];
	qint16 tabu_times[MAX_TABU];*/
	Matrix1D<int> tabu_activities;
	Matrix1D<int> tabu_times;
	////////////

	/*qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 roomsTimetable[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
	Matrix3D<int> teachersTimetable;
	Matrix3D<int> subgroupsTimetable;
	Matrix3D<int> roomsTimetable;


	/*qint16 newTeachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 newSubgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 newTeachersDayNHours[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
	qint16 newTeachersDayNGaps[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
	qint16 newSubgroupsDayNHours[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
	qint16 newSubgroupsDayNGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
	qint16 newSubgroupsDayNFirstGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];*/
	Matrix3D<int> newTeachersTimetable;
	Matrix3D<int> newSubgroupsTimetable;
	Matrix2D<int> newTeachersDayNHours;
	Matrix2D<int> newTeachersDayNGaps;
	Matrix2D<int> newSubgroupsDayNHours;
	Matrix2D<int> newSubgroupsDayNGaps;
	Matrix2D<int> newSubgroupsDayNFirstGaps;


	/*qint16 oldTeachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 oldSubgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 oldTeachersDayNHours[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
	qint16 oldTeachersDayNGaps[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
	qint16 oldSubgroupsDayNHours[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
	qint16 oldSubgroupsDayNGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
	qint16 oldSubgroupsDayNFirstGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];*/
	Matrix3D<int> oldTeachersTimetable;
	Matrix3D<int> oldSubgroupsTimetable;
	Matrix2D<int> oldTeachersDayNHours;
	Matrix2D<int> oldTeachersDayNGaps;
	Matrix2D<int> oldSubgroupsDayNHours;
	Matrix2D<int> oldSubgroupsDayNGaps;
	Matrix2D<int> oldSubgroupsDayNFirstGaps;


	/*qint16 tchTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 tchDayNHours[MAX_DAYS_PER_WEEK];
	qint16 tchDayNGaps[MAX_DAYS_PER_WEEK];

	qint16 sbgTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	qint16 sbgDayNHours[MAX_DAYS_PER_WEEK];
	qint16 sbgDayNGaps[MAX_DAYS_PER_WEEK];
	qint16 sbgDayNFirstGaps[MAX_DAYS_PER_WEEK];*/
	Matrix2D<int> tchTimetable;
	Matrix1D<int> tchDayNHours;
	Matrix1D<int> tchDayNGaps;

	Matrix2D<int> sbgTimetable;
	Matrix1D<int> sbgDayNHours;
	Matrix1D<int> sbgDayNGaps;
	Matrix1D<int> sbgDayNFirstGaps;

	//QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
	Matrix2D<QList<int> > teacherActivitiesOfTheDay;
	Matrix2D<QList<int> > subgroupActivitiesOfTheDay;

	//used at level 0
	Matrix1D<int> l0nWrong;
	Matrix1D<int> l0minWrong;
	Matrix1D<int> l0minIndexAct;

	//2011-09-25
	Matrix1D<QSet<int> > slotSetOfActivities;
	Matrix1D<bool> slotCanEmpty;

	Matrix1D<QSet<int> > activitiesAtTime;

	static const int MAX_LEVEL=31;

	static const int LEVEL_STOP_CONFLICTS_CALCULATION=MAX_LEVEL;

	static const int INF=2000000000;

	static const int MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0=400000;

	//for sorting slots in ascending order of potential conflicts
	struct cmp {
		cmp(const Generate *g);
		bool operator()(int a, int b);
	private:
		const double (*nMinDaysBrokenL)[MAX_HOURS_PER_WEEK];
		const int (*nConflActivitiesL)[MAX_HOURS_PER_WEEK];
	};

	int currentLevel;
	double nMinDaysBrokenL[Generate::MAX_LEVEL][MAX_HOURS_PER_WEEK];
	//int conflPermL[MAX_LEVEL][MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
	int nConflActivitiesL[Generate::MAX_LEVEL][MAX_HOURS_PER_WEEK];
	int selectedRoomL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
	int permL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
	QList<int> conflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
	int roomSlotsL[Generate::MAX_LEVEL][MAX_HOURS_PER_WEEK];
};

#endif
