/*
File generate.cpp
*/

/***************************************************************************
                          generate.cpp  -  description
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

#include <ctime>

#include <QtAlgorithms>

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
using namespace std;

#include "timetable_defs.h"
#include "timetable.h"
#include "generate.h"
#include "rules.h"

#include "generate_pre.h"

#include "matrix.h"

#include <QMutex>

#include <QDateTime>

#include <QList>
#include <QSet>
#include <QHash>

#include <QSemaphore>

extern QMutex myMutex; //timetablegenerateform.cpp

#ifndef FET_COMMAND_LINE
extern QSemaphore semaphorePlacedActivity;
extern QSemaphore finishedSemaphore;
#else
QSemaphore semaphorePlacedActivity;
QSemaphore finishedSemaphore;
#endif

extern Timetable gt;

static bool swappedActivities[MAX_ACTIVITIES];

static bool foundGoodSwap;

//not sure, it might be necessary 2*... or even more
static int restoreActIndex[4*MAX_ACTIVITIES]; //the index of the act. to restore
static int restoreTime[4*MAX_ACTIVITIES]; //the time when to restore
static int restoreRoom[4*MAX_ACTIVITIES]; //the time when to restore
static int nRestore;

static int limitcallsrandomswap;

const int MAX_LEVEL=31;

const int LEVEL_STOP_CONFLICTS_CALCULATION=MAX_LEVEL;

static int level_limit;

static int ncallsrandomswap;
static int maxncallsrandomswap;

Solution highestStageSolution;


//if level==0, choose best position with lowest number
//of conflicting activities
static QList<int> conflActivitiesTimeSlot;
static int timeSlot;
static int roomSlot;


//int triedRemovals[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];
static Matrix2D<int> triedRemovals;

static bool impossibleActivity;

static int invPermutation[MAX_ACTIVITIES];

const int INF=2000000000;


////////tabu list of tried removals (circular)
//const int MAX_TABU=MAX_ACTIVITIES*MAX_HOURS_PER_WEEK;
static int tabu_size;
static int crt_tabu_index;
/*qint16 tabu_activities[MAX_TABU];
qint16 tabu_times[MAX_TABU];*/
static Matrix1D<int> tabu_activities;
static Matrix1D<int> tabu_times;
////////////

/*static qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 roomsTimetable[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
static Matrix3D<int> teachersTimetable;
static Matrix3D<int> subgroupsTimetable;
static Matrix3D<int> roomsTimetable;


/*static qint16 newTeachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 newSubgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 newTeachersDayNHours[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
static qint16 newTeachersDayNGaps[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
static qint16 newSubgroupsDayNHours[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
static qint16 newSubgroupsDayNGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
static qint16 newSubgroupsDayNFirstGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];*/
static Matrix3D<int> newTeachersTimetable;
static Matrix3D<int> newSubgroupsTimetable;
static Matrix2D<int> newTeachersDayNHours;
static Matrix2D<int> newTeachersDayNGaps;
static Matrix2D<int> newSubgroupsDayNHours;
static Matrix2D<int> newSubgroupsDayNGaps;
static Matrix2D<int> newSubgroupsDayNFirstGaps;


/*static qint16 oldTeachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 oldSubgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 oldTeachersDayNHours[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
static qint16 oldTeachersDayNGaps[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
static qint16 oldSubgroupsDayNHours[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
static qint16 oldSubgroupsDayNGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
static qint16 oldSubgroupsDayNFirstGaps[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];*/
static Matrix3D<int> oldTeachersTimetable;
static Matrix3D<int> oldSubgroupsTimetable;
static Matrix2D<int> oldTeachersDayNHours;
static Matrix2D<int> oldTeachersDayNGaps;
static Matrix2D<int> oldSubgroupsDayNHours;
static Matrix2D<int> oldSubgroupsDayNGaps;
static Matrix2D<int> oldSubgroupsDayNFirstGaps;


/*static qint16 tchTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 tchDayNHours[MAX_DAYS_PER_WEEK];
static qint16 tchDayNGaps[MAX_DAYS_PER_WEEK];

static qint16 sbgTimetable[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 sbgDayNHours[MAX_DAYS_PER_WEEK];
static qint16 sbgDayNGaps[MAX_DAYS_PER_WEEK];
static qint16 sbgDayNFirstGaps[MAX_DAYS_PER_WEEK];*/
static Matrix2D<int> tchTimetable;
static Matrix1D<int> tchDayNHours;
static Matrix1D<int> tchDayNGaps;

static Matrix2D<int> sbgTimetable;
static Matrix1D<int> sbgDayNHours;
static Matrix1D<int> sbgDayNGaps;
static Matrix1D<int> sbgDayNFirstGaps;

//static QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
static Matrix2D<QList<int> > teacherActivitiesOfTheDay;
static Matrix2D<QList<int> > subgroupActivitiesOfTheDay;

int maxActivitiesPlaced;

QDateTime generationStartDateTime;
QDateTime generationHighestStageDateTime;

const int MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0=400000;

//used at level 0
static Matrix1D<int> l0nWrong;
static Matrix1D<int> l0minWrong;
static Matrix1D<int> l0minIndexAct;

//2011-09-25
static Matrix1D<QSet<int> > slotSetOfActivities;
static Matrix1D<bool> slotCanEmpty;

static Matrix1D<QSet<int> > activitiesAtTime;
////////////

/*inline int max(qint16 a, int b){
	if(int(a)>=b)
		return int(a);
	else
		return b;
}*/

inline void Generate::addAiToNewTimetable(int ai, const Activity* act, int d, int h)
{
	//foreach(int tch, act->iTeachersList){
	foreach(int tch, mustComputeTimetableTeachers[ai]){
		for(int dur=0; dur<act->duration; dur++){
			oldTeachersTimetable(tch,d,h+dur)=newTeachersTimetable(tch,d,h+dur);
			newTeachersTimetable(tch,d,h+dur)=ai;
		}
		oldTeachersDayNHours(tch,d)=newTeachersDayNHours(tch,d);
		oldTeachersDayNGaps(tch,d)=newTeachersDayNGaps(tch,d);
	}

	//foreach(int sbg, act->iSubgroupsList){
	foreach(int sbg, mustComputeTimetableSubgroups[ai]){
		for(int dur=0; dur<act->duration; dur++){
			oldSubgroupsTimetable(sbg,d,h+dur)=newSubgroupsTimetable(sbg,d,h+dur);
			newSubgroupsTimetable(sbg,d,h+dur)=ai;
		}
		oldSubgroupsDayNHours(sbg,d)=newSubgroupsDayNHours(sbg,d);
		oldSubgroupsDayNGaps(sbg,d)=newSubgroupsDayNGaps(sbg,d);
		oldSubgroupsDayNFirstGaps(sbg,d)=newSubgroupsDayNFirstGaps(sbg,d);
	}
}

inline void Generate::removeAiFromNewTimetable(int ai, const Activity* act, int d, int h)
{
	foreach(int tch, mustComputeTimetableTeachers[ai]){
	//foreach(int tch, act->iTeachersList){
		for(int dur=0; dur<act->duration; dur++){
			assert(newTeachersTimetable(tch,d,h+dur)==ai);
			newTeachersTimetable(tch,d,h+dur)=oldTeachersTimetable(tch,d,h+dur);
		}
		newTeachersDayNHours(tch,d)=oldTeachersDayNHours(tch,d);
		newTeachersDayNGaps(tch,d)=oldTeachersDayNGaps(tch,d);
	}

	foreach(int sbg, mustComputeTimetableSubgroups[ai]){
	//foreach(int sbg, act->iSubgroupsList){
		for(int dur=0; dur<act->duration; dur++){
			assert(newSubgroupsTimetable(sbg,d,h+dur)==ai);
			newSubgroupsTimetable(sbg,d,h+dur)=oldSubgroupsTimetable(sbg,d,h+dur);
		}
		newSubgroupsDayNHours(sbg,d)=oldSubgroupsDayNHours(sbg,d);
		newSubgroupsDayNGaps(sbg,d)=oldSubgroupsDayNGaps(sbg,d);
		newSubgroupsDayNFirstGaps(sbg,d)=oldSubgroupsDayNFirstGaps(sbg,d);
	}
}


inline void Generate::removeAi2FromTchTimetable(int ai2)
{
	Activity* act2=&gt.rules.internalActivitiesList[ai2];
	int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
	int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
	
	for(int dur2=0; dur2<act2->duration; dur2++){
		assert(tchTimetable(d2,h2+dur2)==ai2);
		if(tchTimetable(d2,h2+dur2)==ai2)
			tchTimetable(d2,h2+dur2)=-1;
	}
}

inline void Generate::removeAi2FromSbgTimetable(int ai2)
{
	Activity* act2=&gt.rules.internalActivitiesList[ai2];
	int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
	int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
	
	for(int dur2=0; dur2<act2->duration; dur2++){
		assert(sbgTimetable(d2,h2+dur2)==ai2);
		if(sbgTimetable(d2,h2+dur2)==ai2)
			sbgTimetable(d2,h2+dur2)=-1;
	}
}

inline void Generate::getTchTimetable(int tch, const QList<int>& conflActivities)
{
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2=newTeachersTimetable(tch,d2,h2);
			if(ai2>=0 && !conflActivities.contains(ai2))
				tchTimetable(d2,h2)=ai2;
			else
				tchTimetable(d2,h2)=-1;
		}
		
	/*for(int dur=0; dur<act->duration; dur++){
		assert(tchTimetable(d,h+dur)==-1);
		tchTimetable(d,h+dur)=ai;
	}*/
}

inline void Generate::getSbgTimetable(int sbg, const QList<int>& conflActivities)
{
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2=newSubgroupsTimetable(sbg,d2,h2);
			if(ai2>=0 && !conflActivities.contains(ai2))
				sbgTimetable(d2,h2)=ai2;
			else
				sbgTimetable(d2,h2)=-1;
		}
		
	/*for(int dur=0; dur<act->duration; dur++){
		assert(sbgTimetable(d,h+dur)==-1);
		sbgTimetable(d,h+dur)=ai;
	}*/
}

inline void Generate::updateTchNHoursGaps(int tch, int d)
{
	int hours=0, gaps=0;
	
	int h;
	for(h=0; h<gt.rules.nHoursPerDay; h++)
		if(tchTimetable(d,h)>=0)
			break;
	int ng=0;
	for(; h<gt.rules.nHoursPerDay; h++){
		if(tchTimetable(d,h)>=0){
			hours++;
			gaps+=ng;
			ng=0;
		}
		else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
			ng++;
	}
	tchDayNGaps[d]=gaps;
	tchDayNHours[d]=hours;
}

inline void Generate::updateSbgNHoursGaps(int sbg, int d)
{
	int hours=0, gaps=0, nfirstgaps=0;

	int h;
	for(h=0; h<gt.rules.nHoursPerDay; h++){
		if(sbgTimetable(d,h)>=0)
			break;
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
			nfirstgaps++;
	}
	int ng=0;
	for(; h<gt.rules.nHoursPerDay; h++){
		if(sbgTimetable(d,h)>=0){
			hours++;
			gaps+=ng;
			ng=0;
		}
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
			ng++;
	}
	sbgDayNGaps[d]=gaps;
	sbgDayNHours[d]=hours;
	if(sbgDayNHours[d]>0)
		sbgDayNFirstGaps[d]=nfirstgaps;
	else
		sbgDayNFirstGaps[d]=0;
}

inline void Generate::updateTeachersNHoursGaps(Activity* act, int ai, int d)
{
	Q_UNUSED(act);

	foreach(int tch, mustComputeTimetableTeachers[ai]){
	//foreach(int tch, act->iTeachersList){
		int hours=0, gaps=0;
	
		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++)
			if(newTeachersTimetable(tch,d,h)>=0)
				break;
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(newTeachersTimetable(tch,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
				ng++;
		}
		newTeachersDayNGaps(tch,d)=gaps;
		newTeachersDayNHours(tch,d)=hours;
	}
}

inline void Generate::updateSubgroupsNHoursGaps(Activity* act, int ai, int d)
{
	Q_UNUSED(act);

	foreach(int sbg, mustComputeTimetableSubgroups[ai]){
	//foreach(int sbg, act->iSubgroupsList){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
		newSubgroupsDayNGaps(sbg,d)=gaps;
		newSubgroupsDayNHours(sbg,d)=hours;
		if(hours>0)
			newSubgroupsDayNFirstGaps(sbg,d)=nfirstgaps;
		else
			newSubgroupsDayNFirstGaps(sbg,d)=0;
	}
}


inline void Generate::teacherGetNHoursGaps(int tch)
{
	if(!mustComputeTimetableTeacher[tch])
		return;

	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		newTeachersDayNHours(tch,d2)=0;
		newTeachersDayNGaps(tch,d2)=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(newTeachersTimetable(tch,d2,h2)>=0){
				newTeachersDayNHours(tch,d2)++;
				if(countGaps)
					newTeachersDayNGaps(tch,d2)+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
				ng++;
		}
	}
}

inline void Generate::tchGetNHoursGaps(int tch)
{
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		tchDayNHours[d2]=0;
		tchDayNGaps[d2]=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2,h2)>=0){
				tchDayNHours[d2]++;
				if(countGaps)
					tchDayNGaps[d2]+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
				ng++;
		}
	}
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEnd(int tch, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(tch);

	//remove an activity from begin or from end of any day
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndexBegin=-1, actIndexEnd=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2,h2)>=0){
					actIndexBegin=tchTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(tchTimetable(d2,h2)>=0){
					actIndexEnd=tchTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
					actIndexEnd=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}
						
	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;
				
	if(possibleBeginOrEnd){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(possibleDays.count());
		}
								
		assert(t>=0 && t<possibleDays.count());
							
		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);
			
			assert(tchTimetable(d2,h2)==ai2);
								
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);
			
			assert(tchTimetable(d2,h2)==ai2);
							
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEndCertainDay(int tch, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(tch);

	int actIndexBegin=-1, actIndexEnd=-1;
	
	if(tchDayNHours[d2]>0){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2,h2)>=0){
				actIndexBegin=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;
		for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d2,h2)>=0){
				actIndexEnd=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;
	}
						
	if(actIndexEnd>=0 || actIndexBegin>=0){
		int ai2=-1;
		if(level==0){
			int optMinWrong=INF;

			if(actIndexBegin>=0){
				if(optMinWrong>triedRemovals(actIndexBegin,c.times[actIndexBegin])){
				 	optMinWrong=triedRemovals(actIndexBegin,c.times[actIndexBegin]);
				}
				ai2=actIndexBegin;
			}

			if(actIndexEnd>=0){
				if(optMinWrong>triedRemovals(actIndexEnd,c.times[actIndexEnd])){
				 	optMinWrong=triedRemovals(actIndexEnd,c.times[actIndexEnd]);
				}
				ai2=actIndexEnd;
			}
			
			if(actIndexBegin>=0 && actIndexEnd>=0 && optMinWrong==triedRemovals(actIndexEnd,c.times[actIndexEnd]) &&
			  optMinWrong==triedRemovals(actIndexBegin,c.times[actIndexBegin])){
				if(randomKnuth(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		else{
			if(actIndexBegin>=0 && actIndexEnd<0)
				ai2=actIndexBegin;
			else if(actIndexEnd>=0 && actIndexBegin<0)
				ai2=actIndexEnd;
			else{
				if(randomKnuth(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		assert(ai2>=0);
								
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhere(int tch, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(tch);

	//remove an activity from anywhere
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0){
					actIndex=tchTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainDay(int tch, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(tch);

	//remove an activity from anywhere
	QList<int> acts;
	//for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0){
					actIndex=tchTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	//}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int tch, int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(tch);

	//remove an activity from anywhere
	QList<int> acts;
	//for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0){
					actIndex=tchTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	//}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}


//students
inline void Generate::subgroupGetNHoursGaps(int sbg)
{
	if(!mustComputeTimetableSubgroup[sbg])
		return;

	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
		newSubgroupsDayNGaps(sbg,d)=gaps;
		newSubgroupsDayNHours(sbg,d)=hours;
		if(hours>0)
			newSubgroupsDayNFirstGaps(sbg,d)=nfirstgaps;
		else
			newSubgroupsDayNFirstGaps(sbg,d)=0;
	}

/*	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		newSubgroupsDayNHours(sbg,d2)=0;
		newSubgroupsDayNGaps(sbg,d2)=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(newSubgroupsTimetable(sbg,d2,h2)>=0){
				newSubgroupsDayNHours(sbg,d2)++;
				if(countGaps)
					newSubgroupsDayNGaps(sbg,d2)+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !subgroupNotAvailableDayHour(sbg,d2,h2))
				ng++;
		}
	}*/
}

inline void Generate::sbgGetNHoursGaps(int sbg)
{
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(sbgTimetable(d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(sbgTimetable(d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
	
		sbgDayNGaps[d]=gaps;
		sbgDayNHours[d]=hours;
		if(sbgDayNHours[d]>0)
			sbgDayNFirstGaps[d]=nfirstgaps;
		else
			sbgDayNFirstGaps[d]=0;
	}
	
	/*
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		sbgDayNHours[d2]=0;
		sbgDayNGaps[d2]=0;
		sbgDayNFirstGaps[d2]=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
		if(sbgTimetable(d2,h2)>=0){
				sbgDayNHours[d2]++;
				if(countGaps)
					sbgDayNGaps[d2]+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !subgroupNotAvailableDayHour(sbg,d2,h2))
				ng++;
		}
	}*/
}

inline bool Generate::subgroupRemoveAnActivityFromBeginOrEnd(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from begin or from end of any day
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexBegin=-1, actIndexEnd=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2,h2)>=0){
					actIndexBegin=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2,h2)>=0){
					actIndexEnd=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
					actIndexEnd=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}
						
	bool possibleBeginOrEnd=true;						
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;
				
	if(possibleBeginOrEnd){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(possibleDays.count());
		}
								
		assert(t>=0 && t<possibleDays.count());
							
		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);
		
			assert(sbgTimetable(d2,h2)==ai2);
								
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);
			
			assert(sbgTimetable(d2,h2)==ai2);
							
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBegin(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from begin of any day
	QList<int> possibleDays;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexBegin=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2,h2)>=0){
					actIndexBegin=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				acts.append(actIndexBegin);
			}
		}
	}
						
	bool possibleBegin=true;
	if(possibleDays.count()==0)
		possibleBegin=false;
				
	if(possibleBegin){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(possibleDays.count());
		}
								
		assert(t>=0 && t<possibleDays.count());
							
		int d2=possibleDays.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2<gt.rules.nHoursPerDay);
	
		assert(sbgTimetable(d2,h2)==ai2);
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);
	Q_UNUSED(level);

	if(sbgDayNHours[d2]>0){
		int actIndexBegin=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2,h2)>=0){
				actIndexBegin=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			removedActivity=actIndexBegin;
	
			assert(!conflActivities.contains(actIndexBegin));
			conflActivities.append(actIndexBegin);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEnd(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from begin or from end of any day
	QList<int> possibleDays;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2,h2)>=0){
					actIndexEnd=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				acts.append(actIndexEnd);
			}
		}
	}
						
	bool possibleEnd=true;
	if(possibleDays.count()==0)
		possibleEnd=false;
				
	if(possibleEnd){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(possibleDays.count());
		}
								
		assert(t>=0 && t<possibleDays.count());
							
		int d2=possibleDays.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2>=0);
		
		assert(sbgTimetable(d2,h2)==ai2);
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEndCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);
	Q_UNUSED(level);

	if(sbgDayNHours[d2]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2,h2)>=0){
				actIndexEnd=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			removedActivity=actIndexEnd;
	
			assert(!conflActivities.contains(actIndexEnd));
			conflActivities.append(actIndexEnd);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhere(int sbg, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from anywhere
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0){
					actIndex=sbgTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainDay(int sbg, int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from anywhere
	QList<int> acts;
	//for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0){
					actIndex=sbgTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	//}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int sbg, int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	Q_UNUSED(sbg);

	//remove an activity from anywhere
	QList<int> acts;
	//for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0){
					actIndex=sbgTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	//}
						
	if(acts.count()>0){
		int t;
						
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.size()>=1);
			int mpos=tl.at(randomKnuth(tl.size()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=randomKnuth(acts.count());
		}
								
		int ai2=acts.at(t);
		
		removedActivity=ai2;
							
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}


inline bool skipRandom(double weightPercentage)
{
	if(weightPercentage<0)
		return true; //non-existing constraint
		
	if(weightPercentage>=100.0)
		return false;

	double t=weightPercentage/100.0;
	assert(t>=0 && t<=1);
		
	t*=double(MM);
	int tt=int(floor(t+0.5));
	assert(tt>=0 /* && tt<=MM */); //the second condition is always true, because MM >= any int value
						
	int r=randomKnuth1MM1();
	assert(r>0 && r<MM); //r cannot be 0
	if(tt<=r)
		return true;
	else
		return false;
}


Generate::Generate()
{
}

Generate::~Generate()
{
}

bool Generate::precompute(QWidget* parent, QTextStream* initialOrderStream)
{
	return processTimeSpaceConstraints(parent, initialOrderStream);
}

inline bool Generate::checkBuildingChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list)
{
	assert((sbg==-1 && tch>=0) || (sbg>=0 && tch==-1));
	if(sbg>=0)
		assert(sbg<gt.rules.nInternalSubgroups);
	if(tch>=0)
		assert(tch<gt.rules.nInternalTeachers);

	if(sbg>=0)
		assert(minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 || maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0
		   || maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0);
	if(tch>=0)
		assert(minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 || maxBuildingChangesPerDayForTeachersPercentages[tch]>=0
		   || maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0);
		
	int buildings[MAX_HOURS_PER_DAY], activities[MAX_HOURS_PER_DAY];
	for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
		int ai2;
		if(sbg>=0)
			ai2=newSubgroupsTimetable(sbg,d,h2);
		else
			ai2=newTeachersTimetable(tch,d,h2);

		if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
			int rm2;
			if(h2>=h && h2<h+act->duration){
				assert(ai2==ai);
				rm2=rm;
			}
			else
				rm2=c.rooms[ai2];
			if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
				assert(rm2>=0);
				activities[h2]=ai2;
				buildings[h2]=gt.rules.internalRoomsList[rm2]->buildingIndex;
			}
			else{
				activities[h2]=ai2;
				buildings[h2]=-1;
			}
		}
		else{
			buildings[h2]=-1;
			activities[h2]=-1;
		}
	}
			
	if(buildings[h]==-1) //no problem
		return true;
			
	//min gaps
	double perc;
	int mg;
	if(sbg>=0){
		perc=minGapsBetweenBuildingChangesForStudentsPercentages[sbg];
		mg=minGapsBetweenBuildingChangesForStudentsMinGaps[sbg];
	}
	else{
		perc=minGapsBetweenBuildingChangesForTeachersPercentages[tch];
		mg=minGapsBetweenBuildingChangesForTeachersMinGaps[tch];
	}
	if(perc>=0){
		for(int h2=max(0, h-mg); h2<=min(h+act->duration-1+mg, gt.rules.nHoursPerDay-1); h2++)
			if(!(h2>=h && h2<h+act->duration))
				if(buildings[h2]!=buildings[h] && buildings[h2]!=-1){
					int ai2=activities[h2];
					assert(ai2>=0);
					if(!swappedActivities[ai2] && !(fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2])){
						if(!tmp_list.contains(ai2)){
							tmp_list.append(ai2);
							
							int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
							int dura=gt.rules.internalActivitiesList[ai2].duration;
							for(int h3=ha; h3<ha+dura; h3++){
								assert(activities[h3]==ai2);
								assert(buildings[h3]!=-1);
								buildings[h3]=-1;
								activities[h3]=-1;
							}
						}
					}
					else{
						return false;
					}
				}
	}

	//max changes per day
	int mc;
	if(sbg>=0){
		perc=maxBuildingChangesPerDayForStudentsPercentages[sbg];
		mc=maxBuildingChangesPerDayForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxBuildingChangesPerDayForTeachersPercentages[tch];
		mc=maxBuildingChangesPerDayForTeachersMaxChanges[tch];
	}
	
	if(perc>=0){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(buildings[h2]!=-1){
				if(crt_building!=buildings[h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=buildings[h2];
				}
			}
					
		if(n_changes>mc){ //not OK
			if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
				return false;
					
			QList<int> removableActsList;					
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(h2>=h && h2<h+act->duration))
					if(buildings[h2]!=-1 && activities[h2]>=0 && !swappedActivities[activities[h2]] && !(fixedTimeActivity[activities[h2]]&&fixedSpaceActivity[activities[h2]]))
						if(!removableActsList.contains(activities[h2])){
							removableActsList.append(activities[h2]);
							assert(!globalConflActivities.contains(activities[h2]));
							assert(!tmp_list.contains(activities[h2]));
						}
			}
					
			for(;;){
				int ai2=-1;
				QList<int> optimalRemovableActs;
				if(level==0){
					int nWrong=INF;
					foreach(int a, removableActsList)
						if(nWrong>triedRemovals(a,c.times[a]) ){
							nWrong=triedRemovals(a,c.times[a]);
						}
					foreach(int a, removableActsList)
						if(nWrong==triedRemovals(a,c.times[a]))
							optimalRemovableActs.append(a);
				}
				else
					optimalRemovableActs=removableActsList;
					
				if(removableActsList.count()>0)
					assert(optimalRemovableActs.count()>0);
					
				if(optimalRemovableActs.count()==0)
					return false;
					
				ai2=optimalRemovableActs.at(randomKnuth(optimalRemovableActs.count()));
				
				assert(!swappedActivities[ai2]);
				assert(!(fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2]));
				assert(!globalConflActivities.contains(ai2));
				assert(!tmp_list.contains(ai2));
				assert(ai2>=0);

				tmp_list.append(ai2);
						
				int t=removableActsList.removeAll(ai2);
				assert(t==1);
						
				int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
				int da=gt.rules.internalActivitiesList[ai2].duration;
				for(int h2=ha; h2<ha+da; h2++){
					assert(activities[h2]==ai2);
					assert(buildings[h2]!=-1);
					buildings[h2]=-1;
					activities[h2]=-1;
				}
						
				int crt_building=-1;
				int n_changes=0;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(buildings[h2]!=-1){
						if(crt_building!=buildings[h2]){
							if(crt_building!=-1)
								n_changes++;
							crt_building=buildings[h2];
						}
					}
			
				if(n_changes<=mc){ //OK
					break;
				}
			}
		}
	}
	
	//max changes per week
	if(sbg>=0){
		perc=maxBuildingChangesPerWeekForStudentsPercentages[sbg];
		mc=maxBuildingChangesPerWeekForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxBuildingChangesPerWeekForTeachersPercentages[tch];
		mc=maxBuildingChangesPerWeekForTeachersMaxChanges[tch];
	}	
	if(perc==-1){
		assert(mc==-1);
		return true;
	}
		
	//I would like to get rid of these large static variables, but making them dynamic slows down ~33% for a sample from Timisoara Economics
	int weekBuildings[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY], weekActivities[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2;
			if(sbg>=0)
				ai2=newSubgroupsTimetable(sbg,d2,h2);
			else
				ai2=newTeachersTimetable(tch,d2,h2);
	
			if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if(d==d2 && h2>=h && h2<h+act->duration){
					assert(ai2==ai);
					rm2=rm;
				}
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					assert(rm2>=0);
					weekActivities[d2][h2]=ai2;
					weekBuildings[d2][h2]=gt.rules.internalRoomsList[rm2]->buildingIndex;
				}
				else{
					weekActivities[d2][h2]=ai2;
					weekBuildings[d2][h2]=-1;
				}
			}
			else{
				weekBuildings[d2][h2]=-1;
				weekActivities[d2][h2]=-1;
			}
		}
	}

	int n_changes=0;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		int crt_building=-1;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(weekBuildings[d2][h2]!=-1){
				if(crt_building!=weekBuildings[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=weekBuildings[d2][h2];
				}
			}
	}
					
	if(n_changes>mc){ //not OK
		if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
			return false;
					
		QList<int> removableActsList;					
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(d2==d && h2>=h && h2<h+act->duration))
					if(weekBuildings[d2][h2]!=-1 && weekActivities[d2][h2]>=0 && !swappedActivities[weekActivities[d2][h2]] && !(fixedTimeActivity[weekActivities[d2][h2]]&&fixedSpaceActivity[weekActivities[d2][h2]]))
						if(!removableActsList.contains(weekActivities[d2][h2])){
							removableActsList.append(weekActivities[d2][h2]);
							assert(!globalConflActivities.contains(weekActivities[d2][h2]));
							assert(!tmp_list.contains(weekActivities[d2][h2]));
						}
			}
		}
					
		for(;;){
			int ai2=-1;
			QList<int> optimalRemovableActs;
			if(level==0){
				int nWrong=INF;
				foreach(int a, removableActsList)
					if(nWrong>triedRemovals(a,c.times[a])){
						nWrong=triedRemovals(a,c.times[a]);
					}
				foreach(int a, removableActsList)
					if(nWrong==triedRemovals(a,c.times[a]))
						optimalRemovableActs.append(a);
			}
			else
				optimalRemovableActs=removableActsList;
					
			if(removableActsList.count()>0)
				assert(optimalRemovableActs.count()>0);
					
			if(optimalRemovableActs.count()==0)
				return false;
					
			ai2=optimalRemovableActs.at(randomKnuth(optimalRemovableActs.count()));
				
			assert(!swappedActivities[ai2]);
			assert(!(fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2]));
			assert(!globalConflActivities.contains(ai2));
			assert(!tmp_list.contains(ai2));
			assert(ai2>=0);

			tmp_list.append(ai2);
						
			int t=removableActsList.removeAll(ai2);
			assert(t==1);
						
			int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
			int da=c.times[ai2]%gt.rules.nDaysPerWeek;
			int dura=gt.rules.internalActivitiesList[ai2].duration;
			for(int h2=ha; h2<ha+dura; h2++){
				assert(weekActivities[da][h2]==ai2);
				assert(weekBuildings[da][h2]!=-1);
				weekBuildings[da][h2]=-1;
				weekActivities[da][h2]=-1;
			}
						
			int n_changes=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
				int crt_building=-1;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(weekBuildings[d2][h2]!=-1){
						if(crt_building!=weekBuildings[d2][h2]){
							if(crt_building!=-1)
								n_changes++;
							crt_building=weekBuildings[d2][h2];
						}
					}
			}
		
			if(n_changes<=mc){ //OK
				break;
			}
		}
	}
	
	return true;
}

//2012-04-29
inline bool Generate::checkActivitiesOccupyMaxDifferentRooms(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list)
{
	foreach(ActivitiesOccupyMaxDifferentRooms_item* item, aomdrListForActivity[ai]){
		//preliminary
		QSet<int> occupiedRoomsSet;
		foreach(int ai2, item->activitiesList)
			if(ai2!=ai && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM)
					if(!occupiedRoomsSet.contains(rm2)){
						occupiedRoomsSet.insert(rm2);
						if(occupiedRoomsSet.count()==item->maxDifferentRooms) //no further testing needed
							break;
					}
			}
			
		if(!globalConflActivities.contains(ai) && !tmp_list.contains(ai)) //should be always true
			if(rm!=UNALLOCATED_SPACE && rm!=UNSPECIFIED_ROOM) //should be always true
				if(!occupiedRoomsSet.contains(rm))
					occupiedRoomsSet.insert(rm);
			
		if(occupiedRoomsSet.count()<=item->maxDifferentRooms)
			continue;

		//correction needed
		QList<QSet<int> > activitiesInRoom;
		occupiedRoomsSet.clear();
		QList<int> occupiedRoomsList;
		QHash<int, int> roomIndexInOccupiedRoomsList;
		QList<bool> canEmptyRoom;
		
		foreach(int ai2, item->activitiesList)
			if(!globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if(ai2==ai)
					rm2=rm;
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					int ind;
					if(!occupiedRoomsSet.contains(rm2)){
						occupiedRoomsSet.insert(rm2);
						occupiedRoomsList.append(rm2);
						canEmptyRoom.append(true);
						
						QSet<int> tl;
						tl.insert(ai2);
						activitiesInRoom.append(tl);
						
						ind=activitiesInRoom.count()-1;
						roomIndexInOccupiedRoomsList.insert(rm2, ind);
					}
					else{
						assert(roomIndexInOccupiedRoomsList.contains(rm2));
						ind=roomIndexInOccupiedRoomsList.value(rm2);
						assert(ind>=0 && ind<activitiesInRoom.count());
						activitiesInRoom[ind].insert(ai2);
					}
					
					if(ai2==ai || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) || swappedActivities[ai2])
						if(canEmptyRoom[ind]==true)
							canEmptyRoom[ind]=false;
				}
			}
			
		assert(occupiedRoomsSet.count()==item->maxDifferentRooms+1);
			
		QList<int> candidates;
		foreach(int rm2, occupiedRoomsList){
			assert(roomIndexInOccupiedRoomsList.contains(rm2));
			int ind=roomIndexInOccupiedRoomsList.value(rm2);
			if(canEmptyRoom.at(ind)==true)
				candidates.append(ind);
		}

		if(level==0){
			QList<int> finalCandidates;
			
			int optConflActivities=MAX_ACTIVITIES;
			int optMinWrong=INF;
			int optNWrong=INF;
			int optMinIndexAct=gt.rules.nInternalActivities;

			//phase 1
			foreach(int ind, candidates){
				QSet<int> activitiesForCandidate=activitiesInRoom.at(ind);
				
				int tmp_n_confl_acts=activitiesForCandidate.count();
				int tmp_minWrong=INF;
				int tmp_nWrong=0;
				int tmp_minIndexAct=gt.rules.nInternalActivities;
				
				if(activitiesForCandidate.count()>0){
					foreach(int ai2, activitiesForCandidate){
						tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
						tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
						tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
					}
				}
				else{
					assert(0);
					tmp_minWrong=0;
					tmp_nWrong=0;
					tmp_minIndexAct=-1;
				}
		
				if(optMinWrong>tmp_minWrong ||
				  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
				  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
				  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
					optConflActivities=tmp_n_confl_acts;
					optMinWrong=tmp_minWrong;
					optNWrong=tmp_nWrong;
					optMinIndexAct=tmp_minIndexAct;
				}
			}

			//phase 2
			foreach(int ind, candidates){
				QSet<int> activitiesForCandidate=activitiesInRoom.at(ind);
				
				int tmp_n_confl_acts=activitiesForCandidate.count();
				int tmp_minWrong=INF;
				int tmp_nWrong=0;
				int tmp_minIndexAct=gt.rules.nInternalActivities;
				
				if(activitiesForCandidate.count()>0){
					foreach(int ai2, activitiesForCandidate){
						tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
						tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
						tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
					}
				}
				else{
					assert(0);
					tmp_minWrong=0;
					tmp_nWrong=0;
					tmp_minIndexAct=-1;
				}
		
				if(optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct==tmp_minIndexAct){
					finalCandidates.append(ind);
				}
			}
			
			//phase 3
			if(candidates.count()>0)
				assert(finalCandidates.count()>0);
			candidates=finalCandidates;
		}
		else{ //if(level>0)
			QList<int> finalCandidates;
			
			int optConflActivities=MAX_ACTIVITIES;

			foreach(int ind, candidates){
				if(activitiesInRoom.at(ind).count()<optConflActivities){
					optConflActivities=activitiesInRoom.at(ind).count();
					finalCandidates.clear();
					finalCandidates.append(ind);
				}
				else if(activitiesInRoom.at(ind).count()==optConflActivities){
					finalCandidates.append(ind);
				}
			}
			
			if(candidates.count()>0)
				assert(finalCandidates.count()>0);
			candidates=finalCandidates;
		}
		
		if(candidates.count()==0)
			return false;
		
		int indexToRemove=candidates.at(randomKnuth(candidates.count()));
		assert(canEmptyRoom.at(indexToRemove)==true);
		
		//To keep generation identical on all computers - 2013-01-03
		QList<int> tmpListFromSet=activitiesInRoom.at(indexToRemove).toList();
		//qSort(tmpListFromSet);
		std::stable_sort(tmpListFromSet.begin(), tmpListFromSet.end());
		//Randomize list
		for(int i=0; i<tmpListFromSet.count(); i++){
			int t=tmpListFromSet.at(i);
			int r=randomKnuth(tmpListFromSet.count()-i);
			tmpListFromSet[i]=tmpListFromSet[i+r];
			tmpListFromSet[i+r]=t;
		}
		
		foreach(int ai2, tmpListFromSet){
			assert(!globalConflActivities.contains(ai2) && !tmp_list.contains(ai2));
			assert(ai2!=ai && !(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) && !swappedActivities[ai2]);
			tmp_list.append(ai2);
		}
	}
	
	return true;
}

//2013-09-14
inline bool Generate::checkActivitiesSameRoomIfConsecutive(const QList<int>& globalConflActivities, int rm, int level, int ai, int d, int h, QList<int>& tmp_list)
{
	Q_UNUSED(level);

	foreach(ActivitiesSameRoomIfConsecutive_item* item, asricListForActivity[ai]){
		foreach(int ai2, item->activitiesList){
			if(ai2!=ai && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					if(c.times[ai2]!=UNALLOCATED_TIME){
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
						
						if( d==d2 && (h2==h+gt.rules.internalActivitiesList[ai].duration || h==h2+gt.rules.internalActivitiesList[ai2].duration) ){
							if(rm!=rm2){ //not OK
								if(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) && !swappedActivities[ai2])
									tmp_list.append(ai2);
								else
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return true;
}

inline bool Generate::chooseRoom(const QList<int>& listOfRooms, const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities)
{
	roomSlot=selectedSlot=UNSPECIFIED_ROOM; //if we don't find a room, return these values

	Q_UNUSED(ai);

	int optConflActivities=MAX_ACTIVITIES;
	int optMinWrong=INF;
	int optNWrong=INF;
	int optMinIndexAct=gt.rules.nInternalActivities;
	
	QList<QList<int> > conflActivitiesRooms;
	QList<int> nConflActivitiesRooms;
	QList<int> listedRooms;
	
	QList<int> minWrong;
	QList<int> nWrong;
	QList<int> minIndexAct;
			
	QList<int> tmp_list;
	int tmp_n_confl_acts;
	int tmp_minWrong;
	int tmp_nWrong;
	int tmp_minIndexAct;
	
	int newtime=d+h*gt.rules.nDaysPerWeek;
			
	foreach(int rm, listOfRooms){
		int dur;
		for(dur=0; dur<act->duration; dur++)
			if(notAllowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]>=0 &&
			 !skipRandom(notAllowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]))
			 	break;
						
		if(dur==act->duration){
			tmp_list.clear();
					
			int dur2;
			for(dur2=0; dur2<act->duration; dur2++){
				int ai2=roomsTimetable(rm,d,h+dur2);
				if(ai2>=0){
					if(!globalConflActivities.contains(ai2)){
						if(swappedActivities[ai2] || (fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2])){
							tmp_n_confl_acts=MAX_ACTIVITIES; //not really needed
							break;
						}
						else{
							if(!tmp_list.contains(ai2)){
								tmp_list.append(ai2);
							}
						}
					}
				}
			}
			if(dur2==act->duration){
				//see building changes
				
				//building changes for students
				bool ok=true;
				foreach(int sbg, act->iSubgroupsList){
					if(minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 || maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0
					  || maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0){
						ok=checkBuildingChanges(sbg, -1, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
						if(!ok)
							break;
					}
				}

				if(!ok)
					continue;
			
				//building changes for teachers
				foreach(int tch, act->iTeachersList){
					if(minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 || maxBuildingChangesPerDayForTeachersPercentages[tch]>=0
					  || maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0){
						ok=checkBuildingChanges(-1, tch, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
						if(!ok)
							break;
					}
				}

				if(!ok)
					continue;
			
				//max occupied rooms for a set of activities - 2012-04-29
				if(aomdrListForActivity[ai].count()>0)
					ok=checkActivitiesOccupyMaxDifferentRooms(globalConflActivities, rm, level, ai, tmp_list);
				
				if(!ok)
					continue;
					
				//activities same room if consecutive - introduced on 2013-09-14
				if(asricListForActivity[ai].count()>0)
					ok=checkActivitiesSameRoomIfConsecutive(globalConflActivities, rm, level, ai, d, h, tmp_list);
				
				if(!ok)
					continue;
					
				tmp_minWrong=INF;
				tmp_nWrong=0;
				
				tmp_minIndexAct=gt.rules.nInternalActivities;
				
				tmp_n_confl_acts=tmp_list.count();
				
				if(level==0){
					if(tmp_list.count()>0){ //serious bug corrected on 2012-05-02, but it seems that it didn't affect people until now
						foreach(int ai2, tmp_list){
							tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
							tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
							tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
						}
					}
					else{
						tmp_minWrong=0;
						tmp_nWrong=0;
						tmp_minIndexAct=-1;
					}
				}
				
				listedRooms.append(rm);
				nConflActivitiesRooms.append(tmp_n_confl_acts);
				conflActivitiesRooms.append(tmp_list);
				
				if(level>0){
					if(tmp_n_confl_acts<optConflActivities)
						optConflActivities=tmp_n_confl_acts;
				}
				else{ // if(level==0)
					minWrong.append(tmp_minWrong);
					nWrong.append(tmp_nWrong);
					minIndexAct.append(tmp_minIndexAct);
	
					if(optMinWrong>tmp_minWrong ||
					  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
					  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
					  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
						optConflActivities=tmp_n_confl_acts;
						optMinWrong=tmp_minWrong;
						optNWrong=tmp_nWrong;
						optMinIndexAct=tmp_minIndexAct;
					}
				}
			}
		}
		else //not really needed
			tmp_n_confl_acts=MAX_ACTIVITIES;
	}
		
	if(optConflActivities==MAX_ACTIVITIES) //roomSlot == selectedSlot == UNSPECIFIED_ROOM
		return false;
		
	assert(optConflActivities<MAX_ACTIVITIES);
	
	QList<int> allowedRoomsIndex;

	assert(listedRooms.count()==nConflActivitiesRooms.count());
	assert(listedRooms.count()==conflActivitiesRooms.count());
				
	if(level>0){
		for(int q=0; q<listedRooms.count(); q++){
			//The 'mapr' custom version disables the test below and always does an .append(q). But this
			//gives a very bad behavior for the file examples/Egypt/Dakahlia/institution-2012-2013.fet
			//(usually, the file seems not to solve anymore, and one time it solved very slowly).
			if(nConflActivitiesRooms.at(q)==optConflActivities){
				allowedRoomsIndex.append(q);
			}
		}
	}
	else{
		for(int q=0; q<listedRooms.count(); q++){
			if(optMinWrong==minWrong.at(q) && optNWrong==nWrong.at(q) && optConflActivities==nConflActivitiesRooms.at(q) && optMinIndexAct==minIndexAct.at(q)){
				allowedRoomsIndex.append(q);
			}
		}
		/*if(allowedRoomsIndex.count()!=1)
			cout<<"allowedRoomsIndex.count()=="<<allowedRoomsIndex.count()<<endl;
		assert(allowedRoomsIndex.count()==1);*/
	}
					
	assert(allowedRoomsIndex.count()>0);
	int q=randomKnuth(allowedRoomsIndex.count());
	int t=allowedRoomsIndex.at(q);
	assert(t>=0 && t<listedRooms.count());
	int r=listedRooms.at(t);
	assert(r>=0 && r<gt.rules.nInternalRooms);
	selectedSlot=r;
	roomSlot=r;
				
	assert(nConflActivitiesRooms.at(t)==conflActivitiesRooms.at(t).count());
				
	localConflActivities.clear(); /////Liviu: added 22 august 2008, nasty crash bug fix
				
	foreach(int a, conflActivitiesRooms.at(t)){
		assert(!globalConflActivities.contains(a));
		assert(!localConflActivities.contains(a)); ///////////Liviu: added 22 august 2008
		localConflActivities.append(a);
	}
	
	return true;
}

inline bool Generate::getHomeRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities)
{
	assert(!unspecifiedHomeRoom[ai]);

	return chooseRoom(activitiesHomeRoomsHomeRooms[ai], globalConflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities);
}

inline bool Generate::getPreferredRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, bool& canBeUnspecifiedPreferredRoom)
{
	assert(!unspecifiedPreferredRoom[ai]);
	
	bool unspecifiedRoom=true;
	QSet<int> allowedRooms;
	foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ai]){
		bool skip=skipRandom(it.percentage);
		
		if(!skip){
			if(unspecifiedRoom){
				unspecifiedRoom=false;
				allowedRooms=it.preferredRooms;
			}
			else{
				allowedRooms.intersect(it.preferredRooms);
			}
		}
		else{
			if(unspecifiedRoom){
				allowedRooms.unite(it.preferredRooms);
			}
			else{
				//do nothing
			}
		}
	}
	
	QList<int> allowedRoomsList;
	foreach(int rm, allowedRooms)
		allowedRoomsList.append(rm);
	//qSort(allowedRoomsList); //To keep generation identical on all computers - 2013-01-03
	std::stable_sort(allowedRoomsList.begin(), allowedRoomsList.end()); //To keep generation identical on all computers - 2013-01-03
	//Randomize list
	for(int i=0; i<allowedRoomsList.count(); i++){
		int t=allowedRoomsList.at(i);
		int r=randomKnuth(allowedRoomsList.count()-i);
		allowedRoomsList[i]=allowedRoomsList[i+r];
		allowedRoomsList[i+r]=t;
	}
		
	canBeUnspecifiedPreferredRoom=unspecifiedRoom;

	return chooseRoom(allowedRoomsList, globalConflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities);
}

inline bool Generate::getRoom(int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& conflActivities, int& nConflActivities)
{
	bool okh;
	
	QList<int> localConflActivities;
	
	if(unspecifiedPreferredRoom[ai]){
		if(unspecifiedHomeRoom[ai]){
			roomSlot=UNSPECIFIED_ROOM;
			selectedSlot=UNSPECIFIED_ROOM;
			return true;
		}
		else{
			okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities);
			if(okh){
				foreach(int t, localConflActivities){
					conflActivities.append(t);
					nConflActivities++;
				}
				return okh;
			}
			else{
				okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
				return okh;
			}
		}
	}
	else{
		bool okp;
		bool canBeUnspecifiedPreferredRoom;
	
		okp=getPreferredRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, canBeUnspecifiedPreferredRoom);
		if(okp && localConflActivities.count()==0){
			/*foreach(int t, localConflActivities){
				conflActivities.append(t);
				nConflActivities++;
			}
			assert(nConflActivities==conflActivities.count());*/
			return okp;
		}
		else if(okp){
			if(canBeUnspecifiedPreferredRoom){ //skipRandom(activitiesPreferredRoomsPercentage[ai])){
				//get a home room
				if(unspecifiedHomeRoom[ai]){
					roomSlot=UNSPECIFIED_ROOM;
					selectedSlot=UNSPECIFIED_ROOM;
					return true;
				}
				
				okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities);
				if(okh){
					foreach(int t, localConflActivities){
						conflActivities.append(t);
						nConflActivities++;
					}
					return okh;
				}
				else{
					okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
					return okh;
				}
			}
			else{
				foreach(int t, localConflActivities){
					conflActivities.append(t);
					nConflActivities++;
				}
				assert(nConflActivities==conflActivities.count());
				assert(okp==true);
				return okp;
				//get this preferred room
			}
		}
		else{ //!ok from preferred room, search a home room
			if(canBeUnspecifiedPreferredRoom){ //skipRandom(activitiesPreferredRoomsPercentage[ai])){
				//get a home room
				if(unspecifiedHomeRoom[ai]){
					roomSlot=UNSPECIFIED_ROOM;
					selectedSlot=UNSPECIFIED_ROOM;
					return true;
				}
				
				okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities);
				if(okh){
					foreach(int t, localConflActivities){
						conflActivities.append(t);
						nConflActivities++;
					}
					return okh;
				}
				else{
					okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
					return okh;
				}
			}
			else{
				assert(okp==false);
				return okp;
			}
		}
	}
}

void Generate::generate(int maxSeconds, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream)
{
	this->isThreaded=threaded;

	l0nWrong.resize(gt.rules.nHoursPerWeek);
	l0minWrong.resize(gt.rules.nHoursPerWeek);
	l0minIndexAct.resize(gt.rules.nHoursPerWeek);

	teachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	subgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	roomsTimetable.resize(gt.rules.nInternalRooms, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);

	newTeachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	newSubgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	newTeachersDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	newTeachersDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	newSubgroupsDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	newSubgroupsDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	newSubgroupsDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);

	oldTeachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	oldSubgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	oldTeachersDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	oldTeachersDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);


	tchTimetable.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	tchDayNHours.resize(gt.rules.nDaysPerWeek);
	tchDayNGaps.resize(gt.rules.nDaysPerWeek);

	sbgTimetable.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	sbgDayNHours.resize(gt.rules.nDaysPerWeek);
	sbgDayNGaps.resize(gt.rules.nDaysPerWeek);
	sbgDayNFirstGaps.resize(gt.rules.nDaysPerWeek);
	
	teacherActivitiesOfTheDay.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	subgroupActivitiesOfTheDay.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	
	//2011-09-30
	if(haveActivitiesOccupyOrSimultaneousConstraints){
		activitiesAtTime.resize(gt.rules.nHoursPerWeek);

		slotSetOfActivities.resize(gt.rules.nHoursPerWeek);
		slotCanEmpty.resize(gt.rules.nHoursPerWeek);
	}

if(threaded){
		myMutex.lock();
}
	c.makeUnallocated(gt.rules);

	nDifficultActivities=0;

	impossible=false;
	timeExceeded=false;

	maxncallsrandomswap=-1;

	impossibleActivity=false;
	
	maxActivitiesPlaced=0;

if(threaded){
		myMutex.unlock();
}

	triedRemovals.resize(gt.rules.nInternalActivities, gt.rules.nHoursPerWeek);
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			triedRemovals(i,j)=0;
			
	////////init tabu
	tabu_size=gt.rules.nInternalActivities*gt.rules.nHoursPerWeek;
	//assert(tabu_size<=MAX_TABU);
	crt_tabu_index=0;
	/*qint16 tabu_activities[MAX_TABU];
	qint16 tabu_times[MAX_TABU];*/
	tabu_activities.resize(tabu_size);
	tabu_times.resize(tabu_size);
	for(int i=0; i<tabu_size; i++)
		tabu_activities[i]=tabu_times[i]=-1;
	/////////////////

	//abortOptimization=false; you have to take care of this before calling this function

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		invPermutation[permutation[i]]=i;

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		swappedActivities[permutation[i]]=false;

	//tzset();
	time_t starting_time;
	time(&starting_time);
	
if(threaded){
		myMutex.lock();
}
	timeToHighestStage=0;
	searchTime=0;
	generationStartDateTime=QDateTime::currentDateTime();
if(threaded){
		myMutex.unlock();
}
	
	//2000 was before
	//limitcallsrandomswap=1000; //1600, 1500 also good values, 1000 too low???
	limitcallsrandomswap=2*gt.rules.nInternalActivities; //???? value found practically
	
	level_limit=14; //20; //16
	
	assert(level_limit<MAX_LEVEL);
	
	for(int added_act=0; added_act<gt.rules.nInternalActivities; added_act++){
		prevvalue:

if(threaded){
		myMutex.lock();
}
		if(abortOptimization){
if(threaded){
			myMutex.unlock();
}
			return;
		}
		time_t crt_time;
		time(&crt_time);
		searchTime=int(crt_time-starting_time);
		
		if(searchTime>=maxSeconds){
if(threaded){
			myMutex.unlock();
}
			
			timeExceeded=true;
			
			return;
		}

		for(int i=0; i<=added_act; i++)
			swappedActivities[permutation[i]]=false;
		for(int i=added_act+1; i<gt.rules.nInternalActivities; i++)
			assert(!swappedActivities[permutation[i]]);

		if(VERBOSE){
			cout<<endl<<"Trying to place activity number added_act=="<<added_act<<
			 "\nwith id=="<<gt.rules.internalActivitiesList[permutation[added_act]].id<<
			 ", from nInternalActivities=="<<gt.rules.nInternalActivities<<endl;
		}
	 	//verifyUnallocated(permutation[added_act]]);
		//assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
		//assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		if(fixedTimeActivity[permutation[added_act]] && fixedSpaceActivity[permutation[added_act]]){
			assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else if(fixedTimeActivity[permutation[added_act]] && !fixedSpaceActivity[permutation[added_act]]){
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else if(!fixedTimeActivity[permutation[added_act]]){
			assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else
			assert(0);

		for(int i=0; i<added_act; i++){
			if(c.times[permutation[i]]==UNALLOCATED_TIME)
				cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
			assert(c.times[permutation[i]]!=UNALLOCATED_TIME);
			/*for(int j=0; j<gt.rules.internalActivitiesList[permutation[i]].duration; j++)
				tlistSet[c.times[permutation[i]]+j*gt.rules.nDaysPerWeek].insert(permutation[i]);*/

			if(c.rooms[permutation[i]]==UNALLOCATED_SPACE)
				cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has room unallocated"<<endl;
			assert(c.rooms[permutation[i]]!=UNALLOCATED_SPACE);
		}

		///////////////rooms' timetable
		for(int i=0; i<gt.rules.nInternalRooms; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					roomsTimetable(i,j,k)=-1;
		for(int j=0; j<added_act; j++){
			int i=permutation[j];
			assert(c.rooms[i]!=UNALLOCATED_SPACE);
			if(c.rooms[i]!=UNSPECIFIED_ROOM){
				int rm=c.rooms[i];
			
				Activity* act=&gt.rules.internalActivitiesList[i];
				int hour=c.times[i]/gt.rules.nDaysPerWeek;
				int day=c.times[i]%gt.rules.nDaysPerWeek;
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(roomsTimetable(rm,day,hour+dd)==-1);
					roomsTimetable(rm,day,hour+dd)=i;
				}
			}
		}
		///////////////////////////////
				
		//subgroups' timetable
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					subgroupsTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			foreach(int sb, act->iSubgroupsList){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(subgroupsTimetable(sb,day,hour+dd)==-1);
					subgroupsTimetable(sb,day,hour+dd)=i;
				}
			}
		}

		//new
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					newSubgroupsTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			foreach(int sb, act->iSubgroupsList){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(newSubgroupsTimetable(sb,day,hour+dd)==-1);
					newSubgroupsTimetable(sb,day,hour+dd)=i;
				}
			}
		}

		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			subgroupGetNHoursGaps(i);

		//////////////care for students max days per week
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				subgroupActivitiesOfTheDay(i,j).clear();
																	
		for(int i=0; i<gt.rules.nInternalActivities/*added_act*/; i++){
			if(i<added_act){
			}
			else{
				if(c.times[permutation[i]]==UNALLOCATED_TIME)
					continue;
			}
			//Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			int d=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
			
			foreach(int j, subgroupsWithMaxDaysPerWeekForActivities[permutation[i]]){
				assert(subgroupActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
				subgroupActivitiesOfTheDay(j,d).append(permutation[i]);
			}
		}
		//////////////

		//teachers' timetable
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					teachersTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			foreach(int tc, act->iTeachersList){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(teachersTimetable(tc,day,hour+dd)==-1);
					teachersTimetable(tc,day,hour+dd)=i;
				}
			}
		}

		//new
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					newTeachersTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			foreach(int tc, act->iTeachersList){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(newTeachersTimetable(tc,day,hour+dd)==-1);
					newTeachersTimetable(tc,day,hour+dd)=i;
				}
			}
		}

		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			teacherGetNHoursGaps(i);
		
		//////////////care for teachers max days per week
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				teacherActivitiesOfTheDay(i,j).clear();
																	
		for(int i=0; i<gt.rules.nInternalActivities/*added_act*/; i++){
			if(i<added_act){
			}
			else{
				if(c.times[permutation[i]]==UNALLOCATED_TIME)
					continue;
			}
			//Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			int d=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
			
			foreach(int j, teachersWithMaxDaysPerWeekForActivities[permutation[i]]){
				assert(teacherActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
				teacherActivitiesOfTheDay(j,d).append(permutation[i]);
			}
		}
		//////////////
		
		//2011-09-30
		if(haveActivitiesOccupyOrSimultaneousConstraints){
			for(int t=0; t<gt.rules.nHoursPerWeek; t++)
				activitiesAtTime[t].clear();
	
			for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
				int i=permutation[j];
				if(j<added_act){
					assert(c.times[i]!=UNALLOCATED_TIME);
				}
				else{
					if(c.times[i]==UNALLOCATED_TIME)
						continue;
				}
				assert(c.times[i]!=UNALLOCATED_TIME);
				
				Activity* act=&gt.rules.internalActivitiesList[i];
			
				for(int t=c.times[i]; t<c.times[i]+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(!activitiesAtTime[t].contains(i));
					activitiesAtTime[t].insert(i);
				}
			}
		}
		//////////////
		
		foundGoodSwap=false;
	
		assert(!swappedActivities[permutation[added_act]]);
		swappedActivities[permutation[added_act]]=true;

		nRestore=0;
		ncallsrandomswap=0;
		randomSwap(permutation[added_act], 0);
		
		if(!foundGoodSwap){
			if(impossibleActivity){
if(threaded){
				myMutex.unlock();
}
				nDifficultActivities=1;
				difficultActivities[0]=permutation[added_act];
				
				impossible=true;
				
				emit(impossibleToSolve());
				
				return;
			}
		
			//update difficult activities (activities which are placed correctly so far, together with added_act)
			nDifficultActivities=added_act+1;
			if(VERBOSE){
				cout<<"nDifficultActivities=="<<nDifficultActivities<<endl;
			}
			for(int j=0; j<=added_act; j++)
				difficultActivities[j]=permutation[j];
			
//////////////////////
			assert(conflActivitiesTimeSlot.count()>0);
			
			if(VERBOSE){
				cout<<"conflActivitiesTimeSlot.count()=="<<conflActivitiesTimeSlot.count()<<endl;
				foreach(int i, conflActivitiesTimeSlot){
					cout<<"Confl activity id:"<<gt.rules.internalActivitiesList[i].id;
					cout<<" time of this activity:"<<c.times[i];
					if(c.rooms[i]!=UNSPECIFIED_ROOM)
						cout<<" room of this activity:"<<qPrintable(gt.rules.internalRoomsList[c.rooms[i]]->name)<<endl;
					else
						cout<<" room of this activity: UNSPECIFIED_ROOM"<<endl;
				}
				//cout<<endl;
				cout<<"timeSlot=="<<timeSlot<<endl;
				if(roomSlot!=UNSPECIFIED_ROOM)
					cout<<"roomSlot=="<<qPrintable(gt.rules.internalRoomsList[roomSlot]->name)<<endl;
				else
					cout<<"roomSlot==UNSPECIFIED_ROOM"<<endl;
			}

			QList<int> ok;
			QList<int> confl;
			for(int j=0; j<added_act; j++){
				if(conflActivitiesTimeSlot.indexOf(permutation[j])!=-1){
					if(VERBOSE){
						if(triedRemovals(permutation[j],c.times[permutation[j]])>0){
							cout<<"Warning - explored removal: id=="<<
							 gt.rules.internalActivitiesList[permutation[j]].id<<", time=="<<c.times[permutation[j]]
							 <<", times=="<<triedRemovals(permutation[j],c.times[permutation[j]])<<endl;
						}
					}
					triedRemovals(permutation[j],c.times[permutation[j]])++;
					
					/////update tabu
					int a=tabu_activities[crt_tabu_index];
					int t=tabu_times[crt_tabu_index];
					if(a>=0 && t>=0){
						assert(triedRemovals(a,t)>0);
						triedRemovals(a,t)--;
						//cout<<"Removing activity with id=="<<gt.rules.internalActivitiesList[a].id<<", time=="<<t<<endl;
					}
					tabu_activities[crt_tabu_index]=permutation[j];
					tabu_times[crt_tabu_index]=c.times[permutation[j]];
					//cout<<"Inserting activity with id=="<<gt.rules.internalActivitiesList[permutation[j]].id<<", time=="<<c.times[permutation[j]]<<endl;
					crt_tabu_index=(crt_tabu_index+1)%tabu_size;
					////////////////
				
					confl.append(permutation[j]);
				}
				else
					ok.append(permutation[j]);
			}
				
			assert(confl.count()==conflActivitiesTimeSlot.count());
			
			int j=0;
			int tmp=permutation[added_act];
			foreach(int k, ok){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
			}
			int q=j;
			//cout<<"q=="<<q<<endl;
			permutation[j]=tmp;
			invPermutation[tmp]=j;
			j++;
			if(VERBOSE){
				cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
				cout<<"conflicting:"<<endl;
			}
			foreach(int k, confl){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
				if(VERBOSE){
					cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
				}
			}
			assert(j==added_act+1);
			
			//check
			/*int pv[MAX_ACTIVITIES];
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				pv[i]=0;
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				pv[permutation[i]]++;
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				assert(pv[i]==1);*/
			//

			if(VERBOSE){
				cout<<"tmp represents activity with id=="<<gt.rules.internalActivitiesList[tmp].id;
				cout<<" initial time: "<<c.times[tmp];
				cout<<" final time: "<<timeSlot<<endl;
			}
			c.times[tmp]=timeSlot;
			c.rooms[tmp]=roomSlot;
			
			for(int i=q+1; i<=added_act; i++){
				if(!fixedTimeActivity[permutation[i]])
					c.times[permutation[i]]=UNALLOCATED_TIME;
				c.rooms[permutation[i]]=UNALLOCATED_SPACE;
			}
			c._fitness=-1;
			c.changedForMatrixCalculation=true;
				
			added_act=q+1;
if(threaded){
			myMutex.unlock();
}
	
			//if(semaphorePlacedActivity){
				emit(activityPlaced(q+1));
if(threaded){
				semaphorePlacedActivity.acquire();
}
			//}

			goto prevvalue;
//////////////////////
		}			
		else{ //if foundGoodSwap==true
			nPlacedActivities=added_act+1;
			
			if(maxActivitiesPlaced<added_act+1){
				generationHighestStageDateTime=QDateTime::currentDateTime();
				time_t tmp;
				time(&tmp);
				timeToHighestStage=int(tmp-starting_time);
				
				highestStageSolution.copy(gt.rules, c);

				maxActivitiesPlaced=added_act+1;
				
				if(maxPlacedActivityStream!=NULL){
					int sec=timeToHighestStage;
					int hh=sec/3600;
					sec%=3600;
					int mm=sec/60;
					sec%=60;
					QString s=tr("At time %1 h %2 m %3 s, FET reached %4 activities placed", "h=hours, m=minutes, s=seconds. Please leave spaces between 'time', %1, h, %2, m, %3, s, so they are visible")
						.arg(hh).arg(mm).arg(sec).arg(maxActivitiesPlaced);
					//s+="\n";
					//QString s=QString("At time ")+CustomFETString::number(hh)+QString(" h ")+CustomFETString::number(mm)+QString(" m ")+CustomFETString::number(sec)
					//	+QString(" s, FET reached ")+CustomFETString::number(maxActivitiesPlaced)+QString(" activities placed\n");
					
					(*maxPlacedActivityStream)<<s<<endl;
					//(*maxPlacedActivityStream).flush();
				}
			}
			
if(threaded){
			myMutex.unlock();
}
			emit(activityPlaced(added_act+1));
if(threaded){
			semaphorePlacedActivity.acquire();
}
if(threaded){
			myMutex.lock();
}
			if(added_act==gt.rules.nInternalActivities && foundGoodSwap){
if(threaded){
				myMutex.unlock();
}
				break;
			}
			
			bool ok=true;
			for(int i=0; i<=added_act; i++){
				if(c.times[permutation[i]]==UNALLOCATED_TIME){
					cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
					ok=false;
				}
				if(c.rooms[permutation[i]]==UNALLOCATED_SPACE){
					cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has room unallocated"<<endl;
					ok=false;
				}
			}
			assert(ok);
		}

if(threaded){
		myMutex.unlock();
}
	}

	time_t end_time;
	time(&end_time);
	searchTime=int(end_time-starting_time);
#if FET_COMMAND_LINE
	cout<<"Total searching time (seconds): "<<int(end_time-starting_time)<<endl;
#else
	if(VERBOSE){
		cout<<"Total searching time (seconds): "<<int(end_time-starting_time)<<endl;
	}
#endif

	emit(simulationFinished());
	
	finishedSemaphore.release();
}

void Generate::moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom)
{
	Activity* act=&gt.rules.internalActivitiesList[ai];

	//cout<<"here: id of act=="<<act->id<<", fromslot=="<<fromslot<<", toslot=="<<toslot<<endl;

	assert(fromslot==c.times[ai]);
	assert(fromroom==c.rooms[ai]);
	
	if(!fixedTimeActivity[ai] && (fromslot==UNALLOCATED_TIME || fromroom==UNALLOCATED_SPACE))
		assert(fromslot==UNALLOCATED_TIME && fromroom==UNALLOCATED_SPACE);
	if(!fixedTimeActivity[ai] && (toslot==UNALLOCATED_TIME || toroom==UNALLOCATED_SPACE))
		assert(toslot==UNALLOCATED_TIME && toroom==UNALLOCATED_SPACE);
	
	if(fromslot!=UNALLOCATED_TIME){
		int d=fromslot%gt.rules.nDaysPerWeek;
		int h=fromslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=c.rooms[ai];
		if(rm!=UNSPECIFIED_ROOM && rm!=UNALLOCATED_SPACE)
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				if(roomsTimetable(rm,d,h+dd)==ai)
					roomsTimetable(rm,d,h+dd)=-1;
				else
					assert(0);
			}
		/////////////////////
		
		if(fromslot!=toslot){
			//timetable of students
			for(int q=0; q<act->iSubgroupsList.count(); q++){
				int sb=act->iSubgroupsList.at(q);	
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(subgroupsTimetable(sb,d,h+dd)==ai);
					subgroupsTimetable(sb,d,h+dd)=-1;
				}
			}	

			foreach(int sb, mustComputeTimetableSubgroups[ai]){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newSubgroupsTimetable(sb,d,h+dd)==ai);
					newSubgroupsTimetable(sb,d,h+dd)=-1;
				}
			}

			updateSubgroupsNHoursGaps(act, ai, d);

			//update students' list of activities for each day
			/////////////////
			foreach(int st, subgroupsWithMaxDaysPerWeekForActivities[ai]){
				int tt=subgroupActivitiesOfTheDay(st,d).removeAll(ai);
				assert(tt==1);
			}
			/////////////////

			//teachers' timetable
			for(int q=0; q<act->iTeachersList.count(); q++){
				int tch=act->iTeachersList.at(q);	
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					
					assert(teachersTimetable(tch,d,h+dd)==ai);
					teachersTimetable(tch,d,h+dd)=-1;
				}
			}
	
			foreach(int tch, mustComputeTimetableTeachers[ai]){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newTeachersTimetable(tch,d,h+dd)==ai);
					newTeachersTimetable(tch,d,h+dd)=-1;
				}
			}

			updateTeachersNHoursGaps(act, ai, d);
		
			//update teachers' list of activities for each day
			/////////////////
			foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
				int tt=teacherActivitiesOfTheDay(tch,d).removeAll(ai);
				assert(tt==1);
			}
			/////////////////
			
			//2011-09-30
			if(haveActivitiesOccupyOrSimultaneousConstraints){
				for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(activitiesAtTime[t].contains(ai));
					activitiesAtTime[t].remove(ai);
				}
			}
		}
	}
	
	c.times[ai]=toslot;
	c.rooms[ai]=toroom;
	c._fitness=-1;
	c.changedForMatrixCalculation=true;
	
	if(toslot!=UNALLOCATED_TIME){
		int d=toslot%gt.rules.nDaysPerWeek;
		int h=toslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=c.rooms[ai];
		if(rm!=UNSPECIFIED_ROOM && rm!=UNALLOCATED_SPACE)
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				
				assert(rm!=UNALLOCATED_SPACE);
				/*if(roomsTimetable(rm,d,h+dd)>=0){
					cout<<"room is: "<<qPrintable(gt.rules.internalRoomsList[rm]->name)<<endl;
					cout<<"day is "<<qPrintable(gt.rules.daysOfTheWeek[d])<<endl;
					cout<<"hour is "<<qPrintable(gt.rules.hoursOfTheDay[h+dd])<<endl;
					cout<<"roomsTimetable(rm,d,h+dd) is "<<roomsTimetable(rm,d,h+dd)<<endl;
					cout<<"and represents room "<<qPrintable(gt.rules.internalRoomsList[roomsTimetable(rm,d,h+dd)]->name)<<endl;
				}*/
				
				assert(roomsTimetable(rm,d,h+dd)==-1);
				roomsTimetable(rm,d,h+dd)=ai;
			}
		/////////////////////
		
		if(fromslot!=toslot){
			//compute timetable of subgroups
			for(int q=0; q<act->iSubgroupsList.count(); q++){
				int sb=act->iSubgroupsList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(subgroupsTimetable(sb,d,h+dd)==-1);
					subgroupsTimetable(sb,d,h+dd)=ai;
				}
			}
	
			foreach(int sb, mustComputeTimetableSubgroups[ai]){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newSubgroupsTimetable(sb,d,h+dd)==-1);
					newSubgroupsTimetable(sb,d,h+dd)=ai;
				}
			}

			updateSubgroupsNHoursGaps(act, ai, d);

			//update students' list of activities for each day
			/////////////////
			foreach(int st, subgroupsWithMaxDaysPerWeekForActivities[ai]){
				assert(subgroupActivitiesOfTheDay(st,d).indexOf(ai)==-1);
				subgroupActivitiesOfTheDay(st,d).append(ai);
			}
			/////////////////

			//teachers' timetable
			for(int q=0; q<act->iTeachersList.count(); q++){
				int tch=act->iTeachersList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(teachersTimetable(tch,d,h+dd)==-1);
					teachersTimetable(tch,d,h+dd)=ai;
				}
			}
	
			foreach(int tch, mustComputeTimetableTeachers[ai]){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newTeachersTimetable(tch,d,h+dd)==-1);
					newTeachersTimetable(tch,d,h+dd)=ai;
				}
			}

			//////////
			updateTeachersNHoursGaps(act, ai, d);
		
			//update teachers' list of activities for each day
			/////////////////
			foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
				assert(teacherActivitiesOfTheDay(tch,d).indexOf(ai)==-1);
				teacherActivitiesOfTheDay(tch,d).append(ai);
			}
			/////////////////

			//2011-09-30
			if(haveActivitiesOccupyOrSimultaneousConstraints){
				for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(!activitiesAtTime[t].contains(ai));
					activitiesAtTime[t].insert(ai);
				}
			}
		}
	}
}

//faster: (to avoid allocating memory at each call)
#if 1

static double nMinDaysBrokenL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int selectedRoomL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int permL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static QList<int> conflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
//static int conflPermL[MAX_LEVEL][MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
static int nConflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int roomSlotsL[MAX_LEVEL][MAX_HOURS_PER_WEEK];


static int currentLevel;

inline bool compareFunctionGenerate(int i, int j)
{
	if(nConflActivitiesL[currentLevel][i] < nConflActivitiesL[currentLevel][j] ||
	 (nConflActivitiesL[currentLevel][i] == nConflActivitiesL[currentLevel][j] &&
	 nMinDaysBrokenL[currentLevel][i] < nMinDaysBrokenL[currentLevel][j]))
		return true;
	
	return false;
}


#define nMinDaysBroken			(nMinDaysBrokenL[level])
#define selectedRoom			(selectedRoomL[level])
#define perm					(permL[level])
#define conflActivities			(conflActivitiesL[level])
//#define conflPerm				(conflPermL[level])
#define nConflActivities		(nConflActivitiesL[level])
#define roomSlots				(roomSlotsL[level])

#endif

void Generate::randomSwap(int ai, int level){
	//cout<<"level=="<<level<<endl;
	
	if(level==0){
		conflActivitiesTimeSlot.clear();
		timeSlot=-1;

		/*for(int l=0; l<level_limit; l++)
			for(int i=0; i<gt.rules.nHoursPerWeek; i++){
				nMinDaysBrokenL[l][i]=0;
				selectedRoomL[l][i]=-1;
				permL[l][i]=-1;
				conflActivitiesL[l][i].clear();
				conflPermL[l][i]=-1;
				nConflActivitiesL[l][i]=0;
				roomSlotsL[l][i]=-1;
			}*/
	}

	if(level>=level_limit){
		return;
	}
	
	if(ncallsrandomswap>=limitcallsrandomswap)
		return;
		
	/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		nMinDaysBroken[i]=0;
		selectedRoom[i]=-1;
		perm[i]=-1;
		conflActivities[i].clear();
		conflPerm[i]=-1;
		nConflActivities[i]=0;
		roomSlots[i]=-1;
	}*/
		
	ncallsrandomswap++;
	
	Activity* act=&gt.rules.internalActivitiesList[ai];
	
	bool updateSubgroups=(mustComputeTimetableSubgroups[ai].count()>0);
	bool updateTeachers=(mustComputeTimetableTeachers[ai].count()>0);
	
#if 0
	double nMinDaysBroken[MAX_HOURS_PER_WEEK]; //to count for broken min days between activities constraints
	
	int selectedRoom[MAX_HOURS_PER_WEEK];
#endif
		
	//cout<<"ai=="<<ai<<", corresponding to id=="<<gt.rules.internalActivitiesList[ai].id<<", level=="<<level<<endl;

	//generate random permutation in linear time like in CLR (Cormen, Leiserson and Rivest - Introduction to algorithms).
	//this is used to scan times in random order
#if 0
	int perm[MAX_HOURS_PER_WEEK];
#endif

	int activity_count_impossible_tries=1;

again_if_impossible_activity:

	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		perm[i]=i;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int t=perm[i];
		int r=randomKnuth(gt.rules.nHoursPerWeek-i);
		perm[i]=perm[i+r];
		perm[i+r]=t;
	}
	
	/*int checkPerm[MAX_HOURS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		checkPerm[i]=false;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		checkPerm[perm[i]]=true;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		assert(checkPerm[i]==true);*/
	
	/*
	cout<<"Perm: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<", perm["<<i<<"]="<<perm[i];
	cout<<endl;
	*/
	
	//record the conflicting activities for each timeslot
#if 0
	QList<int> conflActivities[MAX_HOURS_PER_WEEK];
	int conflPerm[MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
	int nConflActivities[MAX_HOURS_PER_WEEK];
	
	int roomSlots[MAX_HOURS_PER_WEEK];
#endif

	for(int n=0; n<gt.rules.nHoursPerWeek; n++){
		int newtime=perm[n];
		
		if(c.times[ai]!=UNALLOCATED_TIME){
			if(c.times[ai]!=newtime){
				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}

		nConflActivities[newtime]=0;
		conflActivities[newtime].clear();

		int d=newtime%gt.rules.nDaysPerWeek;
		int h=newtime/gt.rules.nDaysPerWeek;
		
		/*if(updateSubgroups || updateTeachers){
			addAiToNewTimetable(ai, act, d, h);
			if(updateTeachers)
				updateTeachersNHoursGaps(act, ai, d);
			if(updateSubgroups)
				updateSubgroupsNHoursGaps(act, ai, d);
		}*/
		
		nMinDaysBroken[newtime]=0.0;

		bool okbasictime;
		bool okmindays;
		bool okmaxdays;
		bool oksamestartingtime;
		bool oksamestartinghour;
		bool oksamestartingday;
		bool oknotoverlapping;
		bool oktwoactivitiesconsecutive;
		bool oktwoactivitiesgrouped;
		bool okthreeactivitiesgrouped;
		bool oktwoactivitiesordered;
		bool okactivityendsstudentsday;
		bool okstudentsmaxdaysperweek;
		bool okstudentsearlymaxbeginningsatsecondhour;
		bool okstudentsmaxgapsperweek;
		bool okstudentsmaxgapsperday;
		bool okstudentsmaxhoursdaily;
		bool okstudentsmaxhourscontinuously;
		bool okstudentsminhoursdaily;
		bool okstudentsintervalmaxdaysperweek;
		bool okteachermaxdaysperweek;
		bool okteachersintervalmaxdaysperweek;
		bool okteachersmaxgapsperweek;
		bool okteachersmaxgapsperday;
		bool okteachersmaxhoursdaily;
		bool okteachersmaxhourscontinuously;
		bool okteachersminhoursdaily;
		bool okteachersmindaysperweek;
		bool okmingapsbetweenactivities;

		bool okteachersactivitytagmaxhourscontinuously;
		bool okstudentsactivitytagmaxhourscontinuously;

		bool okteachersactivitytagmaxhoursdaily;
		bool okstudentsactivitytagmaxhoursdaily;

		//2011-09-25
		bool okactivitiesoccupymaxtimeslotsfromselection;
		
		//2011-09-30
		bool okactivitiesmaxsimultaneousinselectedtimeslots;
		
		if(c.times[ai]!=UNALLOCATED_TIME)
			goto skip_here_if_already_allocated_in_time;

/////////////////////////////////////////////////////////////////////////////////////////////

		//not too late
		//unneeded code, because notAllowedTimesPercentages(ai,newtime)==100 now
		//you can comment this code, but you cannot put an assert failed, because the test is done in next section (see 13 lines below).
		if(h+act->duration>gt.rules.nHoursPerDay){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed (tch&st not available, break, act(s) preferred time(s))
		if(!skipRandom(notAllowedTimesPercentages(ai,newtime))){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about basic time constraints
		okbasictime=true;

		///////////////////////////////////
		//added in 5.0.0-preview30
		//same teacher?
		for(int dur=0; dur<act->duration; dur++){
			assert(h+dur<gt.rules.nHoursPerDay);
			//for(int q=0; q<act->iTeachersList.count(); q++){
			//	int tch=act->iTeachersList.at(q);
			foreach(int tch, act->iTeachersList){
				if(teachersTimetable(tch,d,h+dur)>=0){
					int ai2=teachersTimetable(tch,d,h+dur);
					assert(ai2!=ai);
				
					//assert(activitiesConflictingPercentage(ai,ai2)==100);
					assert(activitiesConflictingPercentage[ai].value(ai2, -1)==100);
					//if(!skipRandom(activitiesConflictingPercentage(ai,ai2))){
					if(true){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okbasictime=false;
							goto impossiblebasictime;
						}

						if(!conflActivities[newtime].contains(ai2)){
						//if(conflActivities[newtime].indexOf(ai2)==-1){
							//conflActivities[newtime].append(ai2);
							
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
		//same subgroup?
		for(int dur=0; dur<act->duration; dur++){
			assert(h+dur<gt.rules.nHoursPerDay);
			//for(int q=0; q<act->iSubgroupsList.count(); q++){
			//	int sbg=act->iSubgroupsList.at(q);
			foreach(int sbg, act->iSubgroupsList){
				if(subgroupsTimetable(sbg,d,h+dur)>=0){
					int ai2=subgroupsTimetable(sbg,d,h+dur);
					assert(ai2!=ai);
			
					//assert(activitiesConflictingPercentage(ai,ai2)==100);
					assert(activitiesConflictingPercentage[ai].value(ai2, -1)==100);
					//if(!skipRandom(activitiesConflictingPercentage(ai,ai2))){
					if(true){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okbasictime=false;
							goto impossiblebasictime;
						}

						if(!conflActivities[newtime].contains(ai2)){
						//if(conflActivities[newtime].indexOf(ai2)==-1){
							//conflActivities[newtime].append(ai2);

							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
		///////////////////////////////////
				
impossiblebasictime:
		if(!okbasictime){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about min days
		okmindays=true;
		
		for(int i=0; i<minDaysListOfActivities[ai].count(); i++){
			int ai2=minDaysListOfActivities[ai].at(i);
			int md=minDaysListOfMinDays[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				int h2=ai2time/gt.rules.nDaysPerWeek;
				if(md>abs(d-d2)){
					bool okrand=skipRandom(minDaysListOfWeightPercentages[ai].at(i));
					//if(fixedTimeActivity[ai] && minDaysListOfWeightPercentages[ai].at(i)<100.0)
					//	okrand=true;
				
					//broken min days - there is a minDaysBrokenAllowancePercentage% chance to place them adjacent
					
					if(minDaysListOfConsecutiveIfSameDay[ai].at(i)==true){ //must place them adjacent if on same day
						if(okrand && 
						 ( (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d!=d2 ) ){
						 	//nMinDaysBroken[newtime]++;
						 	nMinDaysBroken[newtime]+=minDaysListOfWeightPercentages[ai].at(i)/100.0;
						}
						else{
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							//if(conflActivities[newtime].indexOf(ai2)==-1){
							if(!conflActivities[newtime].contains(ai2)){
								//conflActivities[newtime].append(ai2);

								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}					
					else{ //can place them anywhere
						if(okrand){
						 	//nMinDaysBroken[newtime]++;
						 	nMinDaysBroken[newtime]+=minDaysListOfWeightPercentages[ai].at(i)/100.0;
						}
						else{
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							//if(conflActivities[newtime].indexOf(ai2)==-1){
							if(!conflActivities[newtime].contains(ai2)){
								//conflActivities[newtime].append(ai2);

								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}					
				}
			}
		}
impossiblemindays:
		if(!okmindays){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about max days between activities
		okmaxdays=true;
		
		for(int i=0; i<maxDaysListOfActivities[ai].count(); i++){
			int ai2=maxDaysListOfActivities[ai].at(i);
			int md=maxDaysListOfMaxDays[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				//int h2=ai2time/gt.rules.nDaysPerWeek;
				if(md<abs(d-d2)){
					bool okrand=skipRandom(maxDaysListOfWeightPercentages[ai].at(i));
					if(!okrand){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okmaxdays=false;
							goto impossiblemaxdays;
						}
						
						if(!conflActivities[newtime].contains(ai2)){
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}
impossiblemaxdays:
		if(!okmaxdays){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////
		//care about min gaps between activities
		okmingapsbetweenactivities=true;
		
		for(int i=0; i<minGapsBetweenActivitiesListOfActivities[ai].count(); i++){
			int ai2=minGapsBetweenActivitiesListOfActivities[ai].at(i);
			int mg=minGapsBetweenActivitiesListOfMinGaps[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				int h2=ai2time/gt.rules.nDaysPerWeek;
				int duration2=gt.rules.internalActivitiesList[ai2].duration;
				bool oktmp=true;
				if(d==d2){
					if(h2>=h){
						if(h+act->duration+mg > h2){
							oktmp=false;
						}
					}
					else{
						if(h2+duration2+mg > h){
							oktmp=false;
						}
					}
				}
				
				if(!oktmp){
					bool okrand=skipRandom(minGapsBetweenActivitiesListOfWeightPercentages[ai].at(i));
					
					//if(fixedTimeActivity[ai] && minGapsBetweenActivitiesListOfWeightPercentages[ai].at(i)<100.0)
					//	okrand=true;
					
					if(!okrand){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okmingapsbetweenactivities=false;
							goto impossiblemingapsbetweenactivities;
						}
							
						if(!conflActivities[newtime].contains(ai2)){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}					
				}
			}
		}
impossiblemingapsbetweenactivities:
		if(!okmingapsbetweenactivities){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting time
		oksamestartingtime=true;
		
		for(int i=0; i<activitiesSameStartingTimeActivities[ai].count(); i++){
			int ai2=activitiesSameStartingTimeActivities[ai].at(i);
			double perc=activitiesSameStartingTimePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if(newtime!=c.times[ai2] && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartingtime=false;
						goto impossiblesamestartingtime;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){
						//conflActivities[newtime].append(ai2);

						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}
impossiblesamestartingtime:
		if(!oksamestartingtime){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting hour
		oksamestartinghour=true;
		
		for(int i=0; i<activitiesSameStartingHourActivities[ai].count(); i++){
			int ai2=activitiesSameStartingHourActivities[ai].at(i);
			double perc=activitiesSameStartingHourPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if((newtime/gt.rules.nDaysPerWeek)!=(c.times[ai2]/gt.rules.nDaysPerWeek) && !sR){
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartinghour=false;
						goto impossiblesamestartinghour;
					}
				
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){

						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}
impossiblesamestartinghour:
		if(!oksamestartinghour){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting day
		oksamestartingday=true;
		
		for(int i=0; i<activitiesSameStartingDayActivities[ai].count(); i++){
			int ai2=activitiesSameStartingDayActivities[ai].at(i);
			double perc=activitiesSameStartingDayPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if((newtime%gt.rules.nDaysPerWeek)!=(c.times[ai2]%gt.rules.nDaysPerWeek) && !sR){
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartingday=false;
						goto impossiblesamestartingday;
					}
				
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){

						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}
impossiblesamestartingday:
		if(!oksamestartingday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from not overlapping
		oknotoverlapping=true;
		
		for(int i=0; i<activitiesNotOverlappingActivities[ai].count(); i++){
			int ai2=activitiesNotOverlappingActivities[ai].at(i);
			double perc=activitiesNotOverlappingPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				//int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				if(d==d2){
					int st=newtime;
					int en=st+gt.rules.nDaysPerWeek*act->duration;
					int st2=c.times[ai2];
					int en2=st2+gt.rules.nDaysPerWeek*gt.rules.internalActivitiesList[ai2].duration;
					
					bool sR=skipRandom(perc);
					//if(fixedTimeActivity[ai] && perc<100.0)
					//	sR=true;
					
					if(!(en<=st2 || en2<=st) && !sR){
						assert(ai2!=ai);
						
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							oknotoverlapping=false;
							goto impossiblenotoverlapping;
						}
						
						if(!conflActivities[newtime].contains(ai2)){
						//if(conflActivities[newtime].indexOf(ai2)==-1){
							//conflActivities[newtime].append(ai2);

							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossiblenotoverlapping:
		if(!oknotoverlapping){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities consecutive
		oktwoactivitiesconsecutive=true;
		
		for(int i=0; i<constrTwoActivitiesConsecutiveActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesConsecutiveActivities[ai].at(i);
			double perc=constrTwoActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h+act->duration > h2)
					ok=false;
				else{
					assert(d2==d);
					int kk;
					for(kk=h+act->duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h2);
					if(kk!=h2)
						ok=false;
				}
				
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesconsecutive=false;
						goto impossibletwoactivitiesconsecutive;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){

						conflActivities[newtime].append(ai2);
						//conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstrTwoActivitiesConsecutiveActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstrTwoActivitiesConsecutiveActivities[ai].at(i);
			double perc=inverseConstrTwoActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h2+gt.rules.internalActivitiesList[ai2].duration > h)
					ok=false;
				else{
					assert(d2==d);
					int kk;
					for(kk=h2+gt.rules.internalActivitiesList[ai2].duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h);
					if(kk!=h)
						ok=false;
				}
				
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesconsecutive=false;
						goto impossibletwoactivitiesconsecutive;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						//conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}
		
impossibletwoactivitiesconsecutive:
		if(!oktwoactivitiesconsecutive){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities grouped
		oktwoactivitiesgrouped=true;
		
		for(int i=0; i<constrTwoActivitiesGroupedActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesGroupedActivities[ai].at(i);
			double perc=constrTwoActivitiesGroupedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				bool ok=true;
				
				if(d2!=d){
					ok=false;
				}
				else if(d==d2 && h2+gt.rules.internalActivitiesList[ai2].duration <= h){
					int kk;
					for(kk=h2+gt.rules.internalActivitiesList[ai2].duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d2,kk))
							break;
					assert(kk<=h);
					if(kk!=h)
						ok=false;
				}
				else if(d==d2 && h+act->duration <= h2){
					int kk;
					for(kk=h+act->duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h2);
					if(kk!=h2)
						ok=false;
				}
				else
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesgrouped=false;
						goto impossibletwoactivitiesgrouped;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){

						conflActivities[newtime].append(ai2);
						//conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}

impossibletwoactivitiesgrouped:
		if(!oktwoactivitiesgrouped){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from three activities grouped
		okthreeactivitiesgrouped=true;
		
		for(int i=0; i<constrThreeActivitiesGroupedActivities[ai].count(); i++){
			int ai2=constrThreeActivitiesGroupedActivities[ai].at(i).first;
			int ai3=constrThreeActivitiesGroupedActivities[ai].at(i).second;
			double perc=constrThreeActivitiesGroupedPercentages[ai].at(i);

			bool sR=skipRandom(perc);
			//if(fixedTimeActivity[ai] && perc<100.0)
			//	sR=true;
			
			int aip1=-1, aip2=-1; //ai placed
			int ainp1=-1, ainp2=-1; //ai not placed
			if(c.times[ai2]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai2))
				ainp1=ai2;
			else
				aip1=ai2;
			if(c.times[ai3]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai3)){
				if(ainp1==-1)
					ainp1=ai3;
				else
					ainp2=ai3;
			}
			else{
				if(aip1==-1)
					aip1=ai3;
				else
					aip2=ai3;
			}
			
			int cnt=0;
			if(ainp1>=0)
				cnt++;
			if(ainp2>=0)
				cnt++;
			if(aip1>=0)
				cnt++;
			if(aip2>=0)
				cnt++;
			assert(cnt==2);
			
			bool ok;
			
			if(aip1==-1){
				//ok - both not placed
				ok=true;
			}
			else if(aip2==-1){
				//only one placed, one not placed
				int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
				int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
				int durp1=gt.rules.internalActivitiesList[aip1].duration;
				
				int hoursBetweenThem=-1;
				
				if(dp1!=d)
					hoursBetweenThem=-1;
				else if(dp1==d && h >= hp1+durp1){
					hoursBetweenThem=0;
					for(int kk=hp1+durp1; kk<h; kk++)
						if(!breakDayHour(d,kk)){
							//check that the working hours are not separated by a break
							//assertion that durp1>0, so the kk-1 >= 0
							if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
								hoursBetweenThem=-1;
								break;
							}

							hoursBetweenThem++;
						}
				}
				else if(dp1==d && hp1 >= h+act->duration){
					hoursBetweenThem=0;
					for(int kk=h+act->duration; kk<hp1; kk++)
						if(!breakDayHour(d,kk)){
							//check that the working hours are not separated by a break
							//assertion that act->duration>0, so the kk-1 >= 0
							if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
								hoursBetweenThem=-1;
								break;
							}

							hoursBetweenThem++;
						}
				}
				else
					hoursBetweenThem=-1;
					
				assert(ainp1>=0);
				if(hoursBetweenThem==0 || hoursBetweenThem==gt.rules.internalActivitiesList[ainp1].duration)
					//OK
					ok=true;
				else
					//not OK
					ok=false;
			}
			else{
				assert(aip1>=0 && aip2>=0);
				//both placed
				int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
				int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
				//int durp1=gt.rules.internalActivitiesList[aip1].duration;
				
				int dp2=c.times[aip2]%gt.rules.nDaysPerWeek;
				int hp2=c.times[aip2]/gt.rules.nDaysPerWeek;
				//int durp2=gt.rules.internalActivitiesList[aip2].duration;
				
				if(dp1==dp2 && dp1==d){
					int ao1=-1, ao2=-1, ao3=-1; //order them, 1 then 2 then 3
					if(h>=hp1 && h>=hp2 && hp2>=hp1){
						ao1=aip1;
						ao2=aip2;
						ao3=ai;
					}
					else if(h>=hp1 && h>=hp2 && hp1>=hp2){
						ao1=aip2;
						ao2=aip1;
						ao3=ai;
					}
					else if(hp1>=h && hp1>=hp2 && h>=hp2){
						ao1=aip2;
						ao2=ai;
						ao3=aip1;
					}
					else if(hp1>=h && hp1>=hp2 && hp2>=h){
						ao1=ai;
						ao2=aip2;
						ao3=aip1;
					}
					else if(hp2>=h && hp2>=hp1 && h>=hp1){
						ao1=aip1;
						ao2=ai;
						ao3=aip2;
					}
					else if(hp2>=h && hp2>=hp1 && hp1>=h){
						ao1=ai;
						ao2=aip1;
						ao3=aip2;
					}
					else
						assert(0);

					int do1;
					int ho1;
					int duro1;

					int do2;
					int ho2;
					int duro2;

					int do3;
					int ho3;
					//int duro3;
					
					if(ao1==ai){
						do1=d;
						ho1=h;
						duro1=act->duration;
					}
					else{
						do1=c.times[ao1]%gt.rules.nDaysPerWeek;
						ho1=c.times[ao1]/gt.rules.nDaysPerWeek;
						duro1=gt.rules.internalActivitiesList[ao1].duration;
					}

					if(ao2==ai){
						do2=d;
						ho2=h;
						duro2=act->duration;
					}
					else{
						do2=c.times[ao2]%gt.rules.nDaysPerWeek;
						ho2=c.times[ao2]/gt.rules.nDaysPerWeek;
						duro2=gt.rules.internalActivitiesList[ao2].duration;
					}

					if(ao3==ai){
						do3=d;
						ho3=h;
						//duro3=act->duration;
					}
					else{
						do3=c.times[ao3]%gt.rules.nDaysPerWeek;
						ho3=c.times[ao3]/gt.rules.nDaysPerWeek;
						//duro3=gt.rules.internalActivitiesList[ao3].duration;
					}
					
					assert(do1==do2 && do1==do3);
					if(ho1+duro1<=ho2 && ho2+duro2<=ho3){
						int hoursBetweenThem=0;
						
						for(int kk=ho1+duro1; kk<ho2; kk++)
							if(!breakDayHour(d,kk))
								hoursBetweenThem++;
						for(int kk=ho2+duro2; kk<ho3; kk++)
							if(!breakDayHour(d,kk))
								hoursBetweenThem++;
						
						if(hoursBetweenThem==0)
							ok=true;
						else
							ok=false;
					}
					else{
						//not OK
						ok=false;
					}
				}
				else{
					//not OK
					ok=false;
				}
			}
			
			bool again;//=false;
			
			if(!ok && !sR){
				int aidisplaced=-1;
			
				if(aip2>=0){ //two placed activities
					again=true;
				
					QList<int> acts;
					
					if(!fixedTimeActivity[aip1] && !swappedActivities[aip1])
						acts.append(aip1);
					if(!fixedTimeActivity[aip2] && !swappedActivities[aip2])
						acts.append(aip2);

					if(acts.count()==0)
						aidisplaced=-1;
					else if(acts.count()==1)
						aidisplaced=acts.at(0);
					else{
						int t;
						if(level==0){
							int optMinWrong=INF;
				
							QList<int> tl;
		
							for(int q=0; q<acts.count(); q++){
								int tta=acts.at(q);
								if(optMinWrong>triedRemovals(tta,c.times[tta])){
								 	optMinWrong=triedRemovals(tta,c.times[tta]);
								}
							}
					
							for(int q=0; q<acts.count(); q++){
								int tta=acts.at(q);
								if(optMinWrong==triedRemovals(tta,c.times[tta]))
									tl.append(q);
							}
				
							assert(tl.size()>=1);
							int mpos=tl.at(randomKnuth(tl.size()));
					
							assert(mpos>=0 && mpos<acts.count());
							t=mpos;
						}
						else{
							t=randomKnuth(acts.count());
						}
						
						aidisplaced=acts.at(t);
					}
				}
				else{
					again=false;
					assert(aip1>=0);
					if(!fixedTimeActivity[aip1] && !swappedActivities[aip1])
						aidisplaced=aip1;
					else
						aidisplaced=-1;
				}
			
				assert(aidisplaced!=ai);
				
				if(aidisplaced==-1){
					okthreeactivitiesgrouped=false;
					goto impossiblethreeactivitiesgrouped;
				}
				if(fixedTimeActivity[aidisplaced] || swappedActivities[aidisplaced]){
					okthreeactivitiesgrouped=false;
					goto impossiblethreeactivitiesgrouped;
				}
				
				assert(!conflActivities[newtime].contains(aidisplaced));
				conflActivities[newtime].append(aidisplaced);
				nConflActivities[newtime]++;
				assert(conflActivities[newtime].count()==nConflActivities[newtime]);
				
				//if !again, everything is OK, because there was one placed activity and it was eliminated
				
				if(again){
					aip1=-1, aip2=-1;
					ainp1=-1, ainp2=-1;
					if(c.times[ai2]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai2))
						ainp1=ai2;
					else
						aip1=ai2;
					if(c.times[ai3]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai3)){
						if(ainp1==-1)
							ainp1=ai3;
						else
							ainp2=ai3;
					}
					else{
						if(aip1==-1)
							aip1=ai3;
						else
							aip2=ai3;
					}

					assert(aip1>=0 && ainp1>=0 && aip2==-1 && ainp2==-1); //only one placed
					
					//again the procedure from above, with 1 placed
					int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
					int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
					int durp1=gt.rules.internalActivitiesList[aip1].duration;
					
					int hoursBetweenThem=-1;
				
					if(dp1!=d)
						hoursBetweenThem=-1;
					else if(dp1==d && h >= hp1+durp1){
						hoursBetweenThem=0;
						for(int kk=hp1+durp1; kk<h; kk++)
							if(!breakDayHour(d,kk)){
								//check that the working hours are not separated by a break
								//assertion that durp1>0, so the kk-1 >= 0
								if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
									hoursBetweenThem=-1;
									break;
								}

								hoursBetweenThem++;
							}
					}
					else if(dp1==d && hp1 >= h+act->duration){
						hoursBetweenThem=0;
						for(int kk=h+act->duration; kk<hp1; kk++)
							if(!breakDayHour(d,kk)){
								//check that the working hours are not separated by a break
								//assertion that act->duration>0, so the kk-1 >= 0
								if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
									hoursBetweenThem=-1;
									break;
								}

								hoursBetweenThem++;
							}
					}
					else
						hoursBetweenThem=-1;
					
					assert(ainp1>=0);
					if(hoursBetweenThem==0 || hoursBetweenThem==gt.rules.internalActivitiesList[ainp1].duration)
						//OK
						ok=true;
					else
						//not OK
						ok=false;
						
					assert(!sR);
					if(!ok){
						aidisplaced=aip1;
						if(fixedTimeActivity[aidisplaced] || swappedActivities[aidisplaced]){
							okthreeactivitiesgrouped=false;
							goto impossiblethreeactivitiesgrouped;
						}
						
						assert(!conflActivities[newtime].contains(aidisplaced));
						conflActivities[newtime].append(aidisplaced);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						//now it is OK, because there were two activities placed and both were eliminated
					}
				} //end if(again)
			}
		}

impossiblethreeactivitiesgrouped:
		if(!okthreeactivitiesgrouped){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities ordered
		oktwoactivitiesordered=true;
		
		for(int i=0; i<constrTwoActivitiesOrderedActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesOrderedActivities[ai].at(i);
			double perc=constrTwoActivitiesOrderedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				bool ok=true;
				
				if(!(d<d2 || (d==d2 && h+act->duration-1<h2)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;

				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesordered=false;
						goto impossibletwoactivitiesordered;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){

						conflActivities[newtime].append(ai2);
						//conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstrTwoActivitiesOrderedActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstrTwoActivitiesOrderedActivities[ai].at(i);
			double perc=inverseConstrTwoActivitiesOrderedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				int dur2=gt.rules.internalActivitiesList[ai2].duration;
				bool ok=true;
				
				if(!(d2<d || (d2==d && h2+dur2-1<h)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesordered=false;
						goto impossibletwoactivitiesordered;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
					//if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						//conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}
			}
		}
		
impossibletwoactivitiesordered:
		if(!oktwoactivitiesordered){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from activity ends students day
		okactivityendsstudentsday=true;
		
		if(haveActivityEndsStudentsDay){
			//1. If current activity needs to be at the end
			if(activityEndsStudentsDayPercentages[ai]>=0){
				bool skip=skipRandom(activityEndsStudentsDayPercentages[ai]);
				if(!skip){
					foreach(int sb, act->iSubgroupsList){
					//for(int j=0; j<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); j++){
					//	int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(j);
						for(int hh=h+act->duration; hh<gt.rules.nHoursPerDay; hh++){
							int ai2=subgroupsTimetable(sb,d,hh);
							if(ai2>=0){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsstudentsday=false;
									goto impossibleactivityendsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
								//if(conflActivities[newtime].indexOf(ai2)==-1){
									//conflActivities[newtime].append(ai2);
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
								}
							}
						}
					}
				}
			}

			//2. Check activities which have to be at the end, in the same day with current activity
			foreach(int sb, act->iSubgroupsList){
			//for(int j=0; j<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); j++){
			//	int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(j);
				for(int hh=h-1; hh>=0; hh--){
					int ai2=subgroupsTimetable(sb,d,hh);
					if(ai2>=0)
						if(activityEndsStudentsDayPercentages[ai2]>=0){
							bool skip=skipRandom(activityEndsStudentsDayPercentages[ai2]);
							if(!skip){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsstudentsday=false;
									goto impossibleactivityendsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){	
								//if(conflActivities[newtime].indexOf(ai2)==-1){
									//conflActivities[newtime].append(ai2);
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
								}
							}
						}
				}
			}
		}

impossibleactivityendsstudentsday:
		if(!okactivityendsstudentsday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//////////////////////////////////////////////////
		if(updateSubgroups || updateTeachers){
			addAiToNewTimetable(ai, act, d, h);
			if(updateTeachers)
				updateTeachersNHoursGaps(act, ai, d);
			if(updateSubgroups)
				updateSubgroupsNHoursGaps(act, ai, d);
		}
		//////////////////////////////////////////////////
		

/////////////////////////////////////////////////////////////////////////////////////////////
		
		////////////STUDENTS////////////////
	
/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max days per week constraints
		////////////////////////////BEGIN max days per week for students
		okstudentsmaxdaysperweek=true;
		//foreach(int tch, act->iTeachersList){
		foreach(int st, subgroupsWithMaxDaysPerWeekForActivities[ai]){
			if(skipRandom(subgroupsMaxDaysPerWeekWeightPercentages[st]))
				continue;

			int maxDays=subgroupsMaxDaysPerWeekMaxDays[st];
			assert(maxDays>=0); //the list contains real information
			
			//preliminary test
			int _nOc=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
				//comments taken from teachers constraint:
				//if(newTeachersDayNHours(tch,d2)>0)
				
				//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
				//The order of evaluation of activities is changed,
				//with activities which were moved forward and back again
				//being put at the end.
				//If you do not follow this, you'll get impossible timetables
				//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
				//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
				//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3
								
				if(subgroupActivitiesOfTheDay(st,d2).count()>0 || d2==d)
					_nOc++;
			if(_nOc<=maxDays)
				continue; //OK, preliminary
			
			if(maxDays>=0){
				assert(maxDays>0);
				
				//getTchTimetable(tch, conflActivities[newtime]);
				//tchGetNHoursGaps(tch);

				bool occupiedDay[MAX_DAYS_PER_WEEK];
				bool canEmptyDay[MAX_DAYS_PER_WEEK];
				
				int _minWrong[MAX_DAYS_PER_WEEK];
				int _nWrong[MAX_DAYS_PER_WEEK];
				int _nConflActivities[MAX_DAYS_PER_WEEK];
				int _minIndexAct[MAX_DAYS_PER_WEEK];
				
				QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					if(d2==d)
						continue;
				
					occupiedDay[d2]=false;
					canEmptyDay[d2]=true;
					
					_minWrong[d2]=INF;
					_nWrong[d2]=0;
					_nConflActivities[d2]=0;
					_minIndexAct[d2]=gt.rules.nInternalActivities;
					_activitiesForDay[d2].clear();
					
					foreach(int ai2, subgroupActivitiesOfTheDay(st,d2)){
						if(ai2>=0){
							if(!conflActivities[newtime].contains(ai2)){
								occupiedDay[d2]=true;
								if(fixedTimeActivity[ai2] || swappedActivities[ai2])
									canEmptyDay[d2]=false;
								else if(!_activitiesForDay[d2].contains(ai2)){
									_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
									_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
									_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
									_nConflActivities[d2]++;
									_activitiesForDay[d2].append(ai2);
									assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
								}
							}
						}
					}
					
					if(!occupiedDay[d2])
						canEmptyDay[d2]=false;
				}
				occupiedDay[d]=true;
				canEmptyDay[d]=false;
				
				int nOc=0;
				bool canChooseDay=false;
				
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(occupiedDay[j]){
						nOc++;
						if(canEmptyDay[j]){
							canChooseDay=true;
						}
					}
						
				if(nOc>maxDays){
					assert(nOc==maxDays+1);
					
					if(!canChooseDay){
						if(level==0){
							//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
							//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
						}
						okstudentsmaxdaysperweek=false;
						goto impossiblestudentsmaxdaysperweek;
					}
					
					int d2=-1;
					
					if(level!=0){
						//choose random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
						
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk] && m==_nConflActivities[kk])
								candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(randomKnuth(candidateDays.count()));
					}
					else{ //level==0
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk]){
								if(_mW>_minWrong[kk] ||
								 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
							
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
								candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(randomKnuth(candidateDays.count()));
					}
					
					assert(d2>=0);

					assert(_activitiesForDay[d2].count()>0);

					foreach(int ai2, _activitiesForDay[d2]){
						assert(ai2!=ai);
						assert(!swappedActivities[ai2]);
						assert(!fixedTimeActivity[ai2]);
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}		
			}
		}
impossiblestudentsmaxdaysperweek:
		if(!okstudentsmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max days per week

/////////////////////////////////////////////////////////////////////////////////////////////
	
		//BEGIN students interval max days per week
		okstudentsintervalmaxdaysperweek=true;
		foreach(int sbg, act->iSubgroupsList){
			double perc=-1.0;
			for(int cnt=0; cnt<3; cnt++){
				if(cnt==0){
					perc=subgroupsIntervalMaxDaysPerWeekPercentages1[sbg];
				}
				else if(cnt==1){
					perc=subgroupsIntervalMaxDaysPerWeekPercentages2[sbg];
				}
				else if(cnt==2){
					perc=subgroupsIntervalMaxDaysPerWeekPercentages3[sbg];
				}
				else
					assert(0);
				
				if(perc>=0){
					int maxDays=-1;
					int sth=-1;
					int endh=-1;
				
					if(cnt==0){
						maxDays=subgroupsIntervalMaxDaysPerWeekMaxDays1[sbg];
						sth=subgroupsIntervalMaxDaysPerWeekIntervalStart1[sbg];
						endh=subgroupsIntervalMaxDaysPerWeekIntervalEnd1[sbg];
					}
					else if(cnt==1){
						maxDays=subgroupsIntervalMaxDaysPerWeekMaxDays2[sbg];
						sth=subgroupsIntervalMaxDaysPerWeekIntervalStart2[sbg];
						endh=subgroupsIntervalMaxDaysPerWeekIntervalEnd2[sbg];
					}
					else if(cnt==2){
						maxDays=subgroupsIntervalMaxDaysPerWeekMaxDays3[sbg];
						sth=subgroupsIntervalMaxDaysPerWeekIntervalStart3[sbg];
						endh=subgroupsIntervalMaxDaysPerWeekIntervalEnd3[sbg];
					}
					else
						assert(0);

					assert(sth>=0 && sth<gt.rules.nHoursPerDay);
					assert(endh>sth && endh<=gt.rules.nHoursPerDay);
					assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek);
					
					if(skipRandom(perc))
						continue;
					
					assert(perc==100.0);
					
					bool foundothers=false;
					bool foundai=false;
					for(int hh=sth; hh<endh; hh++){
						if(newSubgroupsTimetable(sbg,d,hh)==ai){
							foundai=true;
						}
						else{
							assert(newSubgroupsTimetable(sbg,d,hh)==subgroupsTimetable(sbg,d,hh));
							if(newSubgroupsTimetable(sbg,d,hh)>=0){
								if(!conflActivities[newtime].contains(newSubgroupsTimetable(sbg,d,hh))){
									foundothers=true;
								}
							}
						}
					}
					int nrotherdays=0;
					for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++){
						if(dd!=d){
							for(int hh=sth; hh<endh; hh++){
								assert(newSubgroupsTimetable(sbg,dd,hh)==subgroupsTimetable(sbg,dd,hh));
								if(newSubgroupsTimetable(sbg,dd,hh)>=0 && !conflActivities[newtime].contains(newSubgroupsTimetable(sbg,dd,hh))){
									nrotherdays++;
									break;
								}
							}
						}
					}
					assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
					if((foundai && !foundothers) && nrotherdays==maxDays){
						//increased above limit
						bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						int _minWrong[MAX_DAYS_PER_WEEK];
						int _nWrong[MAX_DAYS_PER_WEEK];
						int _nConflActivities[MAX_DAYS_PER_WEEK];
						int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=subgroupsTimetable(sbg,d2,h2);
							//foreach(int ai2, teacherActivitiesOfTheDay(tch,d2)){
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						//if(nOc>maxDays){
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsintervalmaxdaysperweek=false;
							goto impossiblestudentsintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
						
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(randomKnuth(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
								
							assert(candidateDays.count()>0);
							d2=candidateDays.at(randomKnuth(candidateDays.count()));
						}
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						foreach(int ai2, _activitiesForIntervalDay[d2]){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
		//respecting students interval max days per week
impossiblestudentsintervalmaxdaysperweek:
		if(!okstudentsintervalmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students interval max days per week


/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking students early max beginnings at second hour
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsearlymaxbeginningsatsecondhour=true;
		
		foreach(int sbg, act->iSubgroupsList)
			if(!skipRandom(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg])){
				//preliminary check
				int _nHours=0;
				int _nFirstGapsOne=0;
				int _nFirstGapsTwo=0;
				int _nGaps=0;
				int _nIllegalGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newSubgroupsDayNHours(sbg,d2);
					
					if(newSubgroupsDayNFirstGaps(sbg, d2)==1){
						_nFirstGapsOne++;
					}
					else if(newSubgroupsDayNFirstGaps(sbg, d2)>=2){
						_nFirstGapsTwo++;
						_nIllegalGaps++;
						_nGaps+=newSubgroupsDayNFirstGaps(sbg, d2)-2;
					}
					_nGaps+=newSubgroupsDayNGaps(sbg,d2);
				}
				
				int _tt=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				if(_tt>=_nFirstGapsOne){
					_tt-=_nFirstGapsOne;
					_nFirstGapsOne=0;
				}
				else{
					_nFirstGapsOne-=_tt;
					_tt=0;
				}
				if(_tt>=_nFirstGapsTwo){
					_tt-=_nFirstGapsTwo;
					_nFirstGapsTwo=0;
				}
				else{
					_nFirstGapsTwo-=_tt;
					_tt=0;
				}
				
				if(_nFirstGapsTwo>0){
					_nGaps+=_nFirstGapsTwo;
					_nFirstGapsTwo=0;
				}
				
				int _nHoursGaps=0;
				if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
					assert(subgroupsMaxGapsPerWeekPercentage[sbg]==100);
					if(_nGaps>subgroupsMaxGapsPerWeekMaxGaps[sbg])
						_nHoursGaps=_nGaps-subgroupsMaxGapsPerWeekMaxGaps[sbg];
				}
				
				if(_nHours + _nFirstGapsOne + _nHoursGaps + _nIllegalGaps > nHoursPerSubgroup[sbg]){
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsearlymaxbeginningsatsecondhour=false;
						goto impossiblestudentsearlymaxbeginningsatsecondhour;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);

					for(;;){
						int nHours=0;
						int nFirstGapsOne=0;
						int nFirstGapsTwo=0;
						int nGaps=0;
						int nIllegalGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=sbgDayNHours[d2];

							if(sbgDayNFirstGaps[d2]==1){
								nFirstGapsOne++;
							}
							else if(sbgDayNFirstGaps[d2]>=2){
								nFirstGapsTwo++;
								nIllegalGaps++;
								nGaps+=sbgDayNFirstGaps[d2]-2;
							}
							nGaps+=sbgDayNGaps[d2];
						}
						
						int tt=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						if(tt>=nFirstGapsOne){
							tt-=nFirstGapsOne;
							nFirstGapsOne=0;
						}
						else{
							nFirstGapsOne-=tt;
							tt=0;
						}
						if(tt>=nFirstGapsTwo){
							tt-=nFirstGapsTwo;
							nFirstGapsTwo=0;
						}
						else{
							nFirstGapsTwo-=tt;
							tt=0;
						}
						
						if(nFirstGapsTwo>0){
							nGaps+=nFirstGapsTwo;
							nFirstGapsTwo=0;
						}
						
						int nHoursGaps=0;
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							assert(subgroupsMaxGapsPerWeekPercentage[sbg]==100);
							if(nGaps>subgroupsMaxGapsPerWeekMaxGaps[sbg])
								nHoursGaps=nGaps-subgroupsMaxGapsPerWeekMaxGaps[sbg];
						}
				
						int ai2=-1;
						
						if(nHours + nFirstGapsOne + nHoursGaps + nIllegalGaps > nHoursPerSubgroup[sbg]){
							//remove an activity
							bool k=subgroupRemoveAnActivityFromEnd(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//this should not be displayed
									//cout<<"WARNING - maybe bug - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsearlymaxbeginningsatsecondhour=false;
								goto impossiblestudentsearlymaxbeginningsatsecondhour;
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);
						
						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossiblestudentsearlymaxbeginningsatsecondhour:
		if(!okstudentsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students early max beginnings at second hour

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking students max gaps per week
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsmaxgapsperweek=true;
		
		foreach(int sbg, act->iSubgroupsList)
			if(!skipRandom(subgroupsMaxGapsPerWeekPercentage[sbg])){
			//TODO
			//if(!skipRandom(subgroupsMaxGapsPerWeekPercentage[sbg])){
				//assert(subgroupsMaxGapsPerWeekPercentage[sbg]==100);
				
				//preliminary test
				int _nHours=0;
				int _nGaps=0;
				int _nFirstGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newSubgroupsDayNHours(sbg,d2);
					_nGaps+=newSubgroupsDayNGaps(sbg,d2);
					_nFirstGaps+=newSubgroupsDayNFirstGaps(sbg,d2);
				}
				
				int _nFirstHours=0;
				
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
					assert(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]==100);
					if(_nFirstGaps>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
						_nFirstHours=_nFirstGaps-subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				}
				
				if(_nGaps+_nHours+_nFirstHours > subgroupsMaxGapsPerWeekMaxGaps[sbg] + nHoursPerSubgroup[sbg]){
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxgapsperweek=false;
						goto impossiblestudentsmaxgapsperweek;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);

					for(;;){
						int nHours=0;
						int nGaps=0;
						int nFirstGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=sbgDayNHours[d2];
							nGaps+=sbgDayNGaps[d2];
							nFirstGaps+=sbgDayNFirstGaps[d2];
						}
						
						int ai2=-1;
							
						int nFirstHours=0;

						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							assert(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]==100);
							if(nFirstGaps>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
								nFirstHours=nFirstGaps-subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						}
						
						if(nGaps+nHours+nFirstHours > subgroupsMaxGapsPerWeekMaxGaps[sbg] + nHoursPerSubgroup[sbg]){
							//remove an activity
							bool k=subgroupRemoveAnActivityFromBeginOrEnd(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxgapsperweek=false;
								goto impossiblestudentsmaxgapsperweek;
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);

						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossiblestudentsmaxgapsperweek:
		if(!okstudentsmaxgapsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max gaps per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//!!!NOT PERFECT constraint, in other places may be improved, like in min/max hours daily.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect

		//not causing more than subgroupsMaxGapsPerDay students gaps
		
		//TODO: improve, check
		
	okstudentsmaxgapsperday=true;
		
	if(haveStudentsMaxGapsPerDay){
		
		//okstudentsmaxgapsperday=true;
		foreach(int sbg, act->iSubgroupsList)
			if(!skipRandom(subgroupsMaxGapsPerDayPercentage[sbg])){
				assert(subgroupsMaxGapsPerDayPercentage[sbg]==100);

				//preliminary test
				int _total=0;
				int _remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				bool _haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_total+=newSubgroupsDayNHours(sbg,d2);
					int _g=newSubgroupsDayNGaps(sbg,d2);
					if(_haveMaxBegs){
						int _fg=newSubgroupsDayNFirstGaps(sbg,d2);
						if(_fg==0){
							if(_g>subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
						else if(_fg==1){
							if(_remnf>0)
								_remnf--;
							else
								_total++;
							if(_g>subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
						else if(_fg>=2){
							if(_g + _fg - 1 <= subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total++;
							else{
								if(_remnf>0)
									_remnf--;
								else
									_total++;
								_total++;
								assert(_g + _fg - 2 >= subgroupsMaxGapsPerDayMaxGaps[sbg]);
								_total+=(_g + _fg - 2 - subgroupsMaxGapsPerDayMaxGaps[sbg]);
							}
						}
						else
							assert(0);
					}
					else{
						if(_g > subgroupsMaxGapsPerDayMaxGaps[sbg])
							_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
					}
				}
				
				if(_total<=nHoursPerSubgroup[sbg]) //OK
					continue;

				if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
					okstudentsmaxgapsperday=false;
					goto impossiblestudentsmaxgapsperday;
				}

				getSbgTimetable(sbg, conflActivities[newtime]);
				sbgGetNHoursGaps(sbg);

				for(;;){
					int total=0;
					int remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
					bool haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						total+=sbgDayNHours[d2];
						int g=sbgDayNGaps[d2];
						if(haveMaxBegs){
							int fg=sbgDayNFirstGaps[d2];
							if(fg==0){
								if(g>subgroupsMaxGapsPerDayMaxGaps[sbg])
									total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
							}
							else if(fg==1){
								if(remnf>0)
									remnf--;
								else
									total++;
								if(g>subgroupsMaxGapsPerDayMaxGaps[sbg])
									total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
							}
							else if(fg>=2){
								if(g + fg - 1 <= subgroupsMaxGapsPerDayMaxGaps[sbg])
									total++;
								else{
									if(remnf>0)
										remnf--;
									else
										total++;
									total++;
									assert(g + fg - 2 >= subgroupsMaxGapsPerDayMaxGaps[sbg]);
									total+=(g + fg - 2 - subgroupsMaxGapsPerDayMaxGaps[sbg]);
								}
							}
							else
								assert(0);
						}
						else{
							if(g > subgroupsMaxGapsPerDayMaxGaps[sbg])
								total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
					}
					
					if(total<=nHoursPerSubgroup[sbg]) //OK
						break;

					//remove an activity from the beginning or from the end of a day
					//following code is identical to maxgapsperweek
					//remove an activity
					int ai2=-1;
					
					//it should also be allowed to take from anywhere, but it is risky to change now
					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						bool k=subgroupRemoveAnActivityFromEnd(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							bool kk;
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 && 
							 (subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0))
								kk=false;
							else
								kk=subgroupRemoveAnActivityFromBegin(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);

							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!kk){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxgapsperday=false;
								goto impossiblestudentsmaxgapsperday;
							}
						}
					}
					else{
						bool k=subgroupRemoveAnActivityFromBeginOrEnd(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxgapsperday=false;
							goto impossiblestudentsmaxgapsperday;
						}
					}
					
					assert(ai2>=0);

					removeAi2FromSbgTimetable(ai2);
					updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
				}
			}
	}
		
impossiblestudentsmaxgapsperday:
		if(!okstudentsmaxgapsperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per day

/////////////////////////////////////////////////////////////////////////////////////////////

		//to be put after max gaps and early!!! because of an assert

		//allowed from students max hours daily
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsmaxhoursdaily=true;
		
		foreach(int sbg, act->iSubgroupsList){
			for(int count=0; count<2; count++){
				int limitHoursDaily;
				double percentage;
				if(count==0){
					limitHoursDaily=subgroupsMaxHoursDailyMaxHours1[sbg];
					percentage=subgroupsMaxHoursDailyPercentages1[sbg];
				}
				else{
					limitHoursDaily=subgroupsMaxHoursDailyMaxHours2[sbg];
					percentage=subgroupsMaxHoursDailyPercentages2[sbg];
				}
				
				if(limitHoursDaily<0)
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
					if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
						//both
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
						  	increased=true;
						else
							increased=false;
					}
					else{
						//only at beginning
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
						  	increased=true;
						else
							increased=false;
					}
				}
				else{
					if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
						//only max gaps
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
						  	increased=true;
						else
							increased=false;
					}
					else{
						//none
						if(oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
						  	increased=true;
						else
							increased=false;
					}
				}
			
				if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
					if(limitHoursDaily<act->duration){
						okstudentsmaxhoursdaily=false;
						goto impossiblestudentsmaxhoursdaily;
					}
					
					if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 && subgroupsMaxGapsPerWeekMaxGaps[sbg]==0){
					  	if(newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d) > limitHoursDaily){
					  		okstudentsmaxhoursdaily=false;
							goto impossiblestudentsmaxhoursdaily;
						}
						else //OK
							continue;
					}
					
					//////////////////////////new
					bool _ok;
					if(newSubgroupsDayNHours(sbg,d)>limitHoursDaily){
						_ok=false; //trivially
					}
					//basically, see that the gaps are enough
					else{
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//both
								int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newSubgroupsDayNHours(sbg,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newSubgroupsDayNFirstGaps(sbg,d2)+newSubgroupsDayNGaps(sbg,d2)-g;
										if(g>0)
											rg-=g;
									}
								}
								
								if(rg<0)
									rg=0;
								
								int hg=newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)-rg;
								if(hg<0)
									hg=0;
									
								if(hg+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								//only max beginnings
								int lateBeginnings=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										if(newSubgroupsDayNHours(sbg,d2)>=limitHoursDaily && newSubgroupsDayNFirstGaps(sbg,d2)==1)
											lateBeginnings++;
									}
								}
								
								int fg=0, ah=0; //first gaps, added hours
								if(newSubgroupsDayNFirstGaps(sbg,d)==0){
									fg=0;
									ah=0;
								}
								else if(newSubgroupsDayNFirstGaps(sbg,d)==1){
									fg=1;
									ah=0;
									if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
									 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
									 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
										ah+=fg;
									
								}
								else if(newSubgroupsDayNFirstGaps(sbg,d)>=2){
									fg=0;
									ah=1;
								}
								
								if(ah+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
						}
						else{
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//only max gaps
								int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newSubgroupsDayNHours(sbg,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newSubgroupsDayNGaps(sbg,d2)-g;
										if(g>0)
											rg-=g;
									}
								}
								
								if(rg<0)
									rg=0;
								
								int hg=newSubgroupsDayNGaps(sbg,d)-rg;
								if(hg<0)
									hg=0;
									
								if(hg+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								//none
								_ok=true;
							}
						}
					}
					
					/////////////////////////////
					
					//preliminary test
					if(_ok){
						continue;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxhoursdaily=false;
						goto impossiblestudentsmaxhoursdaily;
					}
					
					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);
					
					//theoretically, it should be canTakeFromBegin = true all time and ctfAnywhere = true if max gaps per week is not 0.
					//but practically, I tried these changes and it was 30% slower for a modified german sample (with max gaps per day=1,
					//12 hours per day, removed subacts. pref. times, max hours daily 6 for students).
					bool canTakeFromBegin=(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]!=0); //-1 or >0
					bool canTakeFromEnd=true;
					bool canTakeFromAnywhere=(subgroupsMaxGapsPerWeekMaxGaps[sbg]==-1);
		
					for(;;){
						//////////////////////////new
						bool ok;
						if(sbgDayNHours[d]>limitHoursDaily){
							ok=false;
						}
						else{
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//both
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-sbgDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=sbgDayNFirstGaps[d2]+sbgDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}
									
									if(rg<0)
										rg=0;
									
									int hg=sbgDayNGaps[d]+sbgDayNFirstGaps[d]-rg;
									if(hg<0)
										hg=0;
									
									if(hg+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									//only max beginnings
									int lateBeginnings=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											if(sbgDayNHours[d2]>=limitHoursDaily && sbgDayNFirstGaps[d2]==1)
												lateBeginnings++;
										}
									}
									
									int fg=0, ah=0; //first gaps, added hours
									if(sbgDayNFirstGaps[d]==0){
										fg=0;
										ah=0;
									}
									else if(sbgDayNFirstGaps[d]==1){
										fg=1;
										ah=0;
										if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
										 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
										 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
											ah+=fg;
										
									}
									else if(sbgDayNFirstGaps[d]>=2){
										fg=0;
										ah=1;
									}
								
									if(ah+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
							}
							else{
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//only max gaps
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-sbgDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=sbgDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}
									
									if(rg<0)
										rg=0;
									
									int hg=sbgDayNGaps[d]-rg;
									if(hg<0)
										hg=0;
									
									if(hg+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									//none
									ok=true;
								}
							}
						}
						/////////////////////////////
					
						if(ok){
							break;
						}
						
						int ai2=-1;

						bool kk=false;
						if(canTakeFromEnd)
							kk=subgroupRemoveAnActivityFromEndCertainDay(sbg, d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!kk){
							canTakeFromEnd=false;
							bool k=false;
							if(canTakeFromBegin){
								k=subgroupRemoveAnActivityFromBeginCertainDay(sbg, d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0)
									canTakeFromBegin=false;
							}
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								canTakeFromBegin=false;
								bool ka=false;
								if(canTakeFromAnywhere)
									ka=subgroupRemoveAnActivityFromAnywhereCertainDay(sbg, d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								
								if(!ka){
									if(level==0){
										/*cout<<"subgroup=="<<qPrintable(gt.rules.internalSubgroupsList[sbg]->name)<<endl;
										cout<<"d=="<<d<<endl;
										cout<<"H="<<H<<endl;
										cout<<"Timetable:"<<endl;
										for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
											for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
												cout<<"\t"<<sbgTimetable(d2,h2)<<"\t";
											cout<<endl;
										}*/
									
										//this should not be displayed
										//cout<<"WARNING - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxhoursdaily=false;
									goto impossiblestudentsmaxhoursdaily;
								}
							}
						}
		
						assert(ai2>=0);

						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossiblestudentsmaxhoursdaily:
		if(!okstudentsmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students max hours continuously
		
		okstudentsmaxhourscontinuously=true;
		
		foreach(int sbg, act->iSubgroupsList){
			for(int count=0; count<2; count++){
				int limitHoursCont;
				double percentage;
				if(count==0){
					limitHoursCont=subgroupsMaxHoursContinuouslyMaxHours1[sbg];
					percentage=subgroupsMaxHoursContinuouslyPercentages1[sbg];
				}
				else{
					limitHoursCont=subgroupsMaxHoursContinuouslyMaxHours2[sbg];
					percentage=subgroupsMaxHoursContinuouslyPercentages2[sbg];
				}
				
				if(limitHoursCont<0) //no constraint
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				int h2;
				for(h2=h; h2<h+act->duration; h2++){
					assert(h2<gt.rules.nHoursPerDay);
					if(subgroupsTimetable(sbg,d,h2)==-1)
						break;
				}
				if(h2<h+act->duration)
					increased=true;
				else
					increased=false;
					
				QList<int> removableActs;
					
				int nc=act->duration;
				for(h2=h-1; h2>=0; h2--){
					int ai2=subgroupsTimetable(sbg,d,h2);
					assert(ai2==newSubgroupsTimetable(sbg,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}				
				for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
					int ai2=subgroupsTimetable(sbg,d,h2);
					assert(ai2==newSubgroupsTimetable(sbg,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
										
				if(!increased && percentage==100.0)
					assert(nc<=limitHoursCont);
					
				if(!increased || nc<=limitHoursCont) //OK
					continue;
					
				assert(limitHoursCont>=0);

				if(!skipRandom(percentage) && increased){
					if(act->duration>limitHoursCont){
						okstudentsmaxhourscontinuously=false;
						goto impossiblestudentsmaxhourscontinuously;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxhourscontinuously=false;
						goto impossiblestudentsmaxhourscontinuously;
					}
					
					while(true){
						if(removableActs.count()==0){
							okstudentsmaxhourscontinuously=false;
							goto impossiblestudentsmaxhourscontinuously;
						}
						
						int j=-1;
					
						if(level==0){
							int optMinWrong=INF;
			
							QList<int> tl;

							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
								 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
								}
							}
				
							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
									tl.append(q);
							}
			
							assert(tl.size()>=1);
							j=tl.at(randomKnuth(tl.size()));
			
							assert(j>=0 && j<removableActs.count());
						}
						else{
							j=randomKnuth(removableActs.count());
						}
						
						assert(j>=0);
						
						int ai2=removableActs.at(j);
						
						int t=removableActs.removeAll(ai2);
						assert(t==1);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						////////////
						removableActs.clear();
						
						int nc=act->duration;
						int h2;
						for(h2=h-1; h2>=0; h2--){
							int ai2=subgroupsTimetable(sbg,d,h2);
							assert(ai2==newSubgroupsTimetable(sbg,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
							
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}				
						for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=subgroupsTimetable(sbg,d,h2);
							assert(ai2==newSubgroupsTimetable(sbg,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
						
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else		
								break;
						}
										
						if(nc<=limitHoursCont) //OK
							break;
						////////////
					}
				}
			}
		}
		
impossiblestudentsmaxhourscontinuously:
		if(!okstudentsmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag max hours daily
		
		//!!!NOT PERFECT, there is room for improvement
		
		okstudentsactivitytagmaxhoursdaily=true;
		
		if(haveStudentsActivityTagMaxHoursDaily){
	
			foreach(int sbg, act->iSubgroupsList){
				for(int cnt=0; cnt<subgroupsActivityTagMaxHoursDailyMaxHours[sbg].count(); cnt++){
					int activityTag=subgroupsActivityTagMaxHoursDailyActivityTag[sbg].at(cnt);
				
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursDaily=subgroupsActivityTagMaxHoursDailyMaxHours[sbg].at(cnt);
					double percentage=subgroupsActivityTagMaxHoursDailyPercentage[sbg].at(cnt);

					assert(limitHoursDaily>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					
					int nold=0, nnew=0;
					///////////
					for(int h2=0; h2<h; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(oldSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=oldSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nold++;
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nnew++;
						}
					}
					for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					/////////
					if(nold<nnew)
						increased=true;
					else
						increased=false;
						
					if(percentage==100.0)
						assert(nold<=limitHoursDaily);
					if(!increased && percentage==100.0)
						assert(nnew<=limitHoursDaily);
					
					if(!increased || nnew<=limitHoursDaily) //OK
						continue;
						
					assert(limitHoursDaily>=0);
	
					assert(increased);
					assert(nnew>limitHoursDaily);
					if(!skipRandom(percentage)){
						if(act->duration>limitHoursDaily){
							okstudentsactivitytagmaxhoursdaily=false;
							goto impossiblestudentsactivitytagmaxhoursdaily;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsactivitytagmaxhoursdaily=false;
							goto impossiblestudentsactivitytagmaxhoursdaily;
						}
					
						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);
	
						while(true){
							int ncrt=0;
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(sbgTimetable(d,h2)>=0){
									int ai2=sbgTimetable(d,h2);
									assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
									Activity* act=&gt.rules.internalActivitiesList[ai2];
									if(act->iActivityTagsSet.contains(activityTag))
										ncrt++;
								}
							}
							
							if(ncrt<=limitHoursDaily)
								break;
						
							int ai2=-1;
							
							bool ke=subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(sbg, d, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ke){
								if(level==0){
									//...this is not too good, but hopefully there is no problem
								}
								okstudentsactivitytagmaxhoursdaily=false;
								goto impossiblestudentsactivitytagmaxhoursdaily;
							}
							
							assert(ai2>=0);
							
							assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));
							
							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
			
		}
		
impossiblestudentsactivitytagmaxhoursdaily:
		if(!okstudentsactivitytagmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag max hours continuously
		
		okstudentsactivitytagmaxhourscontinuously=true;
		
		if(haveStudentsActivityTagMaxHoursContinuously){
	
			foreach(int sbg, act->iSubgroupsList){
				for(int cnt=0; cnt<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].count(); cnt++){
					int activityTag=subgroupsActivityTagMaxHoursContinuouslyActivityTag[sbg].at(cnt);
				
					//if(gt.rules.internalActivitiesList[ai].activityTagIndex!=activityTag)
					//	continue;
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursCont=subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].at(cnt);
					double percentage=subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].at(cnt);

					assert(limitHoursCont>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					int h2;
					for(h2=h; h2<h+act->duration; h2++){
						assert(h2<gt.rules.nHoursPerDay);
						if(subgroupsTimetable(sbg,d,h2)==-1)
							break;
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2>=0);
						//if(gt.rules.internalActivitiesList[ai2].activityTagIndex!=activityTag)
						//	break;
						if(!gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag))
							break;
					}
					if(h2<h+act->duration)
						increased=true;
					else
						increased=false;
					
					QList<int> removableActs;
						
					int nc=act->duration;
					for(h2=h-1; h2>=0; h2--){
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2==newSubgroupsTimetable(sbg,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) && 
						 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}				
					for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2==newSubgroupsTimetable(sbg,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) && 
						 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
										
					if(!increased && percentage==100.0)
						assert(nc<=limitHoursCont);
					
					if(!increased || nc<=limitHoursCont) //OK
						continue;
						
					assert(limitHoursCont>=0);
	
					if(!skipRandom(percentage) && increased){
						if(act->duration>limitHoursCont){
							okstudentsactivitytagmaxhourscontinuously=false;
							goto impossiblestudentsactivitytagmaxhourscontinuously;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsactivitytagmaxhourscontinuously=false;
							goto impossiblestudentsactivitytagmaxhourscontinuously;
						}
					
						while(true){
							if(removableActs.count()==0){
								okstudentsactivitytagmaxhourscontinuously=false;
								goto impossiblestudentsactivitytagmaxhourscontinuously;
							}
							
							int j=-1;
						
							if(level==0){
								int optMinWrong=INF;
				
								QList<int> tl;
	
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
									 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
									}
								}
				
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
										tl.append(q);
								}
				
								assert(tl.size()>=1);
								j=tl.at(randomKnuth(tl.size()));
				
								assert(j>=0 && j<removableActs.count());
							}
							else{
								j=randomKnuth(removableActs.count());
							}
						
							assert(j>=0);
							
							int ai2=removableActs.at(j);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
						
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							////////////
							removableActs.clear();
							
							int nc=act->duration;
							int h2;
							for(h2=h-1; h2>=0; h2--){
								int ai2=subgroupsTimetable(sbg,d,h2);
								assert(ai2==newSubgroupsTimetable(sbg,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) && 
								 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
								
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}				
							for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=subgroupsTimetable(sbg,d,h2);
								assert(ai2==newSubgroupsTimetable(sbg,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else		
									break;
							}
											
							if(nc<=limitHoursCont) //OK
								break;
							////////////
						}
					}
				}
			}
			
		}
		
impossiblestudentsactivitytagmaxhourscontinuously:
		if(!okstudentsactivitytagmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin students min hours daily
		
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsminhoursdaily=true;
		
		foreach(int sbg, act->iSubgroupsList){
			if(subgroupsMinHoursDailyMinHours[sbg]>=0){
				assert(subgroupsMinHoursDailyPercentages[sbg]==100);
				
				bool allowEmptyDays=subgroupsMinHoursDailyAllowEmptyDays[sbg];
			
				bool skip=skipRandom(subgroupsMinHoursDailyPercentages[sbg]);
				if(!skip){
					//preliminary test
					bool _ok;
					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							//both limitations
							int remG=0, totalH=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newSubgroupsDayNFirstGaps(sbg,d2)+newSubgroupsDayNGaps(sbg,d2);
								if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
									if(newSubgroupsDayNHours(sbg,d2)<subgroupsMinHoursDailyMinHours[sbg]){
										remGDay-=subgroupsMinHoursDailyMinHours[sbg]-newSubgroupsDayNHours(sbg,d2);
										totalH+=subgroupsMinHoursDailyMinHours[sbg];
									}
									else
										totalH+=newSubgroupsDayNHours(sbg,d2);
								}
								if(remGDay>0)
									remG+=remGDay;
							}
							if((remG+totalH <= nHoursPerSubgroup[sbg]
							  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
							  && (totalH <= nHoursPerSubgroup[sbg]))
							  	_ok=true;
							else
								_ok=false;
						}
						else{
							//only first gaps limitation
							int remG=0, totalH=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=0;
								if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
									if(newSubgroupsDayNHours(sbg,d2)<subgroupsMinHoursDailyMinHours[sbg]){
										remGDay=0;
										totalH+=subgroupsMinHoursDailyMinHours[sbg];
									}
									else{
										totalH+=newSubgroupsDayNHours(sbg,d2);
										if(newSubgroupsDayNFirstGaps(sbg,d2)==0)
											remGDay=0;
										else if(newSubgroupsDayNFirstGaps(sbg,d2)==1)
											remGDay=1;
										else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
											remGDay=0;
											totalH++;
										}
									}
								}
								if(remGDay>0)
									remG+=remGDay;
							}
							if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
							  && (totalH <= nHoursPerSubgroup[sbg]))
							  	_ok=true;
							else
								_ok=false;
						}
					}
					else{
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							//only max gaps per week limitation
							int remG=0, totalH=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newSubgroupsDayNGaps(sbg,d2);
								if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
									if(newSubgroupsDayNHours(sbg,d2)<subgroupsMinHoursDailyMinHours[sbg]){
										remGDay-=subgroupsMinHoursDailyMinHours[sbg]-newSubgroupsDayNHours(sbg,d2);
										totalH+=subgroupsMinHoursDailyMinHours[sbg];
									}
									else
										totalH+=newSubgroupsDayNHours(sbg,d2);
								}
								if(remGDay>0)
									remG+=remGDay;
							}
							if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
							  && (totalH <= nHoursPerSubgroup[sbg]))
							  	_ok=true;
							else
								_ok=false;
						}
						else{
							//no limitation
							int totalH=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
									if(newSubgroupsDayNHours(sbg,d2)<subgroupsMinHoursDailyMinHours[sbg])
										totalH+=subgroupsMinHoursDailyMinHours[sbg];
									else
										totalH+=newSubgroupsDayNHours(sbg,d2);
								}
							}
							if(totalH <= nHoursPerSubgroup[sbg])
							  	_ok=true;
							else
								_ok=false;
						}
					}
					
					if(_ok)
						continue;
				
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsminhoursdaily=false;
						goto impossiblestudentsminhoursdaily;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);
		
					for(;;){
						bool ok;
						////////////////////////////
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//both limitations
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=sbgDayNFirstGaps[d2]+sbgDayNGaps[d2];
									if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
										if(sbgDayNHours[d2]<subgroupsMinHoursDailyMinHours[sbg]){
											remGDay-=subgroupsMinHoursDailyMinHours[sbg]-sbgDayNHours[d2];
											totalH+=subgroupsMinHoursDailyMinHours[sbg];
										}
										else
											totalH+=sbgDayNHours[d2];
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]
								  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
								  	ok=true;
								else
									ok=false;
							}
							else{
								//only first gaps limitation
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=0;
									if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
										if(sbgDayNHours[d2]<subgroupsMinHoursDailyMinHours[sbg]){
											remGDay=0;
											totalH+=subgroupsMinHoursDailyMinHours[sbg];
										}
										else{
											totalH+=sbgDayNHours[d2];
											if(sbgDayNFirstGaps[d2]==0)
												remGDay=0;
											else if(sbgDayNFirstGaps[d2]==1)
												remGDay=1;
											else if(sbgDayNFirstGaps[d2]>=2){
												remGDay=0;
												totalH++;
											}
										}
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
								  	ok=true;
								else
									ok=false;
							}
						}
						else{
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//only max gaps per week limitation
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=sbgDayNGaps[d2];
									if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
										if(sbgDayNHours[d2]<subgroupsMinHoursDailyMinHours[sbg]){
											remGDay-=subgroupsMinHoursDailyMinHours[sbg]-sbgDayNHours[d2];
											totalH+=subgroupsMinHoursDailyMinHours[sbg];
										}
										else
											totalH+=sbgDayNHours[d2];
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
								  	ok=true;
								else
									ok=false;
							}
							else{
								//no limitation
								int totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
										if(sbgDayNHours[d2]<subgroupsMinHoursDailyMinHours[sbg])
											totalH+=subgroupsMinHoursDailyMinHours[sbg];
										else
											totalH+=sbgDayNHours[d2];
									}
								}
								if(totalH <= nHoursPerSubgroup[sbg])
								  	ok=true;
								else
									ok=false;
							}
						}
						////////////////////////////							

						if(ok)
							break; //ok
													
						int ai2=-1;
						
						bool kk=subgroupRemoveAnActivityFromEnd(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!kk){
							bool k=subgroupRemoveAnActivityFromBegin(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=subgroupRemoveAnActivityFromAnywhere(sbg, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								
								if(!ka){
									if(level==0){
										/*cout<<"d=="<<d<<", h=="<<h<<", ai=="<<ai<<endl;
										for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
											for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
												cout<<"\t"<<sbgTimetable(d2,h2);
											cout<<endl;
										}*/
									
										//this should not be displayed
										//cout<<"WARNING - unlikely situation - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsminhoursdaily=false;
									goto impossiblestudentsminhoursdaily;
								}
							}
						}

						assert(ai2>=0);

						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossiblestudentsminhoursdaily:
		if(!okstudentsminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end students(s) min hours daily



/////////////////////////////////////////////////////////////////////////////////////////////


				////////////TEACHERS////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teacher max days per week constraints
		////////////////////////////BEGIN max days per week for teachers
		okteachermaxdaysperweek=true;
		//foreach(int tch, act->iTeachersList){
		foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
			if(skipRandom(teachersMaxDaysPerWeekWeightPercentages[tch]))
				continue;

			int maxDays=teachersMaxDaysPerWeekMaxDays[tch];
			assert(maxDays>=0); //the list contains real information
			
			//preliminary test
			int _nOc=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
				//if(newTeachersDayNHours(tch,d2)>0)
				
				//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
				//The order of evaluation of activities is changed,
				//with activities which were moved forward and back again
				//being put at the end.
				//If you do not follow this, you'll get impossible timetables
				//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
				//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
				//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3
								
				if(teacherActivitiesOfTheDay(tch,d2).count()>0 || d2==d)
					_nOc++;
			if(_nOc<=maxDays)
				continue; //OK, preliminary
			
			if(maxDays>=0){
				assert(maxDays>0);
				
				//getTchTimetable(tch, conflActivities[newtime]);
				//tchGetNHoursGaps(tch);

				bool occupiedDay[MAX_DAYS_PER_WEEK];
				bool canEmptyDay[MAX_DAYS_PER_WEEK];
				
				int _minWrong[MAX_DAYS_PER_WEEK];
				int _nWrong[MAX_DAYS_PER_WEEK];
				int _nConflActivities[MAX_DAYS_PER_WEEK];
				int _minIndexAct[MAX_DAYS_PER_WEEK];
				
				QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					if(d2==d)
						continue;
				
					occupiedDay[d2]=false;
					canEmptyDay[d2]=true;
					
					_minWrong[d2]=INF;
					_nWrong[d2]=0;
					_nConflActivities[d2]=0;
					_minIndexAct[d2]=gt.rules.nInternalActivities;
					_activitiesForDay[d2].clear();
					
					foreach(int ai2, teacherActivitiesOfTheDay(tch,d2)){
						if(ai2>=0){
							if(!conflActivities[newtime].contains(ai2)){
								occupiedDay[d2]=true;
								if(fixedTimeActivity[ai2] || swappedActivities[ai2])
									canEmptyDay[d2]=false;
								else if(!_activitiesForDay[d2].contains(ai2)){
									_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
									_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
									_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
									_nConflActivities[d2]++;
									_activitiesForDay[d2].append(ai2);
									assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
								}
							}
						}
					}
					
					if(!occupiedDay[d2])
						canEmptyDay[d2]=false;
				}
				occupiedDay[d]=true;
				canEmptyDay[d]=false;
				
				int nOc=0;
				bool canChooseDay=false;
				
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(occupiedDay[j]){
						nOc++;
						if(canEmptyDay[j]){
							canChooseDay=true;
						}
					}
						
				if(nOc>maxDays){
					assert(nOc==maxDays+1);
					
					if(!canChooseDay){
						if(level==0){
							//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
							//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
						}
						okteachermaxdaysperweek=false;
						goto impossibleteachermaxdaysperweek;
					}
					
					int d2=-1;
					
					if(level!=0){
						//choose random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
						
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk] && m==_nConflActivities[kk])
								candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(randomKnuth(candidateDays.count()));
					}
					else{ //level==0
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk]){
								if(_mW>_minWrong[kk] ||
								 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
							
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
								candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(randomKnuth(candidateDays.count()));
					}
					
					assert(d2>=0);

					assert(_activitiesForDay[d2].count()>0);

					foreach(int ai2, _activitiesForDay[d2]){
						assert(ai2!=ai);
						assert(!swappedActivities[ai2]);
						assert(!fixedTimeActivity[ai2]);
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
					}
				}		
			}
		}
impossibleteachermaxdaysperweek:
		if(!okteachermaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max days per week

/////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN teachers intervals max days per week

		okteachersintervalmaxdaysperweek=true;
		foreach(int tch, act->iTeachersList){
			double perc=-1.0;
			for(int cnt=0; cnt<3; cnt++){
				if(cnt==0){
					perc=teachersIntervalMaxDaysPerWeekPercentages1[tch];
				}
				else if(cnt==1){
					perc=teachersIntervalMaxDaysPerWeekPercentages2[tch];
				}
				else if(cnt==2){
					perc=teachersIntervalMaxDaysPerWeekPercentages3[tch];
				}
				else
					assert(0);
				
				if(perc>=0){
					int maxDays=-1;
					int sth=-1;
					int endh=-1;

					if(cnt==0){
						maxDays=teachersIntervalMaxDaysPerWeekMaxDays1[tch];
						sth=teachersIntervalMaxDaysPerWeekIntervalStart1[tch];
						endh=teachersIntervalMaxDaysPerWeekIntervalEnd1[tch];
					}
					else if(cnt==1){
						maxDays=teachersIntervalMaxDaysPerWeekMaxDays2[tch];
						sth=teachersIntervalMaxDaysPerWeekIntervalStart2[tch];
						endh=teachersIntervalMaxDaysPerWeekIntervalEnd2[tch];
					}
					else if(cnt==2){
						maxDays=teachersIntervalMaxDaysPerWeekMaxDays3[tch];
						sth=teachersIntervalMaxDaysPerWeekIntervalStart3[tch];
						endh=teachersIntervalMaxDaysPerWeekIntervalEnd3[tch];
					}
					else
						assert(0);
				
					assert(sth>=0 && sth<gt.rules.nHoursPerDay);
					assert(endh>sth && endh<=gt.rules.nHoursPerDay);
					assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek);
					
					if(skipRandom(perc))
						continue;
					
					assert(perc==100.0);
					
					bool foundothers=false;
					bool foundai=false;
					for(int hh=sth; hh<endh; hh++){
						if(newTeachersTimetable(tch,d,hh)==ai){
							foundai=true;
						}
						else{
							assert(newTeachersTimetable(tch,d,hh)==teachersTimetable(tch,d,hh));
							if(newTeachersTimetable(tch,d,hh)>=0){
								if(!conflActivities[newtime].contains(newTeachersTimetable(tch,d,hh))){
									foundothers=true;
								}
							}
						}
					}
					int nrotherdays=0;
					for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++){
						if(dd!=d){
							for(int hh=sth; hh<endh; hh++){
								assert(newTeachersTimetable(tch,dd,hh)==teachersTimetable(tch,dd,hh));
								if(newTeachersTimetable(tch,dd,hh)>=0 && !conflActivities[newtime].contains(newTeachersTimetable(tch,dd,hh))){
									nrotherdays++;
									break;
								}
							}
						}
					}
					assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
					if((foundai && !foundothers) && nrotherdays==maxDays){
						//increased above limit
						bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						int _minWrong[MAX_DAYS_PER_WEEK];
						int _nWrong[MAX_DAYS_PER_WEEK];
						int _nConflActivities[MAX_DAYS_PER_WEEK];
						int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=teachersTimetable(tch,d2,h2);
							//foreach(int ai2, teacherActivitiesOfTheDay(tch,d2)){
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						//if(nOc>maxDays){
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersintervalmaxdaysperweek=false;
							goto impossibleteachersintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
						
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(randomKnuth(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
								if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
								
							assert(candidateDays.count()>0);
							d2=candidateDays.at(randomKnuth(candidateDays.count()));
						}
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						foreach(int ai2, _activitiesForIntervalDay[d2]){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
		//respecting teachers interval max days per week
impossibleteachersintervalmaxdaysperweek:
		if(!okteachersintervalmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerWeek teachers gaps
		okteachersmaxgapsperweek=true;
		foreach(int tch, act->iTeachersList)
			if(!skipRandom(teachersMaxGapsPerWeekPercentage[tch])){
				assert(teachersMaxGapsPerWeekPercentage[tch]==100);
				
				//preliminary test
				int _nHours=0;
				int _nGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newTeachersDayNHours(tch,d2);
					_nGaps+=newTeachersDayNGaps(tch,d2);
				}
				
				if(_nGaps+_nHours > teachersMaxGapsPerWeekMaxGaps[tch]+nHoursPerTeacher[tch]){
			
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxgapsperweek=false;
						goto impossibleteachersmaxgapsperweek;
					}
					
					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int nHours=0;
						int nGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=tchDayNHours[d2];
							nGaps+=tchDayNGaps[d2];
						}
						
						int ai2=-1;
							
						if(nGaps+nHours > teachersMaxGapsPerWeekMaxGaps[tch]+nHoursPerTeacher[tch]){
							//remove an activity
							bool k=teacherRemoveAnActivityFromBeginOrEnd(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxgapsperweek=false;
								goto impossibleteachersmaxgapsperweek;
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossibleteachersmaxgapsperweek:
		if(!okteachersmaxgapsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerDay teachers gaps
		okteachersmaxgapsperday=true;
		foreach(int tch, act->iTeachersList)
			if(!skipRandom(teachersMaxGapsPerDayPercentage[tch])){
				assert(teachersMaxGapsPerDayPercentage[tch]==100);

				//preliminary test
				int _total=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_total+=newTeachersDayNHours(tch,d2);
					if(teachersMaxGapsPerDayMaxGaps[tch]<newTeachersDayNGaps(tch,d2))
						_total+=newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch];
				}
				if(_total<=nHoursPerTeacher[tch]) //OK
					continue;

				if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
					okteachersmaxgapsperday=false;
					goto impossibleteachersmaxgapsperday;
				}

				getTchTimetable(tch, conflActivities[newtime]);
				tchGetNHoursGaps(tch);

				for(;;){
					int total=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						total+=tchDayNHours[d2];
						if(teachersMaxGapsPerDayMaxGaps[tch]<tchDayNGaps[d2])
							total+=tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch];
					}
					if(total<=nHoursPerTeacher[tch]) //OK
						break;
						
					//remove an activity from the beginning or from the end of a day
					//following code is identical to maxgapsperweek
					//remove an activity
					int ai2=-1;
					
					//it should also be allowed to take from anywhere, but it is risky to change now
					bool k=teacherRemoveAnActivityFromBeginOrEnd(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
					assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					if(!k){
						if(level==0){
							//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
							//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
						}
						okteachersmaxgapsperday=false;
						goto impossibleteachersmaxgapsperday;
					}
					
					assert(ai2>=0);

					/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
					int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
					int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
						
					for(int dur2=0; dur2<act2->duration; dur2++){
						assert(tchTimetable(d2,h2+dur2)==ai2);
						tchTimetable(d2,h2+dur2)=-1;
					}*/

					removeAi2FromTchTimetable(ai2);
					updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
				}
			}
		
impossibleteachersmaxgapsperday:
		if(!okteachersmaxgapsperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per day

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours daily
		
		//!!!after max gaps per week and max gaps per day
		
		okteachersmaxhoursdaily=true;
		
		foreach(int tch, act->iTeachersList){
			for(int count=0; count<2; count++){
				int limitHoursDaily;
				double percentage;
				if(count==0){
					limitHoursDaily=teachersMaxHoursDailyMaxHours1[tch];
					percentage=teachersMaxHoursDailyPercentages1[tch];
				}
				else{
					limitHoursDaily=teachersMaxHoursDailyMaxHours2[tch];
					percentage=teachersMaxHoursDailyPercentages2[tch];
				}
				
				if(limitHoursDaily<0)
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				if(teachersMaxGapsPerWeekPercentage[tch]>=0 || teachersMaxGapsPerDayPercentage[tch]>=0){
					if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d) 
					  || newTeachersDayNHours(tch,d)+newTeachersDayNGaps(tch,d) > oldTeachersDayNHours(tch,d)+oldTeachersDayNGaps(tch,d))
					  	increased=true;
					else
						increased=false;
				}
				else{
					if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
					  	increased=true;
					else
						increased=false;
				}
				/*
				if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
				  	increased=true;
				else
					increased=false;*/
			
				if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
					if(limitHoursDaily<act->duration){
						okteachersmaxhoursdaily=false;
						goto impossibleteachersmaxhoursdaily;
					}
					
					//preliminary test

					//basically, see that the gaps are enough
					bool _ok;
					if(newTeachersDayNHours(tch,d)>limitHoursDaily){
						_ok=false;
					}
					else{
						if(teachersMaxGapsPerWeekPercentage[tch]>=0){
							int rg=teachersMaxGapsPerWeekMaxGaps[tch];
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								if(d2!=d){
									int g=limitHoursDaily-newTeachersDayNHours(tch,d2);
									//TODO: if g lower than 0 make g 0
									//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
									g=newTeachersDayNGaps(tch,d2)-g;
									if(g>0)
										rg-=g;
								}
							}
							
							if(rg<0)
								rg=0;
							
							if(teachersMaxGapsPerDayPercentage[tch]>=0)
								if(rg>teachersMaxGapsPerDayMaxGaps[tch])
									rg=teachersMaxGapsPerDayMaxGaps[tch];
									
							int hg=newTeachersDayNGaps(tch,d)-rg;
							if(hg<0)
								hg=0;
								
							if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily){
								_ok=false;
							}
							else
								_ok=true;
						}
						else{
							int rg=newTeachersDayNGaps(tch,d);
							int hg=rg;
							if(teachersMaxGapsPerDayPercentage[tch]>=0)
								if(rg>teachersMaxGapsPerDayMaxGaps[tch])
									rg=teachersMaxGapsPerDayMaxGaps[tch];
							hg-=rg;
							if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily)
								_ok=false;
							else
								_ok=true;
						}
					}

					if(_ok){
						continue;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxhoursdaily=false;
						goto impossibleteachersmaxhoursdaily;
					}
	
					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);
		
					for(;;){
						//basically, see that the gaps are enough
						bool ok;
						if(tchDayNHours[d]>limitHoursDaily){
							ok=false;
						}
						else{
							if(teachersMaxGapsPerWeekPercentage[tch]>=0){
								int rg=teachersMaxGapsPerWeekMaxGaps[tch];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-tchDayNHours[d2];
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=tchDayNGaps[d2]-g;
										if(g>0)
											rg-=g;
									}
								}	
								
								if(rg<0)
									rg=0;
								
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									if(rg>teachersMaxGapsPerDayMaxGaps[tch])
										rg=teachersMaxGapsPerDayMaxGaps[tch];
										
								int hg=tchDayNGaps[d]-rg;
								if(hg<0)
									hg=0;
								
								if(hg+tchDayNHours[d] > limitHoursDaily){
									ok=false;
								}
								else
									ok=true;
							}
							else{
								int rg=tchDayNGaps[d];
								int hg=rg;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									if(rg>teachersMaxGapsPerDayMaxGaps[tch])
										rg=teachersMaxGapsPerDayMaxGaps[tch];
								hg-=rg;
								if(hg+tchDayNHours[d] > limitHoursDaily)
									ok=false;
								else
									ok=true;
							}
						}

						if(ok){
							break;
						}
						
						int ai2=-1;

						bool k=teacherRemoveAnActivityFromBeginOrEndCertainDay(tch, d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							bool ka=teacherRemoveAnActivityFromAnywhereCertainDay(tch, d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ka){
								if(level==0){
									/*cout<<"d=="<<d<<", h=="<<h<<", teacher=="<<qPrintable(gt.rules.internalTeachersList[tch]->name);
									cout<<", ai=="<<ai<<endl;
									for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
											cout<<"\t"<<tchTimetable(d2,h2)<<"\t";
										cout<<endl;
									}
									
									cout<<endl;
									for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
											cout<<"\t"<<newTeachersTimetable(tch,d2,h2)<<"\t";
										cout<<endl;
									}*/
								
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxhoursdaily=false;
								goto impossibleteachersmaxhoursdaily;
							}
						}
		
						assert(ai2>=0);

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossibleteachersmaxhoursdaily:
		if(!okteachersmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours continuously
		
		okteachersmaxhourscontinuously=true;
		
		foreach(int tch, act->iTeachersList){
			for(int count=0; count<2; count++){
				int limitHoursCont;
				double percentage;
				if(count==0){
					limitHoursCont=teachersMaxHoursContinuouslyMaxHours1[tch];
					percentage=teachersMaxHoursContinuouslyPercentages1[tch];
				}
				else{
					limitHoursCont=teachersMaxHoursContinuouslyMaxHours2[tch];
					percentage=teachersMaxHoursContinuouslyPercentages2[tch];
				}
				
				if(limitHoursCont<0) //no constraint
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				int h2;
				for(h2=h; h2<h+act->duration; h2++){
					assert(h2<gt.rules.nHoursPerDay);
					if(teachersTimetable(tch,d,h2)==-1)
						break;
				}
				if(h2<h+act->duration)
					increased=true;
				else
					increased=false;
					
				QList<int> removableActs;
					
				int nc=act->duration;
				for(h2=h-1; h2>=0; h2--){
					int ai2=teachersTimetable(tch,d,h2);
					assert(ai2==newTeachersTimetable(tch,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}				
				for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
					int ai2=teachersTimetable(tch,d,h2);
					/*if(ai2!=newTeachersTimetable(tch,d,h2)){
						cout<<"ai=="<<ai<<", d=="<<d<<", h=="<<h<<endl;
						cout<<"teachersTimetable(tch,d,h2)=="<<teachersTimetable(tch,d,h2)<<endl;
						cout<<"newTeachersTimetable(tch,d,h2)=="<<newTeachersTimetable(tch,d,h2)<<endl;
					}*/
					assert(ai2==newTeachersTimetable(tch,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
										
				if(!increased && percentage==100.0)
					assert(nc<=limitHoursCont);
					
				if(!increased || nc<=limitHoursCont) //OK
					continue;
					
				assert(limitHoursCont>=0);

				if(!skipRandom(percentage) && increased){
					if(act->duration>limitHoursCont){
						okteachersmaxhourscontinuously=false;
						goto impossibleteachersmaxhourscontinuously;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxhourscontinuously=false;
						goto impossibleteachersmaxhourscontinuously;
					}
					
					while(true){
						if(removableActs.count()==0){
							okteachersmaxhourscontinuously=false;
							goto impossibleteachersmaxhourscontinuously;
						}
						
						int j=-1;
					
						if(level==0){
							int optMinWrong=INF;
			
							QList<int> tl;

							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
								 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
								}
							}
				
							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
									tl.append(q);
							}
			
							assert(tl.size()>=1);
							j=tl.at(randomKnuth(tl.size()));
			
							assert(j>=0 && j<removableActs.count());
						}
						else{
							j=randomKnuth(removableActs.count());
						}
						
						assert(j>=0);
						
						int ai2=removableActs.at(j);
						
						int t=removableActs.removeAll(ai2);
						assert(t==1);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						////////////
						removableActs.clear();
						
						int nc=act->duration;
						int h2;
						for(h2=h-1; h2>=0; h2--){
							int ai2=teachersTimetable(tch,d,h2);
							assert(ai2==newTeachersTimetable(tch,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
							
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}				
						for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=teachersTimetable(tch,d,h2);
							assert(ai2==newTeachersTimetable(tch,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
						
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else		
								break;
						}
										
						if(nc<=limitHoursCont) //OK
							break;
						////////////
					}
				}
			}
		}
		
impossibleteachersmaxhourscontinuously:
		if(!okteachersmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag max hours daily

		//!!!NOT PERFECT, there is room for improvement

		okteachersactivitytagmaxhoursdaily=true;
		
		if(haveTeachersActivityTagMaxHoursDaily){
	
			foreach(int tch, act->iTeachersList){
				for(int cnt=0; cnt<teachersActivityTagMaxHoursDailyMaxHours[tch].count(); cnt++){
					int activityTag=teachersActivityTagMaxHoursDailyActivityTag[tch].at(cnt);
				
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursDaily=teachersActivityTagMaxHoursDailyMaxHours[tch].at(cnt);
					double percentage=teachersActivityTagMaxHoursDailyPercentage[tch].at(cnt);

					assert(limitHoursDaily>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					
					int nold=0, nnew=0;
					///////////
					for(int h2=0; h2<h; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(oldTeachersTimetable(tch,d,h2)>=0){
							int ai2=oldTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nold++;
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nnew++;
						}
					}
					for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					/////////
					if(nold<nnew)
						increased=true;
					else
						increased=false;
					
					if(percentage==100.0)
						assert(nold<=limitHoursDaily);
					if(!increased && percentage==100.0)
						assert(nnew<=limitHoursDaily);
					
					if(!increased || nnew<=limitHoursDaily) //OK
						continue;
						
					assert(limitHoursDaily>=0);
	
					assert(increased);
					assert(nnew>limitHoursDaily);
					if(!skipRandom(percentage)){
						if(act->duration>limitHoursDaily){
							okteachersactivitytagmaxhoursdaily=false;
							goto impossibleteachersactivitytagmaxhoursdaily;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersactivitytagmaxhoursdaily=false;
							goto impossibleteachersactivitytagmaxhoursdaily;
						}
					
						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);
	
						while(true){
							int ncrt=0;
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(tchTimetable(d,h2)>=0){
									int ai2=tchTimetable(d,h2);
									assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
									Activity* act=&gt.rules.internalActivitiesList[ai2];
									if(act->iActivityTagsSet.contains(activityTag))
										ncrt++;
								}
							}
							
							if(ncrt<=limitHoursDaily)
								break;
						
							int ai2=-1;
							
							bool ke=teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(tch, d, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ke){
								if(level==0){
									//...this is not too good, but hopefully there is no problem
								}
								okteachersactivitytagmaxhoursdaily=false;
								goto impossibleteachersactivitytagmaxhoursdaily;
							}
							
							assert(ai2>=0);
							
							assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));
							
							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
			
		}
		
impossibleteachersactivitytagmaxhoursdaily:
		if(!okteachersactivitytagmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag max hours continuously
		okteachersactivitytagmaxhourscontinuously=true;
		
		if(haveTeachersActivityTagMaxHoursContinuously){
		
			foreach(int tch, act->iTeachersList){
				for(int cnt=0; cnt<teachersActivityTagMaxHoursContinuouslyMaxHours[tch].count(); cnt++){
					int activityTag=teachersActivityTagMaxHoursContinuouslyActivityTag[tch].at(cnt);
					
					//if(gt.rules.internalActivitiesList[ai].activityTagIndex!=activityTag)
					//	continue;
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursCont=teachersActivityTagMaxHoursContinuouslyMaxHours[tch].at(cnt);
					double percentage=teachersActivityTagMaxHoursContinuouslyPercentage[tch].at(cnt);

					assert(limitHoursCont>=0);
					assert(percentage>=0);
					assert(activityTag>=0/* && activityTag<gt.rules.nInternalActivityTags*/);
					
					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					int h2;
					for(h2=h; h2<h+act->duration; h2++){
						assert(h2<gt.rules.nHoursPerDay);
						if(teachersTimetable(tch,d,h2)==-1)
							break;
						int ai2=teachersTimetable(tch,d,h2);
						//if(gt.rules.internalActivitiesList[ai2].activityTagIndex!=activityTag)
						//	break;
						if(!gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag))
							break;
					}
					if(h2<h+act->duration)
						increased=true;
					else
						increased=false;
					
					QList<int> removableActs;
					
					int nc=act->duration;
					for(h2=h-1; h2>=0; h2--){
						int ai2=teachersTimetable(tch,d,h2);
						assert(ai2==newTeachersTimetable(tch,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}				
					for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						int ai2=teachersTimetable(tch,d,h2);
						assert(ai2==newTeachersTimetable(tch,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
						
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
										
					if(!increased && percentage==100.0)
						assert(nc<=limitHoursCont);
					
					if(!increased || nc<=limitHoursCont) //OK
						continue;
					
					assert(limitHoursCont>=0);

					if(!skipRandom(percentage) && increased){
						if(act->duration>limitHoursCont){
							okteachersactivitytagmaxhourscontinuously=false;
							goto impossibleteachersactivitytagmaxhourscontinuously;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersactivitytagmaxhourscontinuously=false;
							goto impossibleteachersactivitytagmaxhourscontinuously;
						}
						
						while(true){
							if(removableActs.count()==0){
								okteachersactivitytagmaxhourscontinuously=false;
								goto impossibleteachersactivitytagmaxhourscontinuously;
							}
							
							int j=-1;
					
							if(level==0){
								int optMinWrong=INF;
				
								QList<int> tl;

								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
									 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
									}
								}
					
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
										tl.append(q);
								}
				
								assert(tl.size()>=1);
								j=tl.at(randomKnuth(tl.size()));
			
								assert(j>=0 && j<removableActs.count());
							}
							else{
								j=randomKnuth(removableActs.count());
							}
							
							assert(j>=0);
							
							int ai2=removableActs.at(j);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
						
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
							////////////
							removableActs.clear();
						
							int nc=act->duration;
							int h2;
							for(h2=h-1; h2>=0; h2--){
								int ai2=teachersTimetable(tch,d,h2);
								assert(ai2==newTeachersTimetable(tch,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								// gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}				
							for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=teachersTimetable(tch,d,h2);
								assert(ai2==newTeachersTimetable(tch,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								// gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else		
									break;
							}
										
							if(nc<=limitHoursCont) //OK
								break;
							////////////
						}
					}
				}
			}
			
		}
		
impossibleteachersactivitytagmaxhourscontinuously:
		if(!okteachersactivitytagmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min hours daily
		
		//I think it is best to put this routine after max days per week
		
		//Added on 11 September 2009: takes care of teachers min days per week

		okteachersminhoursdaily=true;
		foreach(int tch, act->iTeachersList){
			if(teachersMinHoursDailyMinHours[tch]>=0){
				assert(teachersMinHoursDailyPercentages[tch]==100);
			
				bool skip=skipRandom(teachersMinHoursDailyPercentages[tch]);
				if(!skip){
					//preliminary test
					bool _ok;
					if(teachersMaxGapsPerWeekPercentage[tch]==-1){
						int _reqHours=0;
						int _usedDays=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
							if(newTeachersDayNHours(tch,d2)>0){
								_usedDays++;
								if(teachersMaxGapsPerDayPercentage[tch]==-1){
									_reqHours+=max(newTeachersDayNHours(tch,d2), teachersMinHoursDailyMinHours[tch]);
								}
								else{
									int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
									_reqHours+=max(newTeachersDayNHours(tch,d2)+nh, teachersMinHoursDailyMinHours[tch]);
								}
							}
							
						if(teachersMinDaysPerWeekPercentages[tch]>=0){
							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								_reqHours+=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch];
						}
						
						if(_reqHours <= nHoursPerTeacher[tch])
							_ok=true; //ok
						else
							_ok=false;
					}
					else{
						int remG=0;
						int totalH=0;
						int _usedDays=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							int remGDay=newTeachersDayNGaps(tch,d2);
							int h=newTeachersDayNHours(tch,d2);
							if(h>0){
								_usedDays++;
							}
							int addh;
							if(teachersMaxGapsPerDayPercentage[tch]>=0)
								addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
							else
								addh=0;
							remGDay-=addh;
							assert(remGDay>=0);
							h+=addh;
							if(h>0){
								if(h<teachersMinHoursDailyMinHours[tch]){
									remGDay-=teachersMinHoursDailyMinHours[tch]-h;
									totalH+=teachersMinHoursDailyMinHours[tch];
								}
								else
									totalH+=h;
							}
							if(remGDay>0)
								remG+=remGDay;
						}

						if(teachersMinDaysPerWeekPercentages[tch]>=0){
							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								totalH+=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch];
						}
						
						if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
						  && totalH<=nHoursPerTeacher[tch])
						  	_ok=true;
						else
							_ok=false;
					}
					
					if(_ok)
						continue;
				
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersminhoursdaily=false;
						goto impossibleteachersminhoursdaily;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);
		
					for(;;){
						bool ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(tchDayNHours[d2]>0){
									_usedDays++;
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										_reqHours+=max(tchDayNHours[d2], teachersMinHoursDailyMinHours[tch]);
									}
									else{
										int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
										_reqHours+=max(tchDayNHours[d2]+nh, teachersMinHoursDailyMinHours[tch]);
									}
								}
	
							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_reqHours+=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch];
							}
							
							if(_reqHours <= nHoursPerTeacher[tch])
								ok=true; //ok
							else
								ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=tchDayNGaps[d2];
								int h=tchDayNHours[d2];
								if(h>0)
									_usedDays++;
								int addh;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
								else
									addh=0;
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0){
									if(h<teachersMinHoursDailyMinHours[tch]){
										remGDay-=teachersMinHoursDailyMinHours[tch]-h;
										totalH+=teachersMinHoursDailyMinHours[tch];
									}
									else
										totalH+=h;
								}
								if(remGDay>0)
									remG+=remGDay;
							}
							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									totalH+=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch];
							}
							
							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
							  	ok=true;
							else
								ok=false;
						}
						
						if(ok)
							break;
							
						int ai2=-1;
						
						bool k=teacherRemoveAnActivityFromBeginOrEnd(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							bool ka=teacherRemoveAnActivityFromAnywhere(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ka){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersminhoursdaily=false;
								goto impossibleteachersminhoursdaily;
							}
						}

						assert(ai2>=0);

						/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
						
						for(int dur2=0; dur2<act2->duration; dur2++){
							assert(tchTimetable(d2,h2+dur2)==ai2);
							tchTimetable(d2,h2+dur2)=-1;
						}*/

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossibleteachersminhoursdaily:
		if(!okteachersminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end teacher(s) min hours daily

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min days per week
		
		//Put this routine after min hours daily
		
		//Added on 11 September 2009

		okteachersmindaysperweek=true;
		foreach(int tch, act->iTeachersList){
			if(teachersMinDaysPerWeekMinDays[tch]>=0 && teachersMinHoursDailyMinHours[tch]==-1){ //no need to recheck, if min hours daily is set, because I tested above.
				assert(teachersMinDaysPerWeekPercentages[tch]==100);
			
				bool skip=skipRandom(teachersMinDaysPerWeekPercentages[tch]);
				if(!skip){
					//preliminary test
					bool _ok;
					if(teachersMaxGapsPerWeekPercentage[tch]==-1){
						int _reqHours=0;
						int _usedDays=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
							if(newTeachersDayNHours(tch,d2)>0){
								_usedDays++;
								if(teachersMaxGapsPerDayPercentage[tch]==-1){
									_reqHours+=newTeachersDayNHours(tch,d2);
								}
								else{
									int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
									_reqHours+=newTeachersDayNHours(tch,d2)+nh;
								}
							}
							
						assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
						assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
						int _md=teachersMinDaysPerWeekMinDays[tch];
						assert(_md>=0);
						if(_md>_usedDays)
							_reqHours+=(_md-_usedDays)*1; //one hour per day minimum
						
						if(_reqHours <= nHoursPerTeacher[tch])
							_ok=true; //ok
						else
							_ok=false;
					}
					else{
						int remG=0;
						int totalH=0;
						int _usedDays=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							int remGDay=newTeachersDayNGaps(tch,d2);
							int h=newTeachersDayNHours(tch,d2);
							if(h>0){
								_usedDays++;
							}
							int addh;
							if(teachersMaxGapsPerDayPercentage[tch]>=0)
								addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
							else
								addh=0;
							remGDay-=addh;
							assert(remGDay>=0);
							h+=addh;
							if(h>0)
								totalH+=h;
							if(remGDay>0)
								remG+=remGDay;
						}

						assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
						assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
						int _md=teachersMinDaysPerWeekMinDays[tch];
						assert(_md>=0);
						if(_md>_usedDays)
							totalH+=(_md-_usedDays)*1; //min 1 hour per day
						
						if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
						  && totalH<=nHoursPerTeacher[tch])
						  	_ok=true;
						else
							_ok=false;
					}
					
					if(_ok)
						continue;
				
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmindaysperweek=false;
						goto impossibleteachersmindaysperweek;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);
		
					for(;;){
						bool ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(tchDayNHours[d2]>0){
									_usedDays++;
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										_reqHours+=tchDayNHours[d2];
									}
									else{
										int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
										_reqHours+=tchDayNHours[d2]+nh;
									}
								}
	
							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								_reqHours+=(_md-_usedDays)*1; //min 1 hour for each day
							
							if(_reqHours <= nHoursPerTeacher[tch])
								ok=true; //ok
							else
								ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=tchDayNGaps[d2];
								int h=tchDayNHours[d2];
								if(h>0)
									_usedDays++;
								int addh;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
								else
									addh=0;
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0)
									totalH+=h;
								if(remGDay>0)
									remG+=remGDay;
							}
							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								totalH+=(_md-_usedDays)*1; //min 1 hour each day
							
							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
							  	ok=true;
							else
								ok=false;
						}
						
						if(ok)
							break;
							
						int ai2=-1;
						
						bool k=teacherRemoveAnActivityFromBeginOrEnd(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							bool ka=teacherRemoveAnActivityFromAnywhere(tch, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ka){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmindaysperweek=false;
								goto impossibleteachersmindaysperweek;
							}
						}

						assert(ai2>=0);

						/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
						
						for(int dur2=0; dur2<act2->duration; dur2++){
							assert(tchTimetable(d2,h2+dur2)==ai2);
							tchTimetable(d2,h2+dur2)=-1;
						}*/

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossibleteachersmindaysperweek:
		if(!okteachersmindaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end teacher(s) min days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//2011-09-30
		//care about activities max simultaneous in selected time slots
		//I think it is best to put this after all other major constraints
		//I think it is better to put this before activities occupy max time slots from selection
		if(haveActivitiesOccupyOrSimultaneousConstraints){
			okactivitiesmaxsimultaneousinselectedtimeslots=true;
			
			foreach(ActivitiesMaxSimultaneousInSelectedTimeSlots_item* item, amsistsListForActivity[ai]){
				bool inSpecifiedTimeSlots=false;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t)){
						inSpecifiedTimeSlots=true;
						break;
					}
					
				if(!inSpecifiedTimeSlots)
					continue;
					
				bool correct=true;
			
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					if(item->selectedTimeSlotsSet.contains(t)){
						slotSetOfActivities[t]=activitiesAtTime[t];

						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;

						slotSetOfActivities[t].intersect(item->activitiesSet);

						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;

						slotSetOfActivities[t].subtract(conflActivities[newtime].toSet());
						
						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;
						
						assert(!slotSetOfActivities[t].contains(ai));
						slotSetOfActivities[t].insert(ai);

						correct=false;
					}
				}
				
				if(!correct){
					QList<QSet<int> > candidates;
					QSet<int> allCandidates;
					for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(slotSetOfActivities[t].count() > item->maxSimultaneous){
								QSet<int> tmpSet;
								
								assert(slotSetOfActivities[t].count() == item->maxSimultaneous+1);
								foreach(int ai2, slotSetOfActivities[t])
									if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										tmpSet.insert(ai2);
										if(!allCandidates.contains(ai2))
											allCandidates.insert(ai2);
									}
								
								candidates.append(tmpSet);
							}
						}
					}
					
					foreach(QSet<int> tmpSet, candidates)
						if(tmpSet.count()==0){
							okactivitiesmaxsimultaneousinselectedtimeslots=false;
							goto impossibleactivitiesmaxsimultaneousinselectedtimeslots;
						}
						
					//possible to fix
					while(candidates.count()>0){
						int tc=candidates.count();
	
						int q=randomKnuth(allCandidates.count());
						
						//To keep generation identical on all computers - 2013-01-03
						QList<int> tmpSortedList=allCandidates.toList();
						//qSort(tmpSortedList);
						std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());
						int ai2=tmpSortedList.at(q);
						//int ai2=allCandidates.toList().at(q);
						
						////
						assert(ai2!=ai);
						assert(c.times[ai2]!=UNALLOCATED_TIME);
						assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
					
						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
						////
	
						QList<QSet<int> > newCandidates;
						QSet<int> newAllCandidates;
						
						foreach(QSet<int> tmpSet, candidates)
							if(!tmpSet.contains(ai2)){
								newCandidates.append(tmpSet);
								newAllCandidates.unite(tmpSet);
							}
						
						allCandidates=newAllCandidates;
						candidates=newCandidates;
						
						assert(candidates.count()<tc); //need to have a progress, not an endless loop
					}
				}
			}
			
impossibleactivitiesmaxsimultaneousinselectedtimeslots:
			if(!okactivitiesmaxsimultaneousinselectedtimeslots){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}

			/*foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);*/
		}
	
/////////////////////////////////////////////////////////////////////////////////////////////

		//2011-09-25
		//care about activities max number of occupied time slots from selection
		//I think it is best to put this after all other major constraints
		//I think it is best to put this after activities max simultaneous in selected time slots

		if(haveActivitiesOccupyOrSimultaneousConstraints){
			okactivitiesoccupymaxtimeslotsfromselection=true;
			
			foreach(ActivitiesOccupyMaxTimeSlotsFromSelection_item* item, aomtsListForActivity[ai]){
				//preliminary, for speed
				bool contained=false;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t)){
						contained=true;
						break;
					}
				if(!contained)
					continue;

				int _nOcc=0;
				foreach(int t, item->selectedTimeSlotsList)
					if(activitiesAtTime[t].count()>0)
						_nOcc++;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t))
						if(activitiesAtTime[t].count()==0)
							_nOcc++;
				if(_nOcc<=item->maxOccupiedTimeSlots)
					continue;
				///////
			
				foreach(int t, item->selectedTimeSlotsList){
					slotSetOfActivities[t].clear();
					slotCanEmpty[t]=true;
				}
				
				foreach(int ai2, item->activitiesList){
					if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivities[newtime].contains(ai2)){
						for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								slotSetOfActivities[t].insert(ai2);
								
								if(fixedTimeActivity[ai2] || swappedActivities[ai2])
									slotCanEmpty[t]=false;
							}
						}
					}
					else if(ai2==ai){
						for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								slotSetOfActivities[t].insert(ai);
								slotCanEmpty[t]=false;
							}
						}
					}
				}
				
				int nOccupied=0;
				QSet<int> candidates;
				foreach(int t, item->selectedTimeSlotsList){
					if(slotSetOfActivities[t].count()>0){
						nOccupied++;
						
						if(slotCanEmpty[t])
							candidates.insert(t);
					}
				}
				
				if(nOccupied > item->maxOccupiedTimeSlots){
					int target = nOccupied - item->maxOccupiedTimeSlots;
					while(target>0){
						bool decreased=false;
						
						if(candidates.count()==0){
							okactivitiesoccupymaxtimeslotsfromselection=false;
							goto impossibleactivitiesoccupymaxtimeslotsfromselection;
						}
						int q=randomKnuth(candidates.count());
						
						//To keep generation identical on all computers - 2013-01-03
						QList<int> tmpSortedList=candidates.toList();
						//qSort(tmpSortedList);
						std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());
						int t=tmpSortedList.at(q);
						//int t=candidates.toList().at(q);
						
						QSet<int> tmpSet=slotSetOfActivities[t];
						//To keep generation identical on all computers - 2013-01-03
						QList<int> tmpListFromSet=tmpSet.toList();
						//qSort(tmpListFromSet);
						std::stable_sort(tmpListFromSet.begin(), tmpListFromSet.end());
						//Randomize list
						for(int i=0; i<tmpListFromSet.count(); i++){
							int t=tmpListFromSet.at(i);
							int r=randomKnuth(tmpListFromSet.count()-i);
							tmpListFromSet[i]=tmpListFromSet[i+r];
							tmpListFromSet[i+r]=t;
						}
						
						foreach(int ai2, tmpListFromSet){
							assert(ai2!=ai);
							assert(c.times[ai2]!=UNALLOCATED_TIME);
							assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
							
							for(int tt=c.times[ai2]; tt<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; tt+=gt.rules.nDaysPerWeek)
								if(item->selectedTimeSlotsSet.contains(tt)){
									assert(slotSetOfActivities[tt].contains(ai2));
									slotSetOfActivities[tt].remove(ai2);
									if(slotSetOfActivities[tt].count()==0){
										assert(candidates.contains(tt));
										candidates.remove(tt);
										target--;
										
										decreased=true;
									}
								}
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
						
						assert(decreased);
					}
				}
			}
			
impossibleactivitiesoccupymaxtimeslotsfromselection:
			if(!okactivitiesoccupymaxtimeslotsfromselection){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}

			/*foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);*/
				
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

skip_here_if_already_allocated_in_time:

		//////////////////rooms
		bool okroomnotavailable=getRoom(level, act, ai, d, h, roomSlots[newtime], selectedRoom[newtime], conflActivities[newtime], nConflActivities[newtime]);
		
//impossibleroomnotavailable:
		if(!okroomnotavailable){
			if(c.times[ai]==UNALLOCATED_TIME){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
			}
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		///////////////////////

		if(c.times[ai]==UNALLOCATED_TIME){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
		}
		//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

#if 0
		//sort activities in decreasing order of difficulty.
		//if the index of the activity in "permutation" is smaller, the act. is more difficult
		QList<int> sorted;
		QList<int> conflActs=conflActivities[newtime];
		while(conflActs.count()>0){
			int m=gt.rules.nInternalActivities;
			int j=-1;
			for(int k=0; k<conflActs.count(); k++){
				int a=conflActs.at(k);
				if(invPermutation[a]<m){
					m=invPermutation[a];
					j=k;
				}
			}
			assert(j>=0);
			
			sorted.append(conflActs.at(j));
			int a=conflActs.at(j);
			int t=conflActs.removeAll(a);
			assert(t==1);
		}
		assert(sorted.count()==conflActivities[newtime].count());
		conflActivities[newtime]=sorted;
		
		/*
		for(int k=0; k<conflActivities[newtime].count()-1; k++){
			int a1=conflActivities[newtime].at(k);
			int a2=conflActivities[newtime].at(k+1);
			
			int i1, i2;
			for(i1=0; i1<gt.rules.nInternalActivities; i1++)
				if(permutation[i1]==a1)
					break;
			assert(i1<gt.rules.nInternalActivities);
			
			for(i2=0; i2<gt.rules.nInternalActivities; i2++)
				if(permutation[i2]==a2)
					break;
			assert(i2<gt.rules.nInternalActivities);
			
			assert(i1<i2);
		}*/
#endif
		
		///////////////////////////////
		//5.0.0-preview28
		//no conflicting activities for this timeslot - place the activity and return
		
		if(nConflActivities[newtime]==0 && nMinDaysBroken[newtime]==0.0){
			assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai]&&!fixedSpaceActivity[ai]));
			
			if(c.times[ai]!=UNALLOCATED_TIME && fixedTimeActivity[ai] && !fixedSpaceActivity[ai])
				assert(c.times[ai]==newtime);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		///////////////////////////////
		


		assert(nConflActivities[newtime]==conflActivities[newtime].count());
	}
	

	//for(int i=0; i<gt.rules.nHoursPerWeek; i++)
	//	conflPerm[perm[i]]=perm[i];
		
	//DEPRECATED
	//Sorting - O(n^2) - should be improved?
	//The sorting below is not stable (I hope I am not mistaking) - but this should not be a problem.
/*	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		for(int j=i+1; j<gt.rules.nHoursPerWeek; j++)
			if(nConflActivities[conflPerm[perm[i]]]>nConflActivities[conflPerm[perm[j]]]
			 || (nConflActivities[conflPerm[perm[i]]]==nConflActivities[conflPerm[perm[j]]] 
			 && nMinDaysBroken[conflPerm[perm[i]]]>nMinDaysBroken[conflPerm[perm[j]]] )){
				int t=conflPerm[perm[i]];
				conflPerm[perm[i]]=conflPerm[perm[j]];
				conflPerm[perm[j]]=t;
			}*/
			
	//O(n*log(n)) stable sorting
	currentLevel=level;
	//qStableSort(perm+0, perm+gt.rules.nHoursPerWeek, compareFunctionGenerate);
	std::stable_sort(perm+0, perm+gt.rules.nHoursPerWeek, compareFunctionGenerate);
			
	/*cout<<"perm[i]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<perm[i]<<" ";
	cout<<endl;
	cout<<"conflPerm[perm[i]]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<conflPerm[perm[i]]<<" ";
	cout<<endl;
	cout<<"nConflActivities[i]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<nConflActivities[i]<<" ";
	cout<<endl;
	assert(0);*/
			
	for(int i=1; i<gt.rules.nHoursPerWeek; i++){
		assert( (nConflActivities[perm[i-1]]<nConflActivities[perm[i]])
		|| ( (nConflActivities[perm[i-1]]==nConflActivities[perm[i]]) &&
		(nMinDaysBroken[perm[i-1]]<=nMinDaysBroken[perm[i]]) ) );
	}

	/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=conflPerm[perm[i]];
		if(nConflActivities[newtime]!=MAX_ACTIVITIES)
			foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);
	}*/

	if(level==0 && (nConflActivities[perm[0]]==MAX_ACTIVITIES)){
		//to check if generation was stopped
if(this->isThreaded){
			myMutex.unlock();
			myMutex.lock();
}
		if(!abortOptimization && activity_count_impossible_tries<MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0){
			activity_count_impossible_tries++;
			goto again_if_impossible_activity;
		}
		else{
			if(VERBOSE){
				cout<<__FILE__<<" line "<<__LINE__<<" - WARNING - after retrying for "<<activity_count_impossible_tries
				<<" times - no possible time slot for activity with id=="<<gt.rules.internalActivitiesList[ai].id<<endl;
			}
		}
	}
		
	if(level==0){
		/*Matrix1D<int> l0nWrong;
		Matrix1D<int> l0minWrong;
		Matrix1D<int> l0minIndexAct;
		l0nWrong.resize(gt.rules.nHoursPerWeek);
		l0minWrong.resize(gt.rules.nHoursPerWeek);
		l0minIndexAct.resize(gt.rules.nHoursPerWeek);*/
		
		/*int nWrong[MAX_HOURS_PER_WEEK];
		int minWrong[MAX_HOURS_PER_WEEK];
		int minIndexAct[MAX_HOURS_PER_WEEK];*/

		for(int i=0; i<gt.rules.nHoursPerWeek; i++){
			l0nWrong[i]=INF;
			l0minWrong[i]=INF;
			l0minIndexAct[i]=gt.rules.nInternalActivities;
		}
		
		QList<int> tim;
		for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			if(nConflActivities[perm[i]]>0 && nConflActivities[perm[i]]<MAX_ACTIVITIES
			 && roomSlots[perm[i]]!=UNALLOCATED_SPACE)
				tim.append(perm[i]);
		if(tim.count()==0 && nConflActivities[perm[0]]==MAX_ACTIVITIES){
			//cout<<__FILE__<<" line "<<__LINE__<<" - WARNING - no possible time slot for activity with id=="<<
			// gt.rules.internalActivitiesList[ai].id<<endl;
			
			impossibleActivity=true;
		}
		if(tim.count()>0){
			foreach(int i, tim){
				int cnt=0;
				int m=gt.rules.nInternalActivities;
				foreach(int aii, conflActivities[i]){
					if(triedRemovals(aii,c.times[aii])>0)
						cnt+=triedRemovals(aii,c.times[aii]);
						
					if(l0minWrong[i]>triedRemovals(aii,c.times[aii]))
						l0minWrong[i]=triedRemovals(aii,c.times[aii]);
						
					int j=invPermutation[aii];
					if(m>j)
						m=j;
				}
				l0nWrong[i]=cnt;
				l0minIndexAct[i]=m;
			}
			
			int optMinIndex=gt.rules.nInternalActivities;
			int optNWrong=INF;
			int optMinWrong=INF;
			int optNConflActs=gt.rules.nInternalActivities;
			int j=-1;
			
			//bool chooseRandom = (randomKnuth()%20 == 0);
			
			foreach(int i, tim){
				//choose a random time out of these with minimum number of wrongly replaced activities
				if(optMinWrong>l0minWrong[i]
				 || (optMinWrong==l0minWrong[i] && optNWrong>l0nWrong[i])
				 || (optMinWrong==l0minWrong[i] && optNWrong==l0nWrong[i] && optNConflActs>nConflActivities[i])
				 || (optMinWrong==l0minWrong[i] && optNWrong==l0nWrong[i] && optNConflActs==nConflActivities[i] && optMinIndex>l0minIndexAct[i])){
					optNWrong=l0nWrong[i];
					optMinWrong=l0minWrong[i];
					optNConflActs=nConflActivities[i];
					optMinIndex=l0minIndexAct[i];
					j=i;
				}
			}
			
			assert(j>=0);
			QList<int> tim2;
			foreach(int i, tim)
				if(optNWrong==l0nWrong[i] && l0minWrong[i]==optMinWrong && optNConflActs==nConflActivities[i] && optMinIndex==l0minIndexAct[i])
					tim2.append(i);
			assert(tim2.count()>0);
			int rnd=randomKnuth(tim2.count());
			j=tim2.at(rnd);

			assert(j>=0);
			timeSlot=j;
			assert(roomSlots[j]!=UNALLOCATED_SPACE);
			roomSlot=roomSlots[j];
			
			//conflActivitiesTimeSlot=conflActivities[timeSlot];
			conflActivitiesTimeSlot.clear();
			foreach(int a, conflActivities[timeSlot])
				conflActivitiesTimeSlot.append(a);
		}
	}

	//int nExplored=0;
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=perm[i]; //the considered time
		if(nConflActivities[newtime]>=MAX_ACTIVITIES)
			break;
		
		assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai]&&!fixedSpaceActivity[ai]));
			
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai]&&!fixedSpaceActivity[ai]));
			
			if(c.times[ai]!=UNALLOCATED_TIME && fixedTimeActivity[ai] && !fixedSpaceActivity[ai])
				assert(c.times[ai]==newtime);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);			
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		else{
			/*foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);*/
				
			if(level==level_limit-1){
				//cout<<"level_limit-1==level=="<<level<<", for activity with id "<<gt.rules.internalActivitiesList[ai].id<<" returning"<<endl;
				foundGoodSwap=false;
				break;
			}
			
			if(ncallsrandomswap>=limitcallsrandomswap){
				foundGoodSwap=false;
				break;
			}
		
			/*
			//sort activities in decreasing order of difficulty.
			//if the index of the activity in "permutation" is smaller, the act. is more difficult
			QList<int> sorted;
			QList<int> conflActs=conflActivities[newtime];
			while(conflActs.count()>0){
				int m=gt.rules.nInternalActivities;
				int j=-1;
				for(int k=0; k<conflActs.count(); k++){
					int a=conflActs.at(k);
					if(invPermutation[a]<m){
						m=invPermutation[a];
						j=k;
					}
				}
				assert(j>=0);
				
				sorted.append(conflActs.at(j));
				int a=conflActs.at(j);
				int t=conflActs.removeAll(a);
				assert(t==1);
			}
			assert(sorted.count()==conflActivities[newtime].count());
			conflActivities[newtime]=sorted;*/
		
			int ok=true;
			//cout<<"LEVEL=="<<level<<", for activity ai with id=="<<gt.rules.internalActivitiesList[ai].id<<", list of conflActivities ids: ";
			foreach(int a, conflActivities[newtime]){
				//cout<<gt.rules.internalActivitiesList[a].id<<" ";
				if(swappedActivities[a]){
					assert(0);
					//cout<<"here ";
					ok=false;
					break;
				}
				assert(!(fixedTimeActivity[a] && fixedSpaceActivity[a]));
			}
			//cout<<endl;
			
			if(!ok){
				assert(0);
				continue;
			}
				
			//////////////place it at a new time
			
			int oldNRestore=nRestore;
			
			////////////////
			QList<int> oldacts;
			QList<int> oldtimes;
			QList<int> oldrooms;
			
			if(1 /*ok*/){
				assert(conflActivities[newtime].size()>0);
				
				foreach(int a, conflActivities[newtime]){
					//cout<<"Level=="<<level<<", conflicting act. id=="<<gt.rules.internalActivitiesList[a].id<<", old time=="<<c.times[a]<<endl;
					
					restoreActIndex[nRestore]=a;
					restoreTime[nRestore]=c.times[a];
					restoreRoom[nRestore]=c.rooms[a];
					nRestore++;
					
					oldacts.append(a);
					oldtimes.append(c.times[a]);
					oldrooms.append(c.rooms[a]);
					assert(c.times[a]!=UNALLOCATED_TIME);
					assert(c.rooms[a]!=UNALLOCATED_SPACE);
					int nt=UNALLOCATED_TIME;
					if(fixedTimeActivity[a]&&!fixedSpaceActivity[a])
						nt=c.times[a];
					//cout<<"level=="<<level<<", unallocating activity with id=="<<gt.rules.internalActivitiesList[a].id<<endl;
					moveActivity(a, c.times[a], nt, c.rooms[a], UNALLOCATED_SPACE);
					
					//swappedActivities[a]=true;
				}
			}
			assert(oldacts.count()==conflActivities[newtime].count());
			assert(oldtimes.count()==conflActivities[newtime].count());
			assert(oldrooms.count()==conflActivities[newtime].count());
			////////////////
			
			int oldtime=c.times[ai];
			int oldroom=c.rooms[ai];
			//if(c.times[ai]!=UNALLOCATED_TIME){
				restoreActIndex[nRestore]=ai;
				restoreTime[nRestore]=oldtime;
				restoreRoom[nRestore]=oldroom;
				nRestore++;
			//}
			
			//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", old time=="<<c.times[ai]<<endl;

			moveActivity(ai, oldtime, newtime, oldroom, selectedRoom[newtime]);
			//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[ai].id<<
			// " goes from time: "<<oldtime<<" to time: "<<newtime<<endl;
			//////////////////
			
			if(1)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=true;

			foundGoodSwap=false;
			
			ok=false;
			if(1){
				assert(conflActivities[newtime].size()>0);
				ok=true;
				
				foreach(int a, conflActivities[newtime]){
					randomSwap(a, level+1);
					if(!foundGoodSwap){
						ok=false;
						break;
					}
					assert(c.times[a]!=UNALLOCATED_TIME);
					assert(foundGoodSwap);
					foundGoodSwap=false;
				}
			}
			
			if(ok){
				foreach(int a, conflActivities[newtime])
					assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.times[ai]!=UNALLOCATED_TIME);
			
				foundGoodSwap=true;
				return;
			}

			/*if(1)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=false;*/
			
			//////////////restore times from the restore list
			for(int j=nRestore-1; j>=oldNRestore; j--){
				//assert(c.times[ai]!=UNALLOCATED_TIME);
				
				int aii=restoreActIndex[j];
				oldtime=restoreTime[j];
				oldroom=restoreRoom[j];
				
				/*if(aii!=ai)
					cout<<"Level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<" should change swapped state from true to false"<<endl;
				else
					cout<<"Level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<" should remain swapped==true"<<endl;
				*/
				
				if(aii!=ai){
					//assert(swappedActivities[aii]);
					swappedActivities[aii]=false;
				}
				else{
					assert(swappedActivities[aii]);
					//swappedActivities[aii]=false;
				}
				
				//assert(oldtime!=UNALLOCATED_TIME);
				
				//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<
				// " restored from time: "<<c.times[aii]<<" to time: "<<oldtime<<endl;
				moveActivity(aii, c.times[aii], oldtime, c.rooms[aii], oldroom);
				
				//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", restoring old time=="<<c.times[ai]<<endl;
				
				//assert(oldtime!=UNALLOCATED_TIME);
			}
			nRestore=oldNRestore;

			//////////////////////////////
			foreach(int a, conflActivities[newtime]){
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.rooms[a]!=UNALLOCATED_SPACE);
				assert(!swappedActivities[a]);
				assert(!(fixedTimeActivity[a] && fixedSpaceActivity[a]));
			}
			//////////////////////////////
			
			assert(!foundGoodSwap);
			
			if(level>=5) //7 also might be used? This is a value found practically.
				return;
		}
	}
}
