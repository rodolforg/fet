/*File generate_pre.cpp
*/

/***************************************************************************
                          generate_pre.cpp  -  description
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
#include "rules.h"
#include "timetable.h"

#include "generate_pre.h"

#include "matrix.h"

#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

#include "messageboxes.h"

#include <QtAlgorithms>
#include <QtGlobal>

#include <QPair>
#include <QSet>
#include <QHash>
#include <QQueue>

extern Timetable gt;

//#include <QApplication>
#ifndef FET_COMMAND_LINE
#include <QProgressDialog>
#include <QMessageBox>
#endif

//extern QApplication* pqapplication;


int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//decreasing difficulty order


/*qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 roomsTimetable[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/


//BASIC TIME
//qint8 activitiesConflictingPercentage[MAX_ACTIVITIES][MAX_ACTIVITIES];
//Matrix2D<qint8> activitiesConflictingPercentage;
Matrix1D<QHash<int, int> > activitiesConflictingPercentage;

//MIN DAYS BETWEEN ACTIVITIES
Matrix1D<QList<int> > minDaysListOfActivities;
Matrix1D<QList<int> > minDaysListOfMinDays;
Matrix1D<QList<double> > minDaysListOfWeightPercentages;
Matrix1D<QList<bool> > minDaysListOfConsecutiveIfSameDay;

//MAX DAYS BETWEEN ACTIVITIES
Matrix1D<QList<int> > maxDaysListOfActivities;
Matrix1D<QList<int> > maxDaysListOfMaxDays;
Matrix1D<QList<double> > maxDaysListOfWeightPercentages;

//MIN GAPS BETWEEN ACTIVITIES
Matrix1D<QList<int> > minGapsBetweenActivitiesListOfActivities;
Matrix1D<QList<int> > minGapsBetweenActivitiesListOfMinGaps;
Matrix1D<QList<double> > minGapsBetweenActivitiesListOfWeightPercentages;

//TCH & ST NOT AVAIL, BREAK, ACT(S) PREFERRED TIME(S)
//double notAllowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];
Matrix2D<double> notAllowedTimesPercentages;

//breaks are no gaps - I keep track of this
//bool breakTime[MAX_HOURS_PER_WEEK];
//bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
Matrix2D<bool> breakDayHour;

//bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
Matrix3D<bool> subgroupNotAvailableDayHour;

//bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
Matrix3D<bool> teacherNotAvailableDayHour;

//STUDENTS MAX GAPS & EARLY
int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS];
double subgroupsEarlyMaxBeginningsAtSecondHourPercentage[MAX_TOTAL_SUBGROUPS];
int subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[MAX_TOTAL_SUBGROUPS];
double subgroupsMaxGapsPerWeekPercentage[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxGapsPerWeekMaxGaps[MAX_TOTAL_SUBGROUPS];

double subgroupsMaxGapsPerDayPercentage[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxGapsPerDayMaxGaps[MAX_TOTAL_SUBGROUPS];
bool haveStudentsMaxGapsPerDay;

//STUDENTS MAX DAYS PER WEEK
int subgroupsMaxDaysPerWeekMaxDays[MAX_TOTAL_SUBGROUPS];
double subgroupsMaxDaysPerWeekWeightPercentages[MAX_TOTAL_SUBGROUPS];
Matrix1D<QList<int> > subgroupsWithMaxDaysPerWeekForActivities;

//TEACHERS MAX DAYS PER WEEK
int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS];
double teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS];
Matrix1D<QList<int> > teachersWithMaxDaysPerWeekForActivities;

/////////////////care for teachers max gaps
int nHoursPerTeacher[MAX_TEACHERS];
double teachersMaxGapsPerWeekPercentage[MAX_TEACHERS];
int teachersMaxGapsPerWeekMaxGaps[MAX_TEACHERS];

double teachersMaxGapsPerDayPercentage[MAX_TEACHERS];
int teachersMaxGapsPerDayMaxGaps[MAX_TEACHERS];

//activities same starting time
Matrix1D<QList<int> > activitiesSameStartingTimeActivities;
Matrix1D<QList<double> > activitiesSameStartingTimePercentages;

//activities same starting hour
Matrix1D<QList<int> > activitiesSameStartingHourActivities;
Matrix1D<QList<double> > activitiesSameStartingHourPercentages;

//activities same starting day
Matrix1D<QList<int> > activitiesSameStartingDayActivities;
Matrix1D<QList<double> > activitiesSameStartingDayPercentages;

//activities not overlapping
Matrix1D<QList<int> > activitiesNotOverlappingActivities;
Matrix1D<QList<double> > activitiesNotOverlappingPercentages;

//teacher(s) max hours daily
//int teachersGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
//int teachersNHoursPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

double teachersMaxHoursDailyPercentages1[MAX_TEACHERS];
int teachersMaxHoursDailyMaxHours1[MAX_TEACHERS];
//int teachersRealGapsPerDay1[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

double teachersMaxHoursDailyPercentages2[MAX_TEACHERS];
int teachersMaxHoursDailyMaxHours2[MAX_TEACHERS];
//int teachersRealGapsPerDay2[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

//teacher(s) max hours continuously
double teachersMaxHoursContinuouslyPercentages1[MAX_TEACHERS];
int teachersMaxHoursContinuouslyMaxHours1[MAX_TEACHERS];

double teachersMaxHoursContinuouslyPercentages2[MAX_TEACHERS];
int teachersMaxHoursContinuouslyMaxHours2[MAX_TEACHERS];

//teacher(s) activity tag max hours daily
bool haveTeachersActivityTagMaxHoursDaily;

Matrix1D<QList<int> > teachersActivityTagMaxHoursDailyMaxHours;
Matrix1D<QList<int> > teachersActivityTagMaxHoursDailyActivityTag;
Matrix1D<QList<double> > teachersActivityTagMaxHoursDailyPercentage;
//(formerly arrays of size MAX_TEACHERS)

//teacher(s) activity tag max hours continuously
bool haveTeachersActivityTagMaxHoursContinuously;

Matrix1D<QList<int> > teachersActivityTagMaxHoursContinuouslyMaxHours;
Matrix1D<QList<int> > teachersActivityTagMaxHoursContinuouslyActivityTag;
Matrix1D<QList<double> > teachersActivityTagMaxHoursContinuouslyPercentage;
//(formerly arrays of size MAX_TEACHERS)

//teacher(s) min hours daily
double teachersMinHoursDailyPercentages[MAX_TEACHERS];
int teachersMinHoursDailyMinHours[MAX_TEACHERS];

//teacher(s) min days per week
double teachersMinDaysPerWeekPercentages[MAX_TEACHERS];
int teachersMinDaysPerWeekMinDays[MAX_TEACHERS];

//students (set) max hours continuously
double subgroupsMaxHoursContinuouslyPercentages1[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursContinuouslyMaxHours1[MAX_TOTAL_SUBGROUPS];

double subgroupsMaxHoursContinuouslyPercentages2[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursContinuouslyMaxHours2[MAX_TOTAL_SUBGROUPS];

//students (set) activity tag max hours daily
bool haveStudentsActivityTagMaxHoursDaily;

Matrix1D<QList<int> > subgroupsActivityTagMaxHoursDailyMaxHours;
Matrix1D<QList<int> > subgroupsActivityTagMaxHoursDailyActivityTag;
Matrix1D<QList<double> > subgroupsActivityTagMaxHoursDailyPercentage;
//(formerly arrays of size MAX_TOTAL_SUBGROUPS)

//students (set) activity tag max hours continuously
bool haveStudentsActivityTagMaxHoursContinuously;

Matrix1D<QList<int> > subgroupsActivityTagMaxHoursContinuouslyMaxHours;
Matrix1D<QList<int> > subgroupsActivityTagMaxHoursContinuouslyActivityTag;
Matrix1D<QList<double> > subgroupsActivityTagMaxHoursContinuouslyPercentage;
//(formerly arrays of size MAX_TOTAL_SUBGROUPS)

//students (set) max hours daily
double subgroupsMaxHoursDailyPercentages1[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursDailyMaxHours1[MAX_TOTAL_SUBGROUPS];

double subgroupsMaxHoursDailyPercentages2[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursDailyMaxHours2[MAX_TOTAL_SUBGROUPS];

//students (set) min hours daily
double subgroupsMinHoursDailyPercentages[MAX_TOTAL_SUBGROUPS];
int subgroupsMinHoursDailyMinHours[MAX_TOTAL_SUBGROUPS];
bool subgroupsMinHoursDailyAllowEmptyDays[MAX_TOTAL_SUBGROUPS];
bool haveStudentsMinHoursDailyAllowEmptyDays;

// 2 activities consecutive
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double> > constrTwoActivitiesConsecutivePercentages;
Matrix1D<QList<int> > constrTwoActivitiesConsecutiveActivities;

//index represents the second activity, value in array represents the first activity
Matrix1D<QList<double> > inverseConstrTwoActivitiesConsecutivePercentages;
Matrix1D<QList<int> > inverseConstrTwoActivitiesConsecutiveActivities;
// 2 activities consecutive

// 2 activities grouped
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double> > constrTwoActivitiesGroupedPercentages;
Matrix1D<QList<int> > constrTwoActivitiesGroupedActivities;

// 3 activities grouped
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double> > constrThreeActivitiesGroupedPercentages;
Matrix1D<QList<QPair<int, int> > > constrThreeActivitiesGroupedActivities;

// 2 activities ordered
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double> > constrTwoActivitiesOrderedPercentages;
Matrix1D<QList<int> > constrTwoActivitiesOrderedActivities;

//index represents the second activity, value in array represents the first activity
Matrix1D<QList<double> > inverseConstrTwoActivitiesOrderedPercentages;
Matrix1D<QList<int> > inverseConstrTwoActivitiesOrderedActivities;
// 2 activities consecutive

double activityEndsStudentsDayPercentages[MAX_ACTIVITIES];
bool haveActivityEndsStudentsDay;


///////BEGIN teachers interval max days per week
double teachersIntervalMaxDaysPerWeekPercentages1[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekMaxDays1[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalStart1[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalEnd1[MAX_TEACHERS];

double teachersIntervalMaxDaysPerWeekPercentages2[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekMaxDays2[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalStart2[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalEnd2[MAX_TEACHERS];

double teachersIntervalMaxDaysPerWeekPercentages3[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekMaxDays3[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalStart3[MAX_TEACHERS];
int teachersIntervalMaxDaysPerWeekIntervalEnd3[MAX_TEACHERS];

//bool computeTeachersIntervalMaxDaysPerWeek();
///////END   teachers interval max days per week


///////BEGIN students interval max days per week
double subgroupsIntervalMaxDaysPerWeekPercentages1[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekMaxDays1[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalStart1[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalEnd1[MAX_TOTAL_SUBGROUPS];

double subgroupsIntervalMaxDaysPerWeekPercentages2[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekMaxDays2[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalStart2[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalEnd2[MAX_TOTAL_SUBGROUPS];

double subgroupsIntervalMaxDaysPerWeekPercentages3[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekMaxDays3[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalStart3[MAX_TOTAL_SUBGROUPS];
int subgroupsIntervalMaxDaysPerWeekIntervalEnd3[MAX_TOTAL_SUBGROUPS];

//bool computeSubgroupsIntervalMaxDaysPerWeek();
///////END   subgroups interval max days per week


////////rooms
//double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK]; //-1 for available
Matrix2D<double> notAllowedRoomTimePercentages; //-1 for available

Matrix1D<QList<PreferredRoomsItem> > activitiesPreferredRoomsList;
bool unspecifiedPreferredRoom[MAX_ACTIVITIES];

Matrix1D<QList<int> > activitiesHomeRoomsHomeRooms;
double activitiesHomeRoomsPercentage[MAX_ACTIVITIES];
bool unspecifiedHomeRoom[MAX_ACTIVITIES];
////////rooms


////////BEGIN buildings
double maxBuildingChangesPerDayForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
int maxBuildingChangesPerDayForStudentsMaxChanges[MAX_TOTAL_SUBGROUPS];
//bool computeMaxBuildingChangesPerDayForStudents();

double minGapsBetweenBuildingChangesForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
int minGapsBetweenBuildingChangesForStudentsMinGaps[MAX_TOTAL_SUBGROUPS];
//bool computeMinGapsBetweenBuildingChangesForStudents();

double maxBuildingChangesPerDayForTeachersPercentages[MAX_TEACHERS];
int maxBuildingChangesPerDayForTeachersMaxChanges[MAX_TEACHERS];
//bool computeMaxBuildingChangesPerDayForTeachers();

double minGapsBetweenBuildingChangesForTeachersPercentages[MAX_TEACHERS];
int minGapsBetweenBuildingChangesForTeachersMinGaps[MAX_TEACHERS];
//bool computeMinGapsBetweenBuildingChangesForTeachers();

double maxBuildingChangesPerWeekForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
int maxBuildingChangesPerWeekForStudentsMaxChanges[MAX_TOTAL_SUBGROUPS];
//bool computeMaxBuildingChangesPerWeekForStudents();

double maxBuildingChangesPerWeekForTeachersPercentages[MAX_TEACHERS];
int maxBuildingChangesPerWeekForTeachersMaxChanges[MAX_TEACHERS];
//bool computeMaxBuildingChangesPerWeekForTeachers();
////////END   buildings


Matrix1D<QList<int> > mustComputeTimetableSubgroups;
Matrix1D<QList<int> > mustComputeTimetableTeachers;
bool mustComputeTimetableSubgroup[MAX_TOTAL_SUBGROUPS];
bool mustComputeTimetableTeacher[MAX_TEACHERS];
//void computeMustComputeTimetableSubgroups();
//void computeMustComputeTimetableTeachers();


//2011-09-25 - Constraint activities occupy max time slots from selection
QList<ActivitiesOccupyMaxTimeSlotsFromSelection_item> aomtsList;
Matrix1D<QList<ActivitiesOccupyMaxTimeSlotsFromSelection_item*> > aomtsListForActivity;
//bool computeActivitiesOccupyMaxTimeSlotsFromSelection(QWidget* parent);

//2011-09-30 - Constraint activities max simultaneous in selected time slots
QList<ActivitiesMaxSimultaneousInSelectedTimeSlots_item> amsistsList;
Matrix1D<QList<ActivitiesMaxSimultaneousInSelectedTimeSlots_item*> > amsistsListForActivity;
//bool computeActivitiesMaxSimultaneousInSelectedTimeSlots(QWidget* parent);

bool haveActivitiesOccupyOrSimultaneousConstraints;

//2012-04-29 - Constraint activities occupy max different rooms
QList<ActivitiesOccupyMaxDifferentRooms_item> aomdrList;
Matrix1D<QList<ActivitiesOccupyMaxDifferentRooms_item*> > aomdrListForActivity;
//bool computeActivitiesOccupyMaxDifferentRooms(QWidget* parent);

//2013-09-14 - Constraint activities same room if consecutive
QList<ActivitiesSameRoomIfConsecutive_item> asricList;
Matrix1D<QList<ActivitiesSameRoomIfConsecutive_item*> > asricListForActivity;
//bool computeActivitiesSameRoomIfConsecutive(QWidget* parent);


#ifndef FET_COMMAND_LINE
extern QString initialOrderOfActivities;
#else
QString initialOrderOfActivities;
#endif

extern int initialOrderOfActivitiesIndices[MAX_ACTIVITIES];


bool fixedTimeActivity[MAX_ACTIVITIES];
bool fixedSpaceActivity[MAX_ACTIVITIES];


static int daysTeacherIsAvailable[MAX_TEACHERS];
static int daysSubgroupIsAvailable[MAX_TOTAL_SUBGROUPS];
static int requestedDaysForTeachers[MAX_TEACHERS];
static int requestedDaysForSubgroups[MAX_TOTAL_SUBGROUPS];
static int nReqForTeacher[MAX_TEACHERS];
static int nReqForSubgroup[MAX_TOTAL_SUBGROUPS];


//used only in homeRoomsAreOk
static int nHoursRequiredForRoom[MAX_ROOMS];
static int nHoursAvailableForRoom[MAX_ROOMS];
/////////////////////////////

/////////used only in sortActivities
static int nIncompatible[MAX_ACTIVITIES];
static double nMinDaysConstraintsBroken[MAX_ACTIVITIES];
static int nRoomsIncompat[MAX_ROOMS];
static double nHoursForRoom[MAX_ROOMS];
static Matrix1D<PreferredRoomsItem> maxPercentagePrefRooms;
static int reprNInc[MAX_ACTIVITIES];
////////////////////////////////////

static int nIncompatibleFromFather[MAX_ACTIVITIES];
int fatherActivityInInitialOrder[MAX_ACTIVITIES];
////////////////////////////////////

inline bool findEquivalentSubgroupsCompareFunction(int i1, int i2)
{
	const QList<int>& a1=gt.rules.internalSubgroupsList[i1]->activitiesForSubgroup;
	const QList<int>& a2=gt.rules.internalSubgroupsList[i2]->activitiesForSubgroup;
	
	if(a1.count()<a2.count()){
		return true;
	}
	else if(a1.count()>a2.count()){
		return false;
	}
	else{
		assert(a1.count()==a2.count());
		for(int i=0; i<a1.count(); i++){
			if(a1.at(i)<a2.at(i)){
				return true;
			}
			else if(a1.at(i)>a2.at(i)){
				return false;
			}
		}
	}
	return false;
}

////////////////////////////////////

inline bool compareFunctionGeneratePre(int i, int j)
{
	if(nIncompatible[i]>nIncompatible[j] || (nIncompatible[i]==nIncompatible[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j]))
		return true;
	
	return false;
}

inline bool compareFunctionGeneratePreWithGroupedActivities(int i, int j)
{
	//nMinDaysBroken is different from 0.0 only if the activity is fixed
	if( nIncompatibleFromFather[i]>nIncompatibleFromFather[j]
	 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j])
	 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]==nMinDaysConstraintsBroken[j] && nIncompatible[i]>nIncompatible[j] ) )
		return true;

	return false;
}


bool processTimeSpaceConstraints(QWidget* parent, QTextStream* initialOrderStream)
{
	assert(gt.rules.internalStructureComputed);
	
	////////////Compute equivalent subgroups
	if(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES){
		QList<int> tmpSortedSubgroupsList;
		Matrix1D<bool> isSignificantSubgroup;
		
		isSignificantSubgroup.resize(gt.rules.nInternalSubgroups);
		
		tmpSortedSubgroupsList.clear();
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			tmpSortedSubgroupsList.append(i);
		
		std::stable_sort(tmpSortedSubgroupsList.begin(), tmpSortedSubgroupsList.end(), findEquivalentSubgroupsCompareFunction);
		
		QStringList s;
		
		if(gt.rules.nInternalSubgroups>0)
			isSignificantSubgroup[tmpSortedSubgroupsList.at(0)]=true;
		for(int i=1; i<gt.rules.nInternalSubgroups; i++){
			int s1=tmpSortedSubgroupsList.at(i-1);
			int s2=tmpSortedSubgroupsList.at(i);
			
			isSignificantSubgroup[s2]=true;
		
			const QList<int>& l1=gt.rules.internalSubgroupsList[s1]->activitiesForSubgroup;
			const QList<int>& l2=gt.rules.internalSubgroupsList[s2]->activitiesForSubgroup;
			if(l1.count()==l2.count()){
				int i;
				for(i=0; i<l1.count(); i++)
					if(l1.at(i)!=l2.at(i))
						break;
				if(i==l1.count()){
					isSignificantSubgroup[s2]=false;
					
					s.append(GeneratePreTranslate::tr("Subgroup %1 has the same activities as subgroup %2.").arg(gt.rules.internalSubgroupsList[s2]->name).arg(gt.rules.internalSubgroupsList[s1]->name));
				}
			}
		}
		
		int cnt=0;
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			if(!isSignificantSubgroup[i])
				cnt++;
		
		if(cnt>0){
			QString s0=GeneratePreTranslate::tr("Optimization tip:");
			s0+=" ";
			s0+=GeneratePreTranslate::tr("There are %1 subgroups (from the total of %2 subgroups) which have the same activities as other subgroups. They are listed below."
				" If the constraints relating to these subgroups are also the same, you can make the generation (directly proportional) faster by completely removing the subgroups"
				" which are equivalent to other subgroups (leaving only one representant for each equivalence set). (The generation algorithm will not completely remove the equivalent"
				" subgroups automatically.)").arg(cnt).arg(gt.rules.nInternalSubgroups);
			s0+="\n\n";
			s0+=GeneratePreTranslate::tr("If you did not add all the activities yet or if the number of equivalent subgroups compared to the total number of subgroups"
				" is small, probably you can safely ignore this message.");
			s0+="\n\n";
			s0+=GeneratePreTranslate::tr("You can deactivate this message from the 'Settings' menu.");
			
			GeneratePreReconcilableMessage::largeInformation(parent, GeneratePreTranslate::tr("FET warning"), s0+QString("\n\n")+s.join("\n"));
		}
	}
	//////////////////////////////////

	//////////////////begin resizing

	//MIN DAYS BETWEEN ACTIVITIES
	minDaysListOfActivities.resize(gt.rules.nInternalActivities);
	minDaysListOfMinDays.resize(gt.rules.nInternalActivities);
	minDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);
	minDaysListOfConsecutiveIfSameDay.resize(gt.rules.nInternalActivities);

	//MAX DAYS BETWEEN ACTIVITIES
	maxDaysListOfActivities.resize(gt.rules.nInternalActivities);
	maxDaysListOfMaxDays.resize(gt.rules.nInternalActivities);
	maxDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	//MIN GAPS BETWEEN ACTIVITIES
	minGapsBetweenActivitiesListOfActivities.resize(gt.rules.nInternalActivities);
	minGapsBetweenActivitiesListOfMinGaps.resize(gt.rules.nInternalActivities);
	minGapsBetweenActivitiesListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	teachersWithMaxDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);
	subgroupsWithMaxDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);

	//activities same starting time
	activitiesSameStartingTimeActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingTimePercentages.resize(gt.rules.nInternalActivities);

	//activities same starting hour
	activitiesSameStartingHourActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingHourPercentages.resize(gt.rules.nInternalActivities);

	//activities same starting day
	activitiesSameStartingDayActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingDayPercentages.resize(gt.rules.nInternalActivities);

	//activities not overlapping
	activitiesNotOverlappingActivities.resize(gt.rules.nInternalActivities);
	activitiesNotOverlappingPercentages.resize(gt.rules.nInternalActivities);

	// 2 activities consecutive
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesConsecutivePercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesConsecutiveActivities.resize(gt.rules.nInternalActivities);

	//index represents the second activity, value in array represents the first activity
	inverseConstrTwoActivitiesConsecutivePercentages.resize(gt.rules.nInternalActivities);
	inverseConstrTwoActivitiesConsecutiveActivities.resize(gt.rules.nInternalActivities);
	// 2 activities consecutive

	// 2 activities grouped
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesGroupedPercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesGroupedActivities.resize(gt.rules.nInternalActivities);

	// 3 activities grouped
	//index represents the first activity, value in array represents the second activity
	constrThreeActivitiesGroupedPercentages.resize(gt.rules.nInternalActivities);
	constrThreeActivitiesGroupedActivities.resize(gt.rules.nInternalActivities);

	// 2 activities ordered
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesOrderedPercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesOrderedActivities.resize(gt.rules.nInternalActivities);

	//index represents the second activity, value in array represents the first activity
	inverseConstrTwoActivitiesOrderedPercentages.resize(gt.rules.nInternalActivities);
	inverseConstrTwoActivitiesOrderedActivities.resize(gt.rules.nInternalActivities);
	// 2 activities consecutive

	//rooms
	activitiesPreferredRoomsList.resize(gt.rules.nInternalActivities);

	activitiesHomeRoomsHomeRooms.resize(gt.rules.nInternalActivities);
	////////rooms

	mustComputeTimetableSubgroups.resize(gt.rules.nInternalActivities);
	mustComputeTimetableTeachers.resize(gt.rules.nInternalActivities);

	//////teachers and subgroups activity tag max hours daily and continuously
	teachersActivityTagMaxHoursDailyMaxHours.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyActivityTag.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyPercentage.resize(gt.rules.nInternalTeachers);

	teachersActivityTagMaxHoursContinuouslyMaxHours.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursContinuouslyActivityTag.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursContinuouslyPercentage.resize(gt.rules.nInternalTeachers);

	subgroupsActivityTagMaxHoursDailyMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyActivityTag.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyPercentage.resize(gt.rules.nInternalSubgroups);

	subgroupsActivityTagMaxHoursContinuouslyMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursContinuouslyActivityTag.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursContinuouslyPercentage.resize(gt.rules.nInternalSubgroups);
	
	//2011-09-25
	aomtsListForActivity.resize(gt.rules.nInternalActivities);
	//2011-09-30
	amsistsListForActivity.resize(gt.rules.nInternalActivities);

	//2012-04-29
	aomdrListForActivity.resize(gt.rules.nInternalActivities);

	//2013-09-14
	asricListForActivity.resize(gt.rules.nInternalActivities);

	//////////////////end resizing - new feature
	
	QHash<int, int> reprSameStartingTime;
	QHash<int, QSet<int> > reprSameActivitiesSet;

	/////1. BASIC TIME CONSTRAINTS
	bool t=computeActivitiesConflictingPercentage(parent);
	if(!t)
		return false;
	//////////////////////////////
	
	/////2. min days between activities
	t=computeMinDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.3. max days between activities
	t=computeMaxDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.5. min gaps between activities
	t=computeMinGapsBetweenActivities(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////3. st not avail, tch not avail, break, activity pref time,
	/////   activity preferred times, activities preferred times
	t=computeNotAllowedTimesPercentages(parent);
	if(!t)
		return false;
	///////////////////////////////////////////////////////////////
	
	/////3.5. STUDENTS MAX DAYS PER WEEK
	t=computeMaxDaysPerWeekForStudents(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	/////4. students max gaps and early
	t=computeNHoursPerSubgroup(parent);
	if(!t)
		return false;
	t=computeSubgroupsEarlyAndMaxGapsPercentages(parent);
	if(!t)
		return false;
	t=computeSubgroupsMaxGapsPerDayPercentages(parent); //!!!after max gaps per week
	if(!t)
		return false;
		
	//////////////////////////////////
	
	/////5. TEACHERS MAX DAYS PER WEEK
	t=computeMaxDaysPerWeekForTeachers(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	
	/////6. TEACHERS MAX GAPS PER WEEK/DAY
	t=computeNHoursPerTeacher(parent);
	if(!t)
		return false;
	t=computeTeachersMaxGapsPerWeekPercentage(parent);
	if(!t)
		return false;
	t=computeTeachersMaxGapsPerDayPercentage(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	//must be AFTER basic time constraints (computeActivitiesConflictingPercentage)
	t=computeActivitiesSameStartingTime(parent, reprSameStartingTime, reprSameActivitiesSet);
	if(!t)
		return false;

	computeActivitiesSameStartingHour();
	
	computeActivitiesSameStartingDay();
	
	computeActivitiesNotOverlapping();

	//must be after allowed times, after n hours per teacher and after max days per week
	t=computeTeachersMaxHoursDaily(parent);
	if(!t)
		return false;

	t=computeTeachersMaxHoursContinuously(parent);
	if(!t)
		return false;

	t=computeTeachersActivityTagMaxHoursDaily(parent);
	if(!t)
		return false;

	t=computeTeachersActivityTagMaxHoursContinuously(parent);
	if(!t)
		return false;

	//must be after n hours per teacher
	t=computeTeachersMinHoursDaily(parent);
	if(!t)
		return false;
	
	//must be after teachers min hours daily
	t=computeTeachersMinDaysPerWeek(parent);
	if(!t)
		return false;
	
	//must be after allowed times and after n hours per subgroup
	t=computeSubgroupsMaxHoursDaily(parent);
	if(!t)
		return false;
	
	t=computeStudentsMaxHoursContinuously(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMaxHoursDaily(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMaxHoursContinuously(parent);
	if(!t)
		return false;
		
	//after max hours daily/continuously without/with an activity tag
	t=checkMaxHoursForActivityDuration(parent);
	if(!t)
		return false;

	t=computeSubgroupsMinHoursDaily(parent);
	if(!t)
		return false;
		
	computeConstrTwoActivitiesConsecutive();
	
	computeConstrTwoActivitiesGrouped();
	
	computeConstrThreeActivitiesGrouped();
	
	computeConstrTwoActivitiesOrdered();
	
	t=computeActivityEndsStudentsDayPercentages(parent);
	if(!t)
		return false;
		
	//check for impossible min days
	t=checkMinDays100Percent(parent);
	if(!t)
		return false;
	t=checkMinDaysConsecutiveIfSameDay(parent);
	if(!t)
		return false;
	
	//check teachers interval max days per week
	t=computeTeachersIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;
	
	//check subgroups interval max days per week
	t=computeSubgroupsIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;
		
	////////////////
	haveActivitiesOccupyOrSimultaneousConstraints=false;

	//2011-09-25
	t=computeActivitiesOccupyMaxTimeSlotsFromSelection(parent);
	if(!t)
		return false;
	
	//2011-09-30
	t=computeActivitiesMaxSimultaneousInSelectedTimeSlots(parent);
	if(!t)
		return false;
	////////////////

	//2012-04-29
	t=computeActivitiesOccupyMaxDifferentRooms(parent);
	if(!t)
		return false;
	
	////////////////
	
	//2013-09-14
	t=computeActivitiesSameRoomIfConsecutive(parent);
	if(!t)
		return false;
	
	////////////////
	
	/////////////rooms
	t=computeBasicSpace(parent);
	if(!t)
		return false;
	t=computeNotAllowedRoomTimePercentages();
	if(!t)
		return false;
	t=computeActivitiesRoomsPreferences(parent);
	if(!t)
		return false;
	//////////////////
	
	/////////buildings
	t=computeMaxBuildingChangesPerDayForStudents(parent);
	if(!t)
		return false;
	t=computeMaxBuildingChangesPerWeekForStudents(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenBuildingChangesForStudents(parent);
	if(!t)
		return false;

	t=computeMaxBuildingChangesPerDayForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxBuildingChangesPerWeekForTeachers(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenBuildingChangesForTeachers(parent);
	if(!t)
		return false;
	//////////////////
	
	t=homeRoomsAreOk(parent);
	if(!t)
		return false;
	
	computeMustComputeTimetableSubgroups();
	computeMustComputeTimetableTeachers();
	
	t=computeFixedActivities(parent);
	if(!t)
		return false;
	
	//must have here repr computed correctly
	sortActivities(parent, reprSameStartingTime, reprSameActivitiesSet, initialOrderStream);
	
	if(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS){
		if(haveStudentsMaxGapsPerDay || haveTeachersActivityTagMaxHoursDaily || haveStudentsActivityTagMaxHoursDaily){
			QString s=GeneratePreTranslate::tr("Your data contains constraints students max gaps per day and/or activity tag max hours daily.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("These constraints are good, but they are not perfectly optimized for speed. You may obtain a long generation time or even impossible timetables.");
			s+=" ";
			s+=GeneratePreTranslate::tr("It is recommended to use such constraints with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");
	
#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}
	
	if(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS){
		if(haveStudentsMinHoursDailyAllowEmptyDays){
			QString s=GeneratePreTranslate::tr("Your data contains constraints students min hours daily which allow empty days.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("These constraints are nonstandard. They are recommended only if the students can have free days and a solution with free days for students exists."
				" Otherwise the solution might be impossible for FET to find.");
			s+=" ";
			s+=GeneratePreTranslate::tr("It is recommended to use such constraints with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");

#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}

	if(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER){
		if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
			QString s=GeneratePreTranslate::tr("Your data contains the option to group activities in the initial order.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("This option is nonstandard. It is recommended only if you know what you are doing,"
				" otherwise the solution might be impossible for FET to find.");
			s+=" ";
			s+=GeneratePreTranslate::tr("Use with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");

#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}

	bool ok=true;
	
	return ok;
}

//must be after allowed times and after n hours per subgroup
bool computeSubgroupsMaxHoursDaily(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursDailyMaxHours1[i]=-1;
		subgroupsMaxHoursDailyPercentages1[i]=-1;

		subgroupsMaxHoursDailyMaxHours2[i]=-1;
		subgroupsMaxHoursDailyPercentages2[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
			ConstraintStudentsMaxHoursDaily* smd=(ConstraintStudentsMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursDailyMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{				
						 //cannot proceed
						ok=false;
		
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* smd=(ConstraintStudentsSetMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursDailyMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						//cannot proceed
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages1[sb]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours1[sb]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
				//n days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours1[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages2[sb]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours2[sb]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
				//n days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours2[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}
	
bool computeStudentsMaxHoursContinuously(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursContinuouslyMaxHours1[i]=-1;
		subgroupsMaxHoursContinuouslyPercentages1[i]=-1;

		subgroupsMaxHoursContinuouslyMaxHours2[i]=-1;
		subgroupsMaxHoursContinuouslyPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsMaxHoursContinuously* smd=(ConstraintStudentsMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursContinuouslyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursContinuouslyMaxHours1[sb] >= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursContinuouslyMaxHours1[sb] = smd->maxHoursContinuously;
					subgroupsMaxHoursContinuouslyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursContinuouslyMaxHours1[sb] <= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursContinuouslyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursContinuouslyMaxHours2[sb] >= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursContinuouslyMaxHours2[sb] = smd->maxHoursContinuously;
						subgroupsMaxHoursContinuouslyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursContinuouslyMaxHours2[sb] <= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{				
						 //cannot proceed
						ok=false;
		
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetMaxHoursContinuously* smd=(ConstraintStudentsSetMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursContinuouslyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursContinuouslyMaxHours1[sb] >= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursContinuouslyMaxHours1[sb] = smd->maxHoursContinuously;
					subgroupsMaxHoursContinuouslyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursContinuouslyMaxHours1[sb] <= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursContinuouslyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursContinuouslyMaxHours2[sb] >= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursContinuouslyMaxHours2[sb] = smd->maxHoursContinuously;
						subgroupsMaxHoursContinuouslyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursContinuouslyMaxHours2[sb] <= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						//cannot proceed
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		foreach(int sbg, gt.rules.internalActivitiesList[ai].iSubgroupsList){
			if(subgroupsMaxHoursContinuouslyPercentages1[sbg]>=0 && gt.rules.internalActivitiesList[ai].duration > subgroupsMaxHoursContinuouslyMaxHours1[sbg]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(subgroupsMaxHoursContinuouslyMaxHours1[sbg])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			if(subgroupsMaxHoursContinuouslyPercentages2[sbg]>=0 && gt.rules.internalActivitiesList[ai].duration > subgroupsMaxHoursContinuouslyMaxHours2[sbg]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(subgroupsMaxHoursContinuouslyMaxHours2[sbg])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeStudentsActivityTagMaxHoursDaily(QWidget* parent)
{
	haveStudentsActivityTagMaxHoursDaily=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsActivityTagMaxHoursDailyMaxHours[i].clear();
		subgroupsActivityTagMaxHoursDailyPercentage[i].clear();
		subgroupsActivityTagMaxHoursDailyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveStudentsActivityTagMaxHoursDaily=true;

			ConstraintStudentsActivityTagMaxHoursDaily* samc=(ConstraintStudentsActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int sb, samc->canonicalSubgroupsList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samc->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) <= samc->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) >= samc->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos1]=samc->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samc->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) <= samc->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) >= samc->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos2]=samc->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
								 " of type activity tag max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveStudentsActivityTagMaxHoursDaily=true;

			ConstraintStudentsSetActivityTagMaxHoursDaily* samc=(ConstraintStudentsSetActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int sb, samc->canonicalSubgroupsList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samc->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) <= samc->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) >= samc->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos1]=samc->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samc->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) <= samc->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) >= samc->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos2]=samc->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
								 " of type activity tag max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}

	int navd[MAX_DAYS_PER_WEEK];
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[i][d][h])
					navd[d]++;
			}
		}
	
		for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[i].count(); j++){
			int mh=subgroupsActivityTagMaxHoursDailyMaxHours[i].at(j);
			double perc=subgroupsActivityTagMaxHoursDailyPercentage[i].at(j);
			int at=subgroupsActivityTagMaxHoursDailyActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				foreach(int ai, gt.rules.internalSubgroupsList[i]->activitiesForSubgroup)
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;
						
				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					ava+=min(navd[d], mh);
					
				if(ava<totalAt){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint activity tag %2 max %3 hours daily for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalSubgroupsList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}

bool computeStudentsActivityTagMaxHoursContinuously(QWidget* parent)
{
	haveStudentsActivityTagMaxHoursContinuously=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsActivityTagMaxHoursContinuouslyMaxHours[i].clear();
		subgroupsActivityTagMaxHoursContinuouslyPercentage[i].clear();
		subgroupsActivityTagMaxHoursContinuouslyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveStudentsActivityTagMaxHoursContinuously=true;

			ConstraintStudentsActivityTagMaxHoursContinuously* samc=(ConstraintStudentsActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int sb, samc->canonicalSubgroupsList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
					subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) <= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) >= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos1]=samc->maxHoursContinuously;
						subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
							subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) <= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) >= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos2]=samc->maxHoursContinuously;
								subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
								 " of type activity tag max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveStudentsActivityTagMaxHoursContinuously=true;

			ConstraintStudentsSetActivityTagMaxHoursContinuously* samc=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int sb, samc->canonicalSubgroupsList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
					subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) <= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) >= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos1]=samc->maxHoursContinuously;
						subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
							subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) <= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) >= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos2]=samc->maxHoursContinuously;
								subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
								 " of type activity tag max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return ok;
}

bool checkMaxHoursForActivityDuration(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		
		//teachers
		foreach(int tch, act->iTeachersList){
			if(teachersMaxHoursDailyPercentages1[tch]==100.0){
				int m=teachersMaxHoursDailyMaxHours1[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours daily with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(teachersMaxHoursDailyPercentages2[tch]==100.0){
				int m=teachersMaxHoursDailyMaxHours2[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours daily with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			if(teachersMaxHoursContinuouslyPercentages1[tch]==100.0){
				int m=teachersMaxHoursContinuouslyMaxHours1[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours continuously with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(teachersMaxHoursContinuouslyPercentages2[tch]==100.0){
				int m=teachersMaxHoursContinuouslyMaxHours2[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours continuously with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tch].count(); j++){
				if(teachersActivityTagMaxHoursDailyPercentage[tch].at(j)==100.0){
					int m=teachersActivityTagMaxHoursDailyMaxHours[tch].at(j);
					int at=teachersActivityTagMaxHoursDailyActivityTag[tch].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint teacher(s) activity tag max %3 hours daily with weight = 100% for the teacher %4 and activity tag %5."
						 " The activity's duration is higher than the teacher's max hours daily with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalTeachersList[tch]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}

			for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tch].count(); j++){
				if(teachersActivityTagMaxHoursContinuouslyPercentage[tch].at(j)==100.0){
					int m=teachersActivityTagMaxHoursContinuouslyMaxHours[tch].at(j);
					int at=teachersActivityTagMaxHoursContinuouslyActivityTag[tch].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint teacher(s) activity tag max %3 hours continuously with weight = 100% for the teacher %4 and activity tag %5."
						 " The activity's duration is higher than the teacher's max hours continuously with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalTeachersList[tch]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}

		//students
		foreach(int sbg, act->iSubgroupsList){
			if(subgroupsMaxHoursDailyPercentages1[sbg]==100.0){
				int m=subgroupsMaxHoursDailyMaxHours1[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours daily with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			if(subgroupsMaxHoursDailyPercentages2[sbg]==100.0){
				int m=subgroupsMaxHoursDailyMaxHours2[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours daily with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		
			if(subgroupsMaxHoursContinuouslyPercentages1[sbg]==100.0){
				int m=subgroupsMaxHoursContinuouslyMaxHours1[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours continuously with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(subgroupsMaxHoursContinuouslyPercentages2[sbg]==100.0){
				int m=subgroupsMaxHoursContinuouslyMaxHours2[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours continuously with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		
			for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sbg].count(); j++){
				if(subgroupsActivityTagMaxHoursDailyPercentage[sbg].at(j)==100.0){
					int m=subgroupsActivityTagMaxHoursDailyMaxHours[sbg].at(j);
					int at=subgroupsActivityTagMaxHoursDailyActivityTag[sbg].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint students (set) activity tag max %3 hours daily with weight = 100% for the subgroup %4 and activity tag %5."
						 " The activity's duration is higher than the subgroup's max hours daily with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalSubgroupsList[sbg]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}

			for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].count(); j++){
				if(subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].at(j)==100.0){
					int m=subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].at(j);
					int at=subgroupsActivityTagMaxHoursContinuouslyActivityTag[sbg].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint students (set) activity tag max %3 hours continuously with weight = 100% for the subgroup %4 and activity tag %5."
						 " The activity's duration is higher than the subgroup's max hours continuously with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalSubgroupsList[sbg]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	return ok;
}

bool computeSubgroupsMinHoursDaily(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinHoursDailyMinHours[i]=-1;
		subgroupsMinHoursDailyPercentages[i]=-1;
		subgroupsMinHoursDailyAllowEmptyDays[i]=true;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students, because the constraint of type min hours daily relating to students"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			//////////
			if(smd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours daily with"
				 " %1 min hours daily, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(smd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students set %1, because the constraint of type min hours daily relating to him"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again")
				 .arg(smd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			//////////
			if(smd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours daily for students set %1 with"
				 " %2 min hours daily, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(smd->students)
				 .arg(smd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
		}
	}

	if(!ok)
		return ok;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMinHoursDailyMinHours[sb]==-1 ||
				 (subgroupsMinHoursDailyMinHours[sb] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] <= smd->weightPercentage)){
				 	subgroupsMinHoursDailyMinHours[sb] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
				
				if(smd->allowEmptyDays==false)
					subgroupsMinHoursDailyAllowEmptyDays[sb]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
				if(subgroupsMinHoursDailyMinHours[sb]==-1 ||
				 (subgroupsMinHoursDailyMinHours[sb] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] <= smd->weightPercentage)){
				 	subgroupsMinHoursDailyMinHours[sb] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
				if(smd->allowEmptyDays==false)
					subgroupsMinHoursDailyAllowEmptyDays[sb]=false;
			}
		}
	}
	
	haveStudentsMinHoursDailyAllowEmptyDays=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		if(subgroupsMinHoursDailyMinHours[i]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==true && !haveStudentsMinHoursDailyAllowEmptyDays)
			haveStudentsMinHoursDailyAllowEmptyDays=true;
	
		if(subgroupsMinHoursDailyMinHours[i]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==false){
			if(gt.rules.nDaysPerWeek*subgroupsMinHoursDailyMinHours[i] > nHoursPerSubgroup[i]){
				ok=false;
			
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("For subgroup %1 you have too little activities to respect the constraint(s)"
				 " of type min hours daily (the constraint(s) do not allow empty days). Please modify your data accordingly and try again.")
				 .arg(gt.rules.internalSubgroupsList[i]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			for(int j=0; j<gt.rules.nDaysPerWeek; j++){
				int freeSlots=0;
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
						freeSlots++;
				if(subgroupsMinHoursDailyMinHours[i]>freeSlots){
					ok=false;
			
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For subgroup %1 cannot respect the constraint(s)"
					 " of type min hours daily (the constraint(s) do not allow empty days) on day %2, because of students set not available and/or break."
					 " Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 .arg(gt.rules.daysOfTheWeek[j]),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(subgroupsMinHoursDailyMinHours[i]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==true){

			if(nHoursPerSubgroup[i]>0 && subgroupsMinHoursDailyMinHours[i]>nHoursPerSubgroup[i]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
				 " %2 (the constraint allows empty days). This subgroup has in total only %3 hours per week, so impossible constraint."
				 " Please correct and try again")
				 .arg(subgroupsMinHoursDailyMinHours[i])
				 .arg(gt.rules.internalSubgroupsList[i]->name)
				 .arg(nHoursPerSubgroup[i])
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(subgroupsMinHoursDailyMinHours[i]<2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
				 " %2 and the constraint allows empty days. The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
				 .arg(subgroupsMinHoursDailyMinHours[i])
				 .arg(gt.rules.internalSubgroupsList[i]->name)
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}
	
//must be after allowed times, after n hours per teacher and after max days per week
bool computeTeachersMaxHoursDaily(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursDailyMaxHours1[i]=-1;
		teachersMaxHoursDailyPercentages1[i]=-1;

		teachersMaxHoursDailyMaxHours2[i]=-1;
		teachersMaxHoursDailyPercentages2[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* tmd=(ConstraintTeacherMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID]==-1 ||
			 (teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] >= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] <= tmd->weightPercentage)){
			 	teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] = tmd->maxHoursDaily;
				teachersMaxHoursDailyPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] <= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{
				if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID]==-1 ||
				 (teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] <= tmd->weightPercentage)){
				 	teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours daily.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
			ConstraintTeachersMaxHoursDaily* tmd=(ConstraintTeachersMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursDailyMaxHours1[tch]==-1 ||
				 (teachersMaxHoursDailyMaxHours1[tch] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] <= tmd->weightPercentage)){
				 	teachersMaxHoursDailyMaxHours1[tch] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursDailyMaxHours1[tch] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{
					if(teachersMaxHoursDailyMaxHours2[tch]==-1 ||
					 (teachersMaxHoursDailyMaxHours2[tch] >= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] <= tmd->weightPercentage)){
					 	teachersMaxHoursDailyMaxHours2[tch] = tmd->maxHoursDaily;
						teachersMaxHoursDailyPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursDailyMaxHours2[tch] <= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] >= tmd->weightPercentage){
				 	//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages1[tc]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours1[tc]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//n days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours1[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages2[tc]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours2[tc]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//n days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours2[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

bool computeTeachersMaxHoursContinuously(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursContinuouslyMaxHours1[i]=-1;
		teachersMaxHoursContinuouslyPercentages1[i]=-1;

		teachersMaxHoursContinuouslyMaxHours2[i]=-1;
		teachersMaxHoursContinuouslyPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherMaxHoursContinuously* tmd=(ConstraintTeacherMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			if(teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID]==-1 ||
			 (teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] >= tmd->maxHoursContinuously &&
			 teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] <= tmd->weightPercentage)){
			 	teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] = tmd->maxHoursContinuously;
				teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] <= tmd->maxHoursContinuously &&
			 teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{
				if(teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID]==-1 ||
				 (teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] >= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] <= tmd->weightPercentage)){
				 	teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] = tmd->maxHoursContinuously;
					teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] <= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours continuously.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersMaxHoursContinuously* tmd=(ConstraintTeachersMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursContinuouslyMaxHours1[tch]==-1 ||
				 (teachersMaxHoursContinuouslyMaxHours1[tch] >= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages1[tch] <= tmd->weightPercentage)){
				 	teachersMaxHoursContinuouslyMaxHours1[tch] = tmd->maxHoursContinuously;
					teachersMaxHoursContinuouslyPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursContinuouslyMaxHours1[tch] <= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages1[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{
					if(teachersMaxHoursContinuouslyMaxHours2[tch]==-1 ||
					 (teachersMaxHoursContinuouslyMaxHours2[tch] >= tmd->maxHoursContinuously &&
					 teachersMaxHoursContinuouslyPercentages2[tch] <= tmd->weightPercentage)){
					 	teachersMaxHoursContinuouslyMaxHours2[tch] = tmd->maxHoursContinuously;
						teachersMaxHoursContinuouslyPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursContinuouslyMaxHours2[tch] <= tmd->maxHoursContinuously &&
					 teachersMaxHoursContinuouslyPercentages2[tch] >= tmd->weightPercentage){
				 	//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		foreach(int tch, gt.rules.internalActivitiesList[ai].iTeachersList){
			if(teachersMaxHoursContinuouslyPercentages1[tch]>=0 && gt.rules.internalActivitiesList[ai].duration > teachersMaxHoursContinuouslyMaxHours1[tch]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(teachersMaxHoursContinuouslyMaxHours1[tch])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			if(teachersMaxHoursContinuouslyPercentages2[tch]>=0 && gt.rules.internalActivitiesList[ai].duration > teachersMaxHoursContinuouslyMaxHours2[tch]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(teachersMaxHoursContinuouslyMaxHours2[tch])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeTeachersActivityTagMaxHoursDaily(QWidget* parent)
{
	haveTeachersActivityTagMaxHoursDaily=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersActivityTagMaxHoursDailyMaxHours[i].clear();
		teachersActivityTagMaxHoursDailyPercentage[i].clear();
		teachersActivityTagMaxHoursDailyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveTeachersActivityTagMaxHoursDaily=true;

			ConstraintTeachersActivityTagMaxHoursDaily* samc=(ConstraintTeachersActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int tc, samc->canonicalTeachersList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyActivityTag[tc].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursDailyActivityTag[tc].append(samc->activityTagIndex);
					teachersActivityTagMaxHoursDailyMaxHours[tc].append(samc->maxHoursDaily);
					teachersActivityTagMaxHoursDailyPercentage[tc].append(samc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) <= samc->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) >= samc->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) <= samc->weightPercentage){
					
						teachersActivityTagMaxHoursDailyActivityTag[tc][pos1]=samc->activityTagIndex;
						teachersActivityTagMaxHoursDailyMaxHours[tc][pos1]=samc->maxHoursDaily;
						teachersActivityTagMaxHoursDailyPercentage[tc][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyActivityTag[tc].append(samc->activityTagIndex);
							teachersActivityTagMaxHoursDailyMaxHours[tc].append(samc->maxHoursDaily);
							teachersActivityTagMaxHoursDailyPercentage[tc].append(samc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) <= samc->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) >= samc->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) <= samc->weightPercentage){
							
								teachersActivityTagMaxHoursDailyActivityTag[tc][pos2]=samc->activityTagIndex;
								teachersActivityTagMaxHoursDailyMaxHours[tc][pos2]=samc->maxHoursDaily;
								teachersActivityTagMaxHoursDailyPercentage[tc][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
								 " of type activity tag max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveTeachersActivityTagMaxHoursDaily=true;

			ConstraintTeacherActivityTagMaxHoursDaily* samc=(ConstraintTeacherActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int tc, samc->canonicalTeachersList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyActivityTag[tc].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursDailyActivityTag[tc].append(samc->activityTagIndex);
					teachersActivityTagMaxHoursDailyMaxHours[tc].append(samc->maxHoursDaily);
					teachersActivityTagMaxHoursDailyPercentage[tc].append(samc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) <= samc->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) >= samc->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) <= samc->weightPercentage){
					
						teachersActivityTagMaxHoursDailyActivityTag[tc][pos1]=samc->activityTagIndex;
						teachersActivityTagMaxHoursDailyMaxHours[tc][pos1]=samc->maxHoursDaily;
						teachersActivityTagMaxHoursDailyPercentage[tc][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyActivityTag[tc].append(samc->activityTagIndex);
							teachersActivityTagMaxHoursDailyMaxHours[tc].append(samc->maxHoursDaily);
							teachersActivityTagMaxHoursDailyPercentage[tc].append(samc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) <= samc->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) >= samc->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) <= samc->weightPercentage){
							
								teachersActivityTagMaxHoursDailyActivityTag[tc][pos2]=samc->activityTagIndex;
								teachersActivityTagMaxHoursDailyMaxHours[tc][pos2]=samc->maxHoursDaily;
								teachersActivityTagMaxHoursDailyPercentage[tc][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
								 " of type activity tag max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}

	int navd[MAX_DAYS_PER_WEEK];
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[i][d][h])
					navd[d]++;
			}
		}
	
		for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[i].count(); j++){
			int mh=teachersActivityTagMaxHoursDailyMaxHours[i].at(j);
			double perc=teachersActivityTagMaxHoursDailyPercentage[i].at(j);
			int at=teachersActivityTagMaxHoursDailyActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				foreach(int ai, gt.rules.internalTeachersList[i]->activitiesForTeacher)
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;
						
				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					ava+=min(navd[d], mh);
					
				if(ava<totalAt){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint activity tag %2 max %3 hours daily for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalTeachersList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}

bool computeTeachersActivityTagMaxHoursContinuously(QWidget* parent)
{
	haveTeachersActivityTagMaxHoursContinuously=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersActivityTagMaxHoursContinuouslyMaxHours[i].clear();
		teachersActivityTagMaxHoursContinuouslyPercentage[i].clear();
		teachersActivityTagMaxHoursContinuouslyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveTeachersActivityTagMaxHoursContinuously=true;

			ConstraintTeachersActivityTagMaxHoursContinuously* samc=(ConstraintTeachersActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int tc, samc->canonicalTeachersList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursContinuouslyActivityTag[tc].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(samc->activityTagIndex);
					teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(samc->maxHoursContinuously);
					teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(samc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) <= samc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) >= samc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) <= samc->weightPercentage){
					
						teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos1]=samc->activityTagIndex;
						teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos1]=samc->maxHoursContinuously;
						teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(samc->activityTagIndex);
							teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(samc->maxHoursContinuously);
							teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(samc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) <= samc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) >= samc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) <= samc->weightPercentage){
							
								teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos2]=samc->activityTagIndex;
								teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos2]=samc->maxHoursContinuously;
								teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
								 " of type activity tag max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveTeachersActivityTagMaxHoursContinuously=true;

			ConstraintTeacherActivityTagMaxHoursContinuously* samc=(ConstraintTeacherActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			foreach(int tc, samc->canonicalTeachersList){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursContinuouslyActivityTag[tc].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(samc->activityTagIndex);
					teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(samc->maxHoursContinuously);
					teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(samc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) <= samc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) >= samc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) <= samc->weightPercentage){
					
						teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos1]=samc->activityTagIndex;
						teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos1]=samc->maxHoursContinuously;
						teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(samc->activityTagIndex);
							teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(samc->maxHoursContinuously);
							teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(samc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) <= samc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) >= samc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) <= samc->weightPercentage){
							
								teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos2]=samc->activityTagIndex;
								teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos2]=samc->maxHoursContinuously;
								teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
								 " of type activity tag max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return ok;
}

//must be after n hours per teacher
bool computeTeachersMinHoursDaily(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinHoursDailyMinHours[i]=-1;
		teachersMinHoursDailyPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* tmd=(ConstraintTeacherMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " %2 min hours daily, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			if(teachersMinHoursDailyMinHours[tmd->teacher_ID]==-1 || teachersMinHoursDailyMinHours[tmd->teacher_ID]<tmd->minHoursDaily){
				teachersMinHoursDailyMinHours[tmd->teacher_ID]=tmd->minHoursDaily;
				teachersMinHoursDailyPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
			ConstraintTeachersMinHoursDaily* tmd=(ConstraintTeachersMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " %1 min hours daily, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinHoursDailyMinHours[tch]==-1 || teachersMinHoursDailyMinHours[tch]<tmd->minHoursDaily){
					teachersMinHoursDailyMinHours[tch]=tmd->minHoursDaily;
					teachersMinHoursDailyPercentages[tch]=100;
				}
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinHoursDailyPercentages[tc]==100){
			assert(teachersMinHoursDailyMinHours[tc]>=0);
			if(nHoursPerTeacher[tc]>0 && teachersMinHoursDailyMinHours[tc]>nHoursPerTeacher[tc]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2 (the constraint allows empty days). This teacher has in total only %3 hours per week, so impossible constraint."
				 " Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(nHoursPerTeacher[tc])
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMinHoursDailyMinHours[tc]<2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2 (the constraint allows empty days). The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after min hours for teachers
bool computeTeachersMinDaysPerWeek(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinDaysPerWeekMinDays[i]=-1;
		teachersMinDaysPerWeekPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
			ConstraintTeacherMinDaysPerWeek* tmd=(ConstraintTeacherMinDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min days per week for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min days per week for teacher %1 with"
				 " %2 min days per week, and the number of working days per week is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			if(teachersMinDaysPerWeekMinDays[tmd->teacher_ID]==-1 || teachersMinDaysPerWeekMinDays[tmd->teacher_ID]<tmd->minDaysPerWeek){
				teachersMinDaysPerWeekMinDays[tmd->teacher_ID]=tmd->minDaysPerWeek;
				teachersMinDaysPerWeekPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK){
			ConstraintTeachersMinDaysPerWeek* tmd=(ConstraintTeachersMinDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min days per week with weight"
				 " (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min days per week with"
				 " %1 min days per week, and the number of working days per week is only %2. Please correct and try again")
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinDaysPerWeekMinDays[tch]==-1 || teachersMinDaysPerWeekMinDays[tch]<tmd->minDaysPerWeek){
					teachersMinDaysPerWeekMinDays[tch]=tmd->minDaysPerWeek;
					teachersMinDaysPerWeekPercentages[tch]=100;
				}
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinDaysPerWeekMinDays[tc]>=0){
			int md=teachersMinDaysPerWeekMinDays[tc];
			if(md>gt.rules.internalTeachersList[tc]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and he has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tc]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tc]>=0){
				int mh=teachersMinHoursDailyMinHours[tc];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and min hours daily %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}


	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinDaysPerWeekMinDays[tc]>=0){
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				if(teachersMaxDaysPerWeekMaxDays[tc]<teachersMinDaysPerWeekMinDays[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 > max days per week %3"
					 " - impossible (min days must be <= max days). Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(teachersMaxDaysPerWeekMaxDays[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		
			int med=1; //minimum each day = 1 hour
			if(teachersMinHoursDailyMinHours[tc]>=0)
				med=teachersMinHoursDailyMinHours[tc];
				
			int navdays=0;
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				int navhours=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						navhours++;
				if(navhours>=med)
					navdays++;
			}
			
			if(navdays<teachersMinDaysPerWeekMinDays[tc]){
				ok=false;
				
				QString s;
				
				if(teachersMinHoursDailyMinHours[tc]>=0){
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and only %3"
					 " available days considering breaks and not available and min hours daily for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}
				else{
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and only %3"
					 " available days considering breaks and not available for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}

	
	return ok;
}

void computeActivitiesNotOverlapping()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesNotOverlappingActivities[i].clear();
		activitiesNotOverlappingPercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			ConstraintActivitiesNotOverlapping* no=(ConstraintActivitiesNotOverlapping*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<no->_n_activities; j++){
				int ai1=no->_activities[j];
				for(int k=0; k<no->_n_activities; k++){
					int ai2=no->_activities[k];
					if(ai1!=ai2){
						int t=activitiesNotOverlappingActivities[ai1].indexOf(ai2);
						if(t>=0){
							if(activitiesNotOverlappingPercentages[ai1].at(t) < no->weightPercentage){
								activitiesNotOverlappingPercentages[ai1][t]=no->weightPercentage;
							}
						}
						else{
							activitiesNotOverlappingPercentages[ai1].append(no->weightPercentage);
							activitiesNotOverlappingActivities[ai1].append(ai2);
						}
					}
				}
			}
		}
}

bool computeActivitiesSameStartingTime(QWidget* parent, QHash<int, int> & reprSameStartingTime, QHash<int, QSet<int> > & reprSameActivitiesSet)
{
	bool reportunder100=true;
	bool report100=true;
	
	bool oktocontinue=true;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingTimeActivities[i].clear();
		activitiesSameStartingTimePercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=j+1; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					
					if(sst->weightPercentage==100.0 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100)
						oktocontinue=false;
					
					if(sst->weightPercentage<100.0 && reportunder100 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100){
						QString s;
						
						s+=sst->getDetailedDescription(gt.rules);
						s+="\n";
						s+=GeneratePreTranslate::tr("The constraint is impossible to respect, because there are the activities with id-s %1 and %2 which "
						 "conflict one with another, because they have common students sets or teachers or must be not overlapping. FET will allow you to continue, "
						 "because the weight of this constraint is below 100.0%, "
						 "but anyway most probably you have made a mistake in this constraint, "
						 "so it is recommended to modify it.")
						 .arg(gt.rules.internalActivitiesList[ai1].id)
						 .arg(gt.rules.internalActivitiesList[ai2].id);
					
						int t=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							reportunder100=false;
					}
					else if(sst->weightPercentage==100.0 && report100 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100){
						QString s;
						
						s+=sst->getDetailedDescription(gt.rules);
						s+="\n";
						s+=GeneratePreTranslate::tr("The constraint is impossible to respect, because there are the activities with id-s %1 and %2 which "
						 "conflict one with another, because they have common students sets or teachers or must be not overlapping. The weight of this constraint is 100.0%, "
						 "so your timetable is impossible. Please correct this constraint.")
						 .arg(gt.rules.internalActivitiesList[ai1].id)
						 .arg(gt.rules.internalActivitiesList[ai2].id);
					
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							report100=false;
					}
				}
			}
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingTimeActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingTimePercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingTimeActivities[ai1].append(ai2);
							activitiesSameStartingTimePercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingTimeActivities[ai1].count()==activitiesSameStartingTimePercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingTimePercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
		
		
	//added June 2009, FET-5.10.0
	bool reportIndirect=true;
	
	QMultiHash<int, int> adjMatrix;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
			for(int i=1; i<sst->_n_activities; i++){
				adjMatrix.insert(sst->_activities[0], sst->_activities[i]);
				adjMatrix.insert(sst->_activities[i], sst->_activities[0]);
			}
		}
	}
		
	QHash<int, int> repr;
	//repr.clear();
	
	QQueue<int> queue;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int start=i;
		
		if(repr.value(start, -1)==-1){ //not visited
			repr.insert(start, start);
			queue.enqueue(start);
			while(!queue.isEmpty()){
				int crtHead=queue.dequeue();
				assert(repr.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix.values(crtHead);
				foreach(int neigh, neighList){
					if(repr.value(neigh, -1)==-1){
						queue.enqueue(neigh);
						repr.insert(neigh, start);
					}
					else{
						assert(repr.value(neigh, -1)==start);
					}
				}
			}
		}
	}
	
	//faster than below
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		QHash<int, int> &hashConfl=activitiesConflictingPercentage[i];

		QHashIterator<int, int> it(hashConfl);
		while(it.hasNext()){
			it.next();
			//cout<<it.key()<<": "<<it.value()<<endl;
			int j=it.key();
			if(i!=j){
				if(it.value()==100){
					if(repr.value(i)==repr.value(j)){
						oktocontinue=false;
					
						if(reportIndirect){
							QString s=GeneratePreTranslate::tr("You have a set of impossible constraints activities same starting time, considering all the indirect links between"
							 " activities same starting time constraints");
							s+="\n\n";
							s+=GeneratePreTranslate::tr("The activities with ids %1 and %2 must be simultaneous (request determined indirectly), but they have common teachers and/or students sets or must be not overlapping")
							 .arg(gt.rules.internalActivitiesList[i].id).arg(gt.rules.internalActivitiesList[j].id);
						
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
				 	
							if(t==0)
								reportIndirect=false;
						}
					}
				}
			}
		}
	}

/*
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=i+1; j<gt.rules.nInternalActivities; j++)
			if(repr.value(i) == repr.value(j)){
				if(activitiesConflictingPercentage[i].value(j, -1)==100){
					oktocontinue=false;
					
					if(reportIndirect){
						QString s=GeneratePreTranslate::tr("You have a set of impossible constraints activities same starting time, considering all the indirect links between"
						 " activities same starting time constraints");
						s+="\n\n";
						s+=GeneratePreTranslate::tr("The activities with ids %1 and %2 must be simultaneous (request determined indirectly), but they have common teachers and/or students sets or must be not overlapping")
						 .arg(gt.rules.internalActivitiesList[i].id).arg(gt.rules.internalActivitiesList[j].id);
					
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							reportIndirect=false;
					}
				}
			}*/
	///////////end added 5.10.0, June 2009
	
	QHash<int, QSet<int> > hashSet;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		assert(repr.contains(i));
		int r=repr.value(i);
		hashSet[r].insert(i); //faster
		/*QSet<int> s;
		if(hashSet.contains(r))
			s=hashSet.value(r);
		s.insert(i);
		hashSet.insert(r, s);*/
	}
	
	reprSameStartingTime=repr;
	reprSameActivitiesSet=hashSet;

	return oktocontinue;
}

void computeActivitiesSameStartingHour()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingHourActivities[i].clear();
		activitiesSameStartingHourPercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
			ConstraintActivitiesSameStartingHour* sst=(ConstraintActivitiesSameStartingHour*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingHourActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingHourPercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingHourActivities[ai1].append(ai2);
							activitiesSameStartingHourPercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingHourActivities[ai1].count()==activitiesSameStartingHourPercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingHourPercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
}

void computeActivitiesSameStartingDay()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingDayActivities[i].clear();
		activitiesSameStartingDayPercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
			ConstraintActivitiesSameStartingDay* sst=(ConstraintActivitiesSameStartingDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingDayActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingDayPercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingDayActivities[ai1].append(ai2);
							activitiesSameStartingDayPercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingDayActivities[ai1].count()==activitiesSameStartingDayPercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingDayPercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
}

////////////teachers' max gaps
//important also for other purposes
bool computeNHoursPerTeacher(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		nHoursPerTeacher[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iTeachersList.count(); j++){
			int tc=act->iTeachersList.at(j);
			nHoursPerTeacher[tc]+=act->duration;
		}
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		if(nHoursPerTeacher[i]>gt.rules.nHoursPerWeek){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
			  " and you have only %3 days x %4 hours in a week.")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		int freeSlots=0;
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				if(!teacherNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
					freeSlots++;
		if(nHoursPerTeacher[i]>freeSlots){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
			  " and you have only %3 free slots from constraints teacher not available and/or break. Maybe you inputted wrong constraints teacher"
			  " not available or break or the number of hours per week is less because of a misunderstanding")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(freeSlots),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
	
	//n days per week has 100% weight
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		if(teachersMaxDaysPerWeekMaxDays[i]>=0){
			int nd=teachersMaxDaysPerWeekMaxDays[i];
			if(nHoursPerTeacher[i] > nd*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
				  " and you have only %3 allowed days from constraint teacher max days per week x %4 hours in a day."
				  " Probably there is an error in your data")
				 .arg(gt.rules.internalTeachersList[i]->name)
				 .arg(nHoursPerTeacher[i])
				 .arg(nd)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return ok;
			}
		}
		
	//n days per week has 100% weight
	//check n days per week together with not available and breaks
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			nAllowedSlotsPerDay[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
					nAllowedSlotsPerDay[d]++;
		}

		int dayAvailable[MAX_DAYS_PER_WEEK];
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			dayAvailable[d]=1;
		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=0;
		
			assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
			for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
				int maxPos=-1, maxVal=-1;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==0)
						if(maxVal<nAllowedSlotsPerDay[d]){
							maxVal=nAllowedSlotsPerDay[d];
							maxPos=d;
						}
				assert(maxPos>=0);
				assert(dayAvailable[maxPos]==0);
				dayAvailable[maxPos]=1;
			}
		}
			
		int total=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			if(dayAvailable[d]==1)
				total+=nAllowedSlotsPerDay[d];
		if(total<nHoursPerTeacher[tc]){
			ok=false;
				
			QString s;
			s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because of too constrained"
			 " teacher max days per week, teacher not available and/or breaks."
			 " The number of total hours for this teacher is"
			 " %2 and the number of available slots is, considering max days per week and all other constraints, %3.")
			 .arg(gt.rules.internalTeachersList[tc]->name)
			 .arg(nHoursPerTeacher[tc])
			 .arg(total);
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
	}
	
	return ok;
}

bool computeTeachersMaxGapsPerWeekPercentage(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerWeekMaxGaps[j]=-1;
		teachersMaxGapsPerWeekPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
			ConstraintTeachersMaxGapsPerWeek* tg=(ConstraintTeachersMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}

		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per week with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
			ConstraintTeachersMaxGapsPerWeek* tg=(ConstraintTeachersMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsPerWeekMaxGaps[j]==-1 
				 ||(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]<=tg->weightPercentage)){
					teachersMaxGapsPerWeekMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPerWeekPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps per week relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsPerWeekMaxGaps[j]==-1 
			 ||(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]<=tg->weightPercentage)){
				teachersMaxGapsPerWeekMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPerWeekPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps per week relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeTeachersMaxGapsPerDayPercentage(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerDayMaxGaps[j]=-1;
		teachersMaxGapsPerDayPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
			ConstraintTeachersMaxGapsPerDay* tg=(ConstraintTeachersMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per day with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}

		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* tg=(ConstraintTeacherMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per day with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
			ConstraintTeachersMaxGapsPerDay* tg=(ConstraintTeachersMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsPerDayMaxGaps[j]==-1 
				 ||(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]<=tg->weightPercentage)){
					teachersMaxGapsPerDayMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPerDayPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps per day relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* tg=(ConstraintTeacherMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsPerDayMaxGaps[j]==-1 
			 ||(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]<=tg->weightPercentage)){
				teachersMaxGapsPerDayMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPerDayPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps per day relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}
/////////////////


///////students' max gaps and early (part 1)
//important also for other purposes
bool computeNHoursPerSubgroup(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		nHoursPerSubgroup[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iSubgroupsList.count(); j++){
			int sb=act->iSubgroupsList.at(j);
			nHoursPerSubgroup[sb]+=act->duration;
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(nHoursPerSubgroup[i]>gt.rules.nHoursPerWeek){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
			  " and you have only %3 days x %4 hours in a week.")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		int freeSlots=0;
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
					freeSlots++;
		if(nHoursPerSubgroup[i]>freeSlots){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
			  " and you have only %3 free slots from constraints students set not available and/or break. Maybe you inputted wrong constraints students set"
			  " not available or break or the number of hours per week is less because of a misunderstanding")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(freeSlots),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
	
	//n days per week has 100% weight
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(subgroupsMaxDaysPerWeekMaxDays[i]>=0){
			int nd=subgroupsMaxDaysPerWeekMaxDays[i];
			if(nHoursPerSubgroup[i] > nd*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
				  " and you have only %3 allowed days from constraint students (set) max days per week x %4 hours in a day."
				  " Probably there is an error in your data")
				 .arg(gt.rules.internalSubgroupsList[i]->name)
				 .arg(nHoursPerSubgroup[i])
				 .arg(nd)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return ok;
			}
		}
		
	//n days per week has 100% weight
	//check n days per week together with not available and breaks
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			nAllowedSlotsPerDay[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
					nAllowedSlotsPerDay[d]++;
		}

		int dayAvailable[MAX_DAYS_PER_WEEK];
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			dayAvailable[d]=1;
		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=0;
		
			assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
			for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
				int maxPos=-1, maxVal=-1;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==0)
						if(maxVal<nAllowedSlotsPerDay[d]){
							maxVal=nAllowedSlotsPerDay[d];
							maxPos=d;
						}
				assert(maxPos>=0);
				assert(dayAvailable[maxPos]==0);
				dayAvailable[maxPos]=1;
			}
		}
			
		int total=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			if(dayAvailable[d]==1)
				total+=nAllowedSlotsPerDay[d];
		if(total<nHoursPerSubgroup[sb]){
			ok=false;
				
			QString s;
			s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because of too constrained"
			 " students (set) max days per week, students set not available and/or breaks."
			 " The number of total hours for this subgroup is"
			 " %2 and the number of available slots is, considering max days per week and all other constraints, %3.")
			 .arg(gt.rules.internalSubgroupsList[sb]->name)
			 .arg(nHoursPerSubgroup[sb])
			 .arg(total);
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
	}
	
	return ok;
}

bool computeMaxDaysPerWeekForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxDaysPerWeekMaxDays[j]=-1;
		teachersMaxDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* tn=(ConstraintTeacherMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]==-1 ||
			 (teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]>=0 && teachersMaxDaysPerWeekMaxDays[tn->teacher_ID] > tn->maxDaysPerWeek)){
				teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersMaxDaysPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			/*else{
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 
				if(t==0)
					break;
			}*/
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK){
			ConstraintTeachersMaxDaysPerWeek* tn=(ConstraintTeachersMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxDaysPerWeekMaxDays[t]==-1 ||
				 (teachersMaxDaysPerWeekMaxDays[t]>=0 && teachersMaxDaysPerWeekMaxDays[t] > tn->maxDaysPerWeek)){
					teachersMaxDaysPerWeekMaxDays[t]=tn->maxDaysPerWeek;
					teachersMaxDaysPerWeekWeightPercentages[t]=tn->weightPercentage;
				}
			}
			/*else{
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 
				if(t==0)
					break;
			}*/
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxDaysPerWeekForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);
				
				if(teachersMaxDaysPerWeekMaxDays[tch]>=0){
					assert(teachersWithMaxDaysPerWeekForActivities[i].indexOf(tch)==-1);
					teachersWithMaxDaysPerWeekForActivities[i].append(tch);
				}
			}
		}
	}
	
	return ok;
}

bool computeMaxDaysPerWeekForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxDaysPerWeekMaxDays[j]=-1;
		subgroupsMaxDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetMaxDaysPerWeek* cn=(ConstraintStudentsSetMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];
			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max days per week with"
				 " weight (percentage) below 100 for students set %1. It is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			foreach(int sb, cn->iSubgroupsList){
				if(subgroupsMaxDaysPerWeekMaxDays[sb]==-1 ||
				 (subgroupsMaxDaysPerWeekMaxDays[sb]>=0 && subgroupsMaxDaysPerWeekMaxDays[sb] > cn->maxDaysPerWeek)){
					subgroupsMaxDaysPerWeekMaxDays[sb]=cn->maxDaysPerWeek;
					subgroupsMaxDaysPerWeekWeightPercentages[sb]=cn->weightPercentage;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK){
			ConstraintStudentsMaxDaysPerWeek* cn=(ConstraintStudentsMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			for(int s=0; s<gt.rules.nInternalSubgroups; s++){
				if(subgroupsMaxDaysPerWeekMaxDays[s]==-1 ||
				 (subgroupsMaxDaysPerWeekMaxDays[s]>=0 && subgroupsMaxDaysPerWeekMaxDays[s] > cn->maxDaysPerWeek)){
					subgroupsMaxDaysPerWeekMaxDays[s]=cn->maxDaysPerWeek;
					subgroupsMaxDaysPerWeekWeightPercentages[s]=cn->weightPercentage;
				}
			}
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxDaysPerWeekForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sb=act->iSubgroupsList.at(j);
				
				if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
					assert(subgroupsWithMaxDaysPerWeekForActivities[i].indexOf(sb)==-1);
					subgroupsWithMaxDaysPerWeekForActivities[i].append(sb);
				}
			}
		}
	}
	
	return ok;
}

bool computeSubgroupsEarlyAndMaxGapsPercentages(QWidget* parent) //st max gaps & early - part 2
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
		subgroupsMaxGapsPerWeekPercentage[i]=-1;
		subgroupsMaxGapsPerWeekMaxGaps[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//students early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//students set early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int q=0; q<se->iSubgroupsList.count(); q++){
				int j=se->iSubgroupsList.at(q);
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//students max gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK){
			ConstraintStudentsMaxGapsPerWeek* sg=(ConstraintStudentsMaxGapsPerWeek*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerWeekPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekMaxGaps[j]==-1 || subgroupsMaxGapsPerWeekMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerWeekMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		//students set max gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
			ConstraintStudentsSetMaxGapsPerWeek* sg=(ConstraintStudentsSetMaxGapsPerWeek*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);
				
				if(subgroupsMaxGapsPerWeekPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekMaxGaps[s]==-1 || subgroupsMaxGapsPerWeekMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerWeekMaxGaps[s] = sg->maxGaps;
				}
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		assert((subgroupsMaxGapsPerWeekPercentage[i]==-1 &&
		 subgroupsMaxGapsPerWeekMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerWeekPercentage[i]>=0 &&
		 subgroupsMaxGapsPerWeekMaxGaps[i]>=0));
	
		bool oksubgroup=true;
		/*if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0
			&& subgroupsNoGapsPercentage[i]==subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i])
				oksubgroup=true;
		else
			oksubgroup=false;*/
			
		if(subgroupsMaxGapsPerWeekPercentage[i]>=0 && subgroupsMaxGapsPerWeekPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have a max gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint max gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have an early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		/*if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because a students early max beginnings at second hour constraint"
			 " exists for this subgroup, and you have not 'max gaps' requirements for this subgroup. "
			 "The algorithm can 1. optimize with 'early' and 'max gaps'"
			 " having the same weight percentage or 2. only 'max gaps' optimization"
			 " without 'early'. Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest of early - max gaps problems"), GeneratePreTranslate::tr("See next incompatibility max gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		if(subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 
		 && subgroupsNoGapsPercentage[i]!=subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]){
		 	oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because early max beginnings at second hour constraint"
			 " has weight percentage %2, and 'max gaps' constraint has weight percentage %3."
			 ". The algorithm can:"
			 "\n1: Optimize with 'early' and 'max gaps' having the same weight percentage or"
			 "\n2. Only 'max gaps' optimization without 'early'."
			 "\nPlease modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]).
			 arg(subgroupsNoGapsPercentage[i]),
			 GeneratePreTranslate::tr("Skip rest of early - max gaps problems"), GeneratePreTranslate::tr("See next incompatibility max gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}*/

		if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0 
		 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek){
		 	oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of days per week is %3 which is less. It must be that the number of"
			 " days per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]).
			 arg(gt.rules.nDaysPerWeek),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		
		if(!oksubgroup)
			ok=false;
	}
	
	return ok;
}

bool computeSubgroupsMaxGapsPerDayPercentages(QWidget* parent)
{
	haveStudentsMaxGapsPerDay=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxGapsPerDayPercentage[i]=-1;
		subgroupsMaxGapsPerDayMaxGaps[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY){
			haveStudentsMaxGapsPerDay=true;
			ConstraintStudentsMaxGapsPerDay* sg=(ConstraintStudentsMaxGapsPerDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerDayPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerDayPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerDayMaxGaps[j]==-1 || subgroupsMaxGapsPerDayMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerDayMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
			haveStudentsMaxGapsPerDay=true;
			ConstraintStudentsSetMaxGapsPerDay* sg=(ConstraintStudentsSetMaxGapsPerDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);
				
				if(subgroupsMaxGapsPerDayPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerDayPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerDayMaxGaps[s]==-1 || subgroupsMaxGapsPerDayMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerDayMaxGaps[s] = sg->maxGaps;
				}
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsMaxGapsPerDayPercentage[i]==-1 &&
		 subgroupsMaxGapsPerDayMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerDayPercentage[i]>=0 &&
		 subgroupsMaxGapsPerDayMaxGaps[i]>=0));
	
		bool oksubgroup=true;

		if(subgroupsMaxGapsPerDayPercentage[i]>=0 && subgroupsMaxGapsPerDayPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have a max gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint max gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}

		if(!oksubgroup)
			ok=false;
	}

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		if(subgroupsMaxGapsPerDayMaxGaps[i]>=0){
			int mgw=gt.rules.nDaysPerWeek*subgroupsMaxGapsPerDayMaxGaps[i];
			assert(mgw>=0);
			if(subgroupsMaxGapsPerWeekMaxGaps[i]==-1 || (subgroupsMaxGapsPerWeekMaxGaps[i]>=0 && subgroupsMaxGapsPerWeekMaxGaps[i]>mgw)){
				subgroupsMaxGapsPerWeekMaxGaps[i]=mgw;
				subgroupsMaxGapsPerWeekPercentage[i]=100.0;
			}
		}
	}
	
	return ok;
}

bool computeNotAllowedTimesPercentages(QWidget* parent)
{
	Matrix1D<bool> allowed;
	allowed.resize(gt.rules.nHoursPerWeek);

	bool ok=true;

	assert(gt.rules.internalStructureComputed);

	breakDayHour.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//BREAK
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int k=0; k<gt.rules.nHoursPerDay; k++)
			breakDayHour[j][k]=false;
			
	subgroupNotAvailableDayHour.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//STUDENTS SET NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				subgroupNotAvailableDayHour[i][j][k]=false;	
	
	teacherNotAvailableDayHour.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//TEACHER NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				teacherNotAvailableDayHour[i][j][k]=false;
	
	notAllowedTimesPercentages.resize(gt.rules.nInternalActivities, gt.rules.nHoursPerWeek);
	//improvement by Volker Dirr (late activities):
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<gt.rules.nHoursPerWeek; j++){
			int h=j/gt.rules.nDaysPerWeek;
			if(h+act->duration <= gt.rules.nHoursPerDay)
				notAllowedTimesPercentages[i][j]=-1;
			else
				notAllowedTimesPercentages[i][j]=100;
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			//TEACHER not available
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
				ConstraintTeacherNotAvailableTimes* tn=(ConstraintTeacherNotAvailableTimes*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int ti=0; ti<act->iTeachersList.count(); ti++)
						if(act->iTeachersList.at(ti)==tn->teacher_ID){
							assert(tn->days.count()==tn->hours.count());
							for(int kk=0; kk<tn->days.count(); kk++){
								int d=tn->days.at(kk);
								int h=tn->hours.at(kk);
								
								for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
									if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<tn->weightPercentage)
										notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=tn->weightPercentage;
							}
							//break; //search no more for teacher -- careful with that
						}
				}

				if(tn->weightPercentage!=100){
					ok=false;

					GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					 "teacher not available with weight percentage less than 100% for teacher %1. Currently, FET can only optimize with "
					 "constraints teacher not available with 100% weight (or no constraint). Please "
					 "modify your data accordingly and try again.").arg(tn->teacher));
			
					return ok;
				}
				else{				
					assert(tn->weightPercentage==100);
					assert(tn->days.count()==tn->hours.count());
					for(int kk=0; kk<tn->days.count(); kk++){
						int d=tn->days.at(kk);
						int h=tn->hours.at(kk);

						teacherNotAvailableDayHour[tn->teacher_ID][d][h]=true;
					}
				}
			}

			//STUDENTS SET not available		
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
				ConstraintStudentsSetNotAvailableTimes* sn=(ConstraintStudentsSetNotAvailableTimes*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int sg=0; sg<act->iSubgroupsList.count(); sg++)
						for(int j=0; j<sn->iSubgroupsList.count(); j++){
							if(act->iSubgroupsList.at(sg)==sn->iSubgroupsList.at(j)){
								assert(sn->days.count()==sn->hours.count());
								for(int kk=0; kk<sn->days.count(); kk++){
									int d=sn->days.at(kk);
									int h=sn->hours.at(kk);

									for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
										if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<sn->weightPercentage)
											notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=sn->weightPercentage;
								}
								//break; //search no more for subgroup - this can bring an improvement in precalculation
								//but needs attention
							}
						}
				}

				if(sn->weightPercentage!=100){
					ok=false;

					GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					 "students set not available with weight percentage less than 100% for students set %1. Currently, FET can only optimize with "
					 "constraints students set not available with 100% weight (or no constraint). Please "
					 "modify your data accordingly and try again.").arg(sn->students));
			
					return ok;
				}
				else{				
					assert(sn->weightPercentage==100);
					for(int q=0; q<sn->iSubgroupsList.count(); q++){
						int ss=sn->iSubgroupsList.at(q);
						assert(sn->days.count()==sn->hours.count());
						for(int kk=0; kk<sn->days.count(); kk++){
							int d=sn->days.at(kk);
							int h=sn->hours.at(kk);

							subgroupNotAvailableDayHour[ss][d][h]=true;
						}
					}
				}
			}
			
			//BREAK
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BREAK_TIMES){
				ConstraintBreakTimes* br=(ConstraintBreakTimes*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];

					assert(br->days.count()==br->hours.count());
					for(int kk=0; kk<br->days.count(); kk++){
						int d=br->days.at(kk);
						int h=br->hours.at(kk);

						for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
							if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<br->weightPercentage)
								notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=br->weightPercentage;
					}
				}

				if(br->weightPercentage!=100){
					ok=false;

					GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					 "break with weight percentage less than 100%. Currently, FET can only optimize with "
					 "constraints break with 100% weight (or no constraint). Please "
					 "modify your data accordingly and try again."));
			
					return ok;
				}
				else{				
					assert(br->weightPercentage==100);
	
					assert(br->days.count()==br->hours.count());
					for(int kk=0; kk<br->days.count(); kk++){
						int d=br->days.at(kk);
						int h=br->hours.at(kk);

						breakDayHour[d][h]=true;
					}
				}
			}

			//ACTIVITY preferred starting time
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* ap=(ConstraintActivityPreferredStartingTime*)gt.rules.internalTimeConstraintsList[i];
				
				if(ap->day>=0 && ap->hour>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(d!=ap->day || h!=ap->hour)
								if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else if(ap->day>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(d!=ap->day)
								if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else if(ap->hour>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(h!=ap->hour)
								if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					 "activity preferred starting time with no day nor hour selected (for activity with id==%1). "
					 "Please modify your data accordingly (remove or edit constraint) and try again.")
					 .arg(gt.rules.internalActivitiesList[ap->activityIndex].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					1, 0 );

					if(t==0)
						break;
					//assert(0);
				}
			}	

			//ACTIVITY preferred starting times
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
				ConstraintActivityPreferredStartingTimes* ap=(ConstraintActivityPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
				
				int ai=ap->activityIndex;
				
				//Matrix1D<bool> allowed;
				//allowed.resize(gt.rules.nHoursPerWeek);
				//bool allowed[MAX_HOURS_PER_WEEK];
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
						
				for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
					int d=ap->days_L[m];
					int h=ap->hours_L[m];
					
					if(d>=0 && h>=0){
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					else if(d>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							allowed[d+hh*gt.rules.nDaysPerWeek]=true;
					}
					else if(h>=0){
						for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
							allowed[dd+h*gt.rules.nDaysPerWeek]=true;
					}
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					if(!allowed[k])
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
			}
			
			//ACTIVITIES preferred starting times
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
				ConstraintActivitiesPreferredStartingTimes* ap=(ConstraintActivitiesPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
				
				for(int j=0; j<ap->nActivities; j++){
					int ai=ap->activitiesIndices[j];
					
					//Matrix1D<bool> allowed;
					//allowed.resize(gt.rules.nHoursPerWeek);
					//bool allowed[MAX_HOURS_PER_WEEK];
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						allowed[k]=false;
						
					for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
						int d=ap->days_L[m];
						int h=ap->hours_L[m];
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						if(!allowed[k])
							if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
								notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
			//subactivities preferred starting times
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
				ConstraintSubactivitiesPreferredStartingTimes* ap=(ConstraintSubactivitiesPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
				
				for(int j=0; j<ap->nActivities; j++){
					int ai=ap->activitiesIndices[j];
					
					//Matrix1D<bool> allowed;
					//allowed.resize(gt.rules.nHoursPerWeek);
					//bool allowed[MAX_HOURS_PER_WEEK];
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						allowed[k]=false;
						
					for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
						int d=ap->days_L[m];
						int h=ap->hours_L[m];
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						if(!allowed[k])
							if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
								notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
			//ACTIVITY preferred time slots
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
				ConstraintActivityPreferredTimeSlots* ap=(ConstraintActivityPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
				
				int ai=ap->p_activityIndex;
				
				//Matrix1D<bool> allowed;
				//allowed.resize(gt.rules.nHoursPerWeek);
				//bool allowed[MAX_HOURS_PER_WEEK];
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
						
				for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
					int d=ap->p_days_L[m];
					int h=ap->p_hours_L[m];
					
					if(d>=0 && h>=0){
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					else if(d>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							allowed[d+hh*gt.rules.nDaysPerWeek]=true;
					}
					else if(h>=0){
						for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
							allowed[dd+h*gt.rules.nDaysPerWeek]=true;
					}
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++){
					int d=k%gt.rules.nDaysPerWeek;
					int h=k/gt.rules.nDaysPerWeek;
					
					bool ok=true;
					
					for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
						assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
						if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
							ok=false;
							break;
						}
					}
				
					if(!ok)
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
			
			//ACTIVITIES preferred time slots
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
				ConstraintActivitiesPreferredTimeSlots* ap=(ConstraintActivitiesPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
				
				for(int j=0; j<ap->p_nActivities; j++){
					int ai=ap->p_activitiesIndices[j];
					
					//Matrix1D<bool> allowed;
					//allowed.resize(gt.rules.nHoursPerWeek);
					//bool allowed[MAX_HOURS_PER_WEEK];
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						allowed[k]=false;
						
					for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
						int d=ap->p_days_L[m];
						int h=ap->p_hours_L[m];
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					
					for(int k=0; k<gt.rules.nHoursPerWeek; k++){
						int d=k%gt.rules.nDaysPerWeek;
						int h=k/gt.rules.nDaysPerWeek;
						
						bool ok=true;
						
						for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
							assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
							if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
								ok=false;
								break;
							}
						}
				
						if(!ok)
							if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
								notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
					}
				}
			}
			//subactivities preferred time slots 
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
				ConstraintSubactivitiesPreferredTimeSlots* ap=(ConstraintSubactivitiesPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
				
				for(int j=0; j<ap->p_nActivities; j++){
					int ai=ap->p_activitiesIndices[j];
					
					//Matrix1D<bool> allowed;
					//allowed.resize(gt.rules.nHoursPerWeek);
					//bool allowed[MAX_HOURS_PER_WEEK];
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						allowed[k]=false;
						
					for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
						int d=ap->p_days_L[m];
						int h=ap->p_hours_L[m];
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					
					for(int k=0; k<gt.rules.nHoursPerWeek; k++){
						int d=k%gt.rules.nDaysPerWeek;
						int h=k/gt.rules.nDaysPerWeek;
						
						bool ok=true;
						
						for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
							assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
							if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
								ok=false;
								break;
							}
						}
				
						if(!ok)
							if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
								notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
					}
				}
			}
	}
	
	return ok;
}

bool computeMinDays(QWidget* parent)
{
	QSet<ConstraintMinDaysBetweenActivities*> mdset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minDaysListOfActivities[j].clear();
		minDaysListOfMinDays[j].clear();
		minDaysListOfConsecutiveIfSameDay[j].clear();
		minDaysListOfWeightPercentages[j].clear();
				
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinDaysBetweenActivities* md=
			 (ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){						
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min days with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->minDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						minDaysListOfActivities[ai1].append(ai2);
						minDaysListOfMinDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						minDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						minDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMaxDays(QWidget* parent)
{
	QSet<ConstraintMaxDaysBetweenActivities*> mdset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxDaysListOfActivities[j].clear();
		maxDaysListOfMaxDays[j].clear();
		maxDaysListOfWeightPercentages[j].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMaxDaysBetweenActivities* md=
			 (ConstraintMaxDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){						
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint max days between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->maxDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						maxDaysListOfActivities[ai1].append(ai2);
						maxDaysListOfMaxDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						maxDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						//maxDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMinGapsBetweenActivities(QWidget* parent)
{
	QSet<ConstraintMinGapsBetweenActivities*> mgset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minGapsBetweenActivitiesListOfActivities[j].clear();
		minGapsBetweenActivitiesListOfMinGaps[j].clear();
		minGapsBetweenActivitiesListOfWeightPercentages[j].clear();
				
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinGapsBetweenActivities* mg=
			 (ConstraintMinGapsBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			 
			assert(mg->_n_activities==mg->_activities.count());
			
			for(int j=0; j<mg->_n_activities; j++){
				int ai1=mg->_activities[j];
				for(int k=0; k<mg->_n_activities; k++)
					if(j!=k){
						int ai2=mg->_activities[k];
						if(ai1==ai2){						
							ok=false;
							
							if(!mgset.contains(mg)){
								mgset.insert(mg);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min gaps between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(mg->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=mg->minGaps;
						
						minGapsBetweenActivitiesListOfActivities[ai1].append(ai2);
						minGapsBetweenActivitiesListOfMinGaps[ai1].append(m);
						assert(mg->weightPercentage >=0 && mg->weightPercentage<=100);
						minGapsBetweenActivitiesListOfWeightPercentages[ai1].append(mg->weightPercentage);
					}
			}
		}

	return ok;
}

bool computeActivitiesConflictingPercentage(QWidget* parent)
{
	//get maximum weight percent of a basic time constraint
	double m=-1;
	
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			ok=true;
			if(gt.rules.internalTimeConstraintsList[i]->weightPercentage>m)
				m=gt.rules.internalTimeConstraintsList[i]->weightPercentage;
		}
		
	if(m<100)
		ok=false;
		
	if(!ok || m<100){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot generate, because you do not have a constraint of type basic compulsory time or its weight is lower than 100.0%.")
		 +" "+
		 GeneratePreTranslate::tr("Please add a constraint of this type with weight 100%.")
		 +" "+
		 GeneratePreTranslate::tr("You can add this constraint from the menu Data -> Time constraints -> Miscellaneous -> Basic compulsory time constraints.")
		 +"\n\n"+
		 GeneratePreTranslate::tr("Explanation:")
		 +" "+
		 GeneratePreTranslate::tr("Each time you create a new file, it contains an automatically added constraint of this type.")
		 +" "+
		 GeneratePreTranslate::tr("For complete flexibility, you are allowed to remove it (even if this is a wrong idea).")
		 +" "+
		 GeneratePreTranslate::tr("Maybe you removed it by mistake from your file.")
		 +" "+
		 GeneratePreTranslate::tr("By adding it again, everything should be all right.")
		 );
		return false;
	}
	
	assert(m>=0 && m<=100);
	assert(m==100);

	//compute conflicting
	activitiesConflictingPercentage.resize(gt.rules.nInternalActivities);
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i].clear();
		
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i].insert(i, 100);

	QProgressDialog progress(parent);
	progress.setWindowTitle(GeneratePreTranslate::tr("Precomputing", "Title of a progress dialog"));
	progress.setLabelText(GeneratePreTranslate::tr("Precomputing ... please wait"));
	progress.setRange(0, qMax(gt.rules.nInternalTeachers+gt.rules.nInternalSubgroups, 1));
	progress.setModal(true);
	
	int ttt=0;
	
	for(int t=0; t<gt.rules.nInternalTeachers; t++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			GeneratePreIrreconcilableMessage::information(parent, GeneratePreTranslate::tr("FET information"), GeneratePreTranslate::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		foreach(int i, gt.rules.internalTeachersList[t]->activitiesForTeacher)
			foreach(int j, gt.rules.internalTeachersList[t]->activitiesForTeacher)
				activitiesConflictingPercentage[i].insert(j, 100);
	}
	
	for(int s=0; s<gt.rules.nInternalSubgroups; s++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			GeneratePreIrreconcilableMessage::information(parent, GeneratePreTranslate::tr("FET information"), GeneratePreTranslate::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		foreach(int i, gt.rules.internalSubgroupsList[s]->activitiesForSubgroup)
			foreach(int j, gt.rules.internalSubgroupsList[s]->activitiesForSubgroup)
				activitiesConflictingPercentage[i].insert(j, 100);
	}

	progress.setValue(qMax(gt.rules.nInternalTeachers+gt.rules.nInternalSubgroups, 1));
	
	//new volker (start)
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
		if(tc->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			if(tc->weightPercentage==100.0){
				ConstraintActivitiesNotOverlapping* cno=(ConstraintActivitiesNotOverlapping*) tc;
				
				for(int a=0; a<cno->_n_activities; a++){
					for(int b=0; b<cno->_n_activities; b++){
						if(cno->_activities[a]!=cno->_activities[b]){
							activitiesConflictingPercentage[cno->_activities[a]].insert(cno->_activities[b], 100);
						}
					}
				}
			}
			else{
				ConstraintActivitiesNotOverlapping* cno=(ConstraintActivitiesNotOverlapping*) tc;
				
				int ww=int(cno->weightPercentage);
				if(ww>100)
					ww=100;
			
				for(int a=0; a<cno->_n_activities; a++){
					for(int b=0; b<cno->_n_activities; b++){
						if(cno->_activities[a]!=cno->_activities[b]){
							if(activitiesConflictingPercentage[cno->_activities[a]].value(cno->_activities[b], -1) < ww)
								activitiesConflictingPercentage[cno->_activities[a]].insert(cno->_activities[b], ww);
						}
					}
				}
			}
		}
	}
	//new volker (end)

	return true;
}

//old
/*
#if 0
bool computeActivitiesConflictingPercentage()
{
	//get maximum weight percent of a basic time constraint
	double m=-1;
	
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			ok=true;
			if(gt.rules.internalTimeConstraintsList[i]->weightPercentage>m)
				m=gt.rules.internalTimeConstraintsList[i]->weightPercentage;
		}
		
	if(m<100)
		ok=false;
		
	if(!ok || m<100){
		QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot optimize, because you have no basic time constraints or its weight is lower than 100.0%. "
		 "Please add a basic time constraint (100% weight)"));
		return false;
	}
	
	assert(m>=0 && m<=100);
	assert(m==100);

	//compute conflictig
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i][i]=100;
		
	QProgressDialog progress(parent);
	progress.setLabelText(GeneratePreTranslate::tr("Precomputing ... please wait"));
	progress.setRange(0, gt.rules.nInternalActivities*(gt.rules.nInternalActivities-1)/2);
	progress.setModal(true);
	
	int ttt=0;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		progress.setValue(ttt);
		pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(parent, GeneratePreTranslate::tr("FET information"), GeneratePreTranslate::tr("Canceled"));
			return false;
		}
			
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			ttt++;
		
			//see if they share a teacher
			bool shareTeacher=false;
			foreach(QString tni, gt.rules.internalActivitiesList[i].teachersNames){
				foreach(QString tnj, gt.rules.internalActivitiesList[j].teachersNames){
					if(tni==tnj){
						shareTeacher=true;
						break;
					}
				}
				if(shareTeacher)
					break;
			}
			
			bool shareStudentsSet=false;
			foreach(QString ssi, gt.rules.internalActivitiesList[i].studentsNames){
				foreach(QString ssj, gt.rules.internalActivitiesList[j].studentsNames){
					if(gt.rules.studentsSetsRelated(ssi, ssj)){
						shareStudentsSet=true;
						break;
					}
				}
				if(shareStudentsSet)
					break;
			}
			
			if(shareTeacher||shareStudentsSet)
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=100;
			else
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=-1;
		}
	}

	progress.setValue(gt.rules.nInternalActivities*(gt.rules.nInternalActivities-1)/2);
		
	return true;
}
#endif
//endif 0
*/

void computeConstrTwoActivitiesConsecutive()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesConsecutivePercentages[i].clear();
		constrTwoActivitiesConsecutiveActivities[i].clear();

		inverseConstrTwoActivitiesConsecutivePercentages[i].clear();
		inverseConstrTwoActivitiesConsecutiveActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE){
			ConstraintTwoActivitiesConsecutive* c2=(ConstraintTwoActivitiesConsecutive*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesConsecutiveActivities[fai].indexOf(sai); 
			if(j==-1){
				constrTwoActivitiesConsecutiveActivities[fai].append(sai);
				constrTwoActivitiesConsecutivePercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesConsecutivePercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesConsecutivePercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstrTwoActivitiesConsecutiveActivities[sai].indexOf(fai); 
			if(j==-1){
				inverseConstrTwoActivitiesConsecutiveActivities[sai].append(fai);
				inverseConstrTwoActivitiesConsecutivePercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstrTwoActivitiesConsecutivePercentages[sai].at(j)<c2->weightPercentage){
				inverseConstrTwoActivitiesConsecutivePercentages[sai][j]=c2->weightPercentage;
			}
		}
}

void computeConstrTwoActivitiesGrouped()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesGroupedPercentages[i].clear();
		constrTwoActivitiesGroupedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED){
			ConstraintTwoActivitiesGrouped* c2=(ConstraintTwoActivitiesGrouped*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesGroupedActivities[fai].indexOf(sai); 
			if(j==-1){
				constrTwoActivitiesGroupedActivities[fai].append(sai);
				constrTwoActivitiesGroupedPercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesGroupedPercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesGroupedPercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=constrTwoActivitiesGroupedActivities[sai].indexOf(fai); 
			if(j==-1){
				constrTwoActivitiesGroupedActivities[sai].append(fai);
				constrTwoActivitiesGroupedPercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesGroupedPercentages[sai].at(j)<c2->weightPercentage){
				constrTwoActivitiesGroupedPercentages[sai][j]=c2->weightPercentage;
			}
		}
}

void computeConstrThreeActivitiesGrouped()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrThreeActivitiesGroupedPercentages[i].clear();
		constrThreeActivitiesGroupedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_THREE_ACTIVITIES_GROUPED){
			ConstraintThreeActivitiesGrouped* c3=(ConstraintThreeActivitiesGrouped*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c3->firstActivityIndex;
			int sai=c3->secondActivityIndex;
			int tai=c3->thirdActivityIndex;

			QPair<int, int> p23(sai, tai);
			int j=constrThreeActivitiesGroupedActivities[fai].indexOf(p23);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[fai].append(p23);
				constrThreeActivitiesGroupedPercentages[fai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[fai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[fai][j]=c3->weightPercentage;
			}

			QPair<int, int> p13(fai, tai);
			j=constrThreeActivitiesGroupedActivities[sai].indexOf(p13);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[sai].append(p13);
				constrThreeActivitiesGroupedPercentages[sai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[sai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[sai][j]=c3->weightPercentage;
			}

			QPair<int, int> p12(fai, sai);
			j=constrThreeActivitiesGroupedActivities[tai].indexOf(p12);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[tai].append(p12);
				constrThreeActivitiesGroupedPercentages[tai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[tai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[tai][j]=c3->weightPercentage;
			}
		}
}

void computeConstrTwoActivitiesOrdered()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesOrderedPercentages[i].clear();
		constrTwoActivitiesOrderedActivities[i].clear();

		inverseConstrTwoActivitiesOrderedPercentages[i].clear();
		inverseConstrTwoActivitiesOrderedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED){
			ConstraintTwoActivitiesOrdered* c2=(ConstraintTwoActivitiesOrdered*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesOrderedActivities[fai].indexOf(sai); 
			if(j==-1){
				constrTwoActivitiesOrderedActivities[fai].append(sai);
				constrTwoActivitiesOrderedPercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesOrderedPercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesOrderedPercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstrTwoActivitiesOrderedActivities[sai].indexOf(fai); 
			if(j==-1){
				inverseConstrTwoActivitiesOrderedActivities[sai].append(fai);
				inverseConstrTwoActivitiesOrderedPercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstrTwoActivitiesOrderedPercentages[sai].at(j)<c2->weightPercentage){
				inverseConstrTwoActivitiesOrderedPercentages[sai][j]=c2->weightPercentage;
			}
		}
}

bool computeActivityEndsStudentsDayPercentages(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityEndsStudentsDayPercentages[ai]=-1;
		
	haveActivityEndsStudentsDay=false;
		
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
			haveActivityEndsStudentsDay=true;
		
			ConstraintActivityEndsStudentsDay* cae=(ConstraintActivityEndsStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activity activity ends students day for activity with id==%1 with weight percentage under 100%. "
				 "Constraint activity ends students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityEndsStudentsDayPercentages[ai] < cae->weightPercentage)
				activityEndsStudentsDayPercentages[ai] = cae->weightPercentage;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			haveActivityEndsStudentsDay=true;
		
			ConstraintActivitiesEndStudentsDay* cae=(ConstraintActivitiesEndStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activities end students day with weight percentage under 100%. "
				 "Constraint activities end students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			for(int i=0; i<cae->nActivities; i++){
				int ai=cae->activitiesIndices[i];
				if(activityEndsStudentsDayPercentages[ai] < cae->weightPercentage)
					activityEndsStudentsDayPercentages[ai] = cae->weightPercentage;
			}
		}
	}
		
	return ok;
}

bool checkMinDays100Percent(QWidget* parent)
{
	bool ok=true;
	
	//int daysTeacherIsAvailable[MAX_TEACHERS];

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		daysTeacherIsAvailable[tc]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysTeacherIsAvailable[tc]++;
		}
			
		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
			assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
			daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
		}
	}

	//int daysSubgroupIsAvailable[MAX_TOTAL_SUBGROUPS];

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		daysSubgroupIsAvailable[sb]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysSubgroupIsAvailable[sb]++;
		}

		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
			assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
			daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintMinDaysBetweenActivities* md=(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			if(md->minDays>=1){
				int na=md->_n_activities;
				int nd=md->minDays;
				if((na-1)*nd+1 > gt.rules.nDaysPerWeek){
					ok=false;
						
					QString s=GeneratePreTranslate::tr("%1 cannot be respected because it contains %2 activities,"
					 " has weight 100% and has min number of days between activities=%3. The minimum required number of days per week for"
					 " that would be (nactivities-1)*mindays+1=%4, and you have only %5 days per week - impossible. Please correct this constraint.", "%1 is the detailed description of a constraint"
					)
					 .arg(md->getDetailedDescription(gt.rules))
					 .arg(na)
					 .arg(nd)
					 .arg((na-1)*nd+1)
					 .arg(gt.rules.nDaysPerWeek)
					 ;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, 
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return ok;
				}
			}
			
			if(md->minDays>=1){
				//int requestedDaysForTeachers[MAX_TEACHERS];
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					requestedDaysForTeachers[tc]=0;
				//int requestedDaysForSubgroups[MAX_TOTAL_SUBGROUPS];
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					requestedDaysForSubgroups[sb]=0;
			
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
						int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
						requestedDaysForTeachers[tc]++;
					}
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
						int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
						requestedDaysForSubgroups[sb]++;
					}
				}
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					if(requestedDaysForTeachers[tc]>daysTeacherIsAvailable[tc]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
						 " %3 available days from teacher not available, breaks and teacher max days per week."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, 
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					if(requestedDaysForSubgroups[sb]>daysSubgroupIsAvailable[sb]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
						 " %3 available days from students set not available and breaks."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						 )
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, 
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
			}
		}
	}
	
	return ok;
}
	
bool checkMinDaysConsecutiveIfSameDay(QWidget* parent)
{
	bool ok=true;
	
	//int daysTeacherIsAvailable[MAX_TEACHERS];

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		daysTeacherIsAvailable[tc]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysTeacherIsAvailable[tc]++;
		}
			
		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
			assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
			daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
		}
	}

	//int daysSubgroupIsAvailable[MAX_TOTAL_SUBGROUPS];

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		daysSubgroupIsAvailable[sb]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysSubgroupIsAvailable[sb]++;
		}

		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
			assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
			daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* md=(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			if(md->consecutiveIfSameDay){
				//int nReqForTeacher[MAX_TEACHERS];				
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					nReqForTeacher[tc]=0;
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
						int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
						nReqForTeacher[tc]++;
					}
				}			
			
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
					if(2*daysTeacherIsAvailable[tc] < nReqForTeacher[tc]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
						 " %3 available days. You specified for this constraint consecutive if same day=true."
						 " Currently FET cannot put more than 2 activities in the same day"
						 " if consecutive if same day is true. You have 2*available days<number of activities in this constraint."
						 " This is a very unlikely situation, that is why I didn't care too much about it."
						 " If you encounter it, please please modify your file (uncheck consecutive if same day"
						 " or add other activities with larger duration).", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, 
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
						
						if(t==0)
							return ok;
					}	
				}

				//int nReqForSubgroup[MAX_TOTAL_SUBGROUPS];
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					nReqForSubgroup[sb]=0;
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
						int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
						nReqForSubgroup[sb]++;
					}
				}			
			
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
					if(2*daysSubgroupIsAvailable[sb] < nReqForSubgroup[sb]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
						 " %3 available days. You specified for this constraint consecutive if same day=true."
						 " Currently FET cannot put more than 2 activities in the same day"
						 " if consecutive if same day is true. You have 2*available days<number of activities in this constraint."
						 " This is a very unlikely situation, that is why I didn't care too much about it."
						 " If you encounter it, please modify your file (uncheck consecutive if same day"
						 " or add other activities with larger duration).", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, 
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				}
			}
		}
	}
	
	return ok;
}

bool computeTeachersIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersIntervalMaxDaysPerWeekPercentages1[i]=-1.0;
		teachersIntervalMaxDaysPerWeekMaxDays1[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalStart1[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalEnd1[i]=-1;

		teachersIntervalMaxDaysPerWeekPercentages2[i]=-1.0;
		teachersIntervalMaxDaysPerWeekMaxDays2[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalStart2[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalEnd2[i]=-1;

		teachersIntervalMaxDaysPerWeekPercentages3[i]=-1.0;
		teachersIntervalMaxDaysPerWeekMaxDays3[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalStart3[i]=-1;
		teachersIntervalMaxDaysPerWeekIntervalEnd3[i]=-1;
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherIntervalMaxDaysPerWeek* tn=(ConstraintTeacherIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher interval max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersIntervalMaxDaysPerWeekPercentages1[tn->teacher_ID]==-1){
				teachersIntervalMaxDaysPerWeekPercentages1[tn->teacher_ID]=tn->weightPercentage;
				teachersIntervalMaxDaysPerWeekMaxDays1[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersIntervalMaxDaysPerWeekIntervalStart1[tn->teacher_ID]=tn->startHour;
				teachersIntervalMaxDaysPerWeekIntervalEnd1[tn->teacher_ID]=tn->endHour;
			}
			else if(teachersIntervalMaxDaysPerWeekPercentages2[tn->teacher_ID]==-1){
				teachersIntervalMaxDaysPerWeekPercentages2[tn->teacher_ID]=tn->weightPercentage;
				teachersIntervalMaxDaysPerWeekMaxDays2[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersIntervalMaxDaysPerWeekIntervalStart2[tn->teacher_ID]=tn->startHour;
				teachersIntervalMaxDaysPerWeekIntervalEnd2[tn->teacher_ID]=tn->endHour;
			}
			else if(teachersIntervalMaxDaysPerWeekPercentages3[tn->teacher_ID]==-1){
				teachersIntervalMaxDaysPerWeekPercentages3[tn->teacher_ID]=tn->weightPercentage;
				teachersIntervalMaxDaysPerWeekMaxDays3[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersIntervalMaxDaysPerWeekIntervalStart3[tn->teacher_ID]=tn->startHour;
				teachersIntervalMaxDaysPerWeekIntervalEnd3[tn->teacher_ID]=tn->endHour;
			}
			else{
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has more than three constraints interval max days per week"
				 ". Please modify your data correspondingly (leave maximum three constraints of type"
				 " constraint teacher(s) interval max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeachersIntervalMaxDaysPerWeek* tn=(ConstraintTeachersIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersIntervalMaxDaysPerWeekPercentages1[tch]==-1){
					teachersIntervalMaxDaysPerWeekPercentages1[tch]=tn->weightPercentage;
					teachersIntervalMaxDaysPerWeekMaxDays1[tch]=tn->maxDaysPerWeek;
					teachersIntervalMaxDaysPerWeekIntervalStart1[tch]=tn->startHour;
					teachersIntervalMaxDaysPerWeekIntervalEnd1[tch]=tn->endHour;
				}
				else if(teachersIntervalMaxDaysPerWeekPercentages2[tch]==-1){
					teachersIntervalMaxDaysPerWeekPercentages2[tch]=tn->weightPercentage;
					teachersIntervalMaxDaysPerWeekMaxDays2[tch]=tn->maxDaysPerWeek;
					teachersIntervalMaxDaysPerWeekIntervalStart2[tch]=tn->startHour;
					teachersIntervalMaxDaysPerWeekIntervalEnd2[tch]=tn->endHour;
				}
				else if(teachersIntervalMaxDaysPerWeekPercentages3[tch]==-1){
					teachersIntervalMaxDaysPerWeekPercentages3[tch]=tn->weightPercentage;
					teachersIntervalMaxDaysPerWeekMaxDays3[tch]=tn->maxDaysPerWeek;
					teachersIntervalMaxDaysPerWeekIntervalStart3[tch]=tn->startHour;
					teachersIntervalMaxDaysPerWeekIntervalEnd3[tch]=tn->endHour;
				}
				else{
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has more than three constraints interval max days per week"
					 ". Please modify your data correspondingly (leave maximum three constraints of type"
					 " constraint teacher(s) interval max days per week for each teacher) and try again")
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 
					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}

bool computeSubgroupsIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsIntervalMaxDaysPerWeekPercentages1[i]=-1.0;
		subgroupsIntervalMaxDaysPerWeekMaxDays1[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalStart1[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalEnd1[i]=-1;

		subgroupsIntervalMaxDaysPerWeekPercentages2[i]=-1.0;
		subgroupsIntervalMaxDaysPerWeekMaxDays2[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalStart2[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalEnd2[i]=-1;

		subgroupsIntervalMaxDaysPerWeekPercentages3[i]=-1.0;
		subgroupsIntervalMaxDaysPerWeekMaxDays3[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalStart3[i]=-1;
		subgroupsIntervalMaxDaysPerWeekIntervalEnd3[i]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* cn=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set interval max days per week with"
				 " weight (percentage) below 100 for students set %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			foreach(int sbg, cn->iSubgroupsList){
				if(subgroupsIntervalMaxDaysPerWeekPercentages1[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages1[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays1[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart1[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd1[sbg]=cn->endHour;
				}
				else if(subgroupsIntervalMaxDaysPerWeekPercentages2[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages2[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays2[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart2[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd2[sbg]=cn->endHour;
				}
				else if(subgroupsIntervalMaxDaysPerWeekPercentages3[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages3[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays3[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart3[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd3[sbg]=cn->endHour;
				}
				else{
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because it has more than three constraints interval max days per week"
					 ". Please modify your data correspondingly (leave maximum three constraints of type"
					 " constraint students (set) interval max days per week for each subgroup) and try again")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsIntervalMaxDaysPerWeek* cn=(ConstraintStudentsIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 //.arg(cn->students),
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
			//foreach(int sbg, cn->iSubgroupsList){
				if(subgroupsIntervalMaxDaysPerWeekPercentages1[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages1[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays1[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart1[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd1[sbg]=cn->endHour;
				}
				else if(subgroupsIntervalMaxDaysPerWeekPercentages2[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages2[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays2[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart2[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd2[sbg]=cn->endHour;
				}
				else if(subgroupsIntervalMaxDaysPerWeekPercentages3[sbg]==-1){
					subgroupsIntervalMaxDaysPerWeekPercentages3[sbg]=cn->weightPercentage;
					subgroupsIntervalMaxDaysPerWeekMaxDays3[sbg]=cn->maxDaysPerWeek;
					subgroupsIntervalMaxDaysPerWeekIntervalStart3[sbg]=cn->startHour;
					subgroupsIntervalMaxDaysPerWeekIntervalEnd3[sbg]=cn->endHour;
				}
				else{
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because it has more than three constraints interval max days per week"
					 ". Please modify your data correspondingly (leave maximum three constraints of type"
					 " constraint students (set) interval max days per week for each subgroup) and try again")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 
					if(t==0)
						return false;
				}
			}
		}
	}

	return ok;
}

//2011-09-25
bool computeActivitiesOccupyMaxTimeSlotsFromSelection(QWidget* parent)
{
	bool ok=true;
	
	aomtsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		aomtsListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION){
			if(!haveActivitiesOccupyOrSimultaneousConstraints)
				haveActivitiesOccupyOrSimultaneousConstraints=true;

			ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* cn=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy max time slots from selection'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesOccupyMaxTimeSlotsFromSelection_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.activitiesSet=item.activitiesList.toSet();
			item.maxOccupiedTimeSlots=cn->maxOccupiedTimeSlots;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
			
			aomtsList.append(item);
			ActivitiesOccupyMaxTimeSlotsFromSelection_item* p_item=&aomtsList[aomtsList.count()-1];
			foreach(int ai, cn->_activitiesIndices)
				aomtsListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2011-09-30
bool computeActivitiesMaxSimultaneousInSelectedTimeSlots(QWidget* parent)
{
	bool ok=true;
	
	amsistsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		amsistsListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
			if(!haveActivitiesOccupyOrSimultaneousConstraints)
				haveActivitiesOccupyOrSimultaneousConstraints=true;
			
			ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* cn=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities max simultaneous in selected time slots'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesMaxSimultaneousInSelectedTimeSlots_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.activitiesSet=item.activitiesList.toSet();
			item.maxSimultaneous=cn->maxSimultaneous;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
			
			amsistsList.append(item);
			ActivitiesMaxSimultaneousInSelectedTimeSlots_item* p_item=&amsistsList[amsistsList.count()-1];
			foreach(int ai, cn->_activitiesIndices)
				amsistsListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2012-04-29
bool computeActivitiesOccupyMaxDifferentRooms(QWidget* parent)
{
	bool ok=true;
	
	aomdrList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		aomdrListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS){
			ConstraintActivitiesOccupyMaxDifferentRooms* cn=(ConstraintActivitiesOccupyMaxDifferentRooms*)gt.rules.internalSpaceConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy max different rooms'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesOccupyMaxDifferentRooms_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.activitiesSet=item.activitiesList.toSet();
			item.maxDifferentRooms=cn->maxDifferentRooms;
			
			aomdrList.append(item);
			ActivitiesOccupyMaxDifferentRooms_item* p_item=&aomdrList[aomdrList.count()-1];
			foreach(int ai, cn->_activitiesIndices)
				aomdrListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2013-09-14
bool computeActivitiesSameRoomIfConsecutive(QWidget* parent)
{
	bool ok=true;
	
	asricList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		asricListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE){
			ConstraintActivitiesSameRoomIfConsecutive* cn=(ConstraintActivitiesSameRoomIfConsecutive*)gt.rules.internalSpaceConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities same room if consecutive'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesSameRoomIfConsecutive_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.activitiesSet=item.activitiesList.toSet();
			
			asricList.append(item);
			ActivitiesSameRoomIfConsecutive_item* p_item=&asricList[asricList.count()-1];
			foreach(int ai, cn->_activitiesIndices)
				asricListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

bool computeBasicSpace(QWidget* parent)
{
	double m=-1;
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++)
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
			ok=true;
			if(gt.rules.internalSpaceConstraintsList[i]->weightPercentage>m)
				m=gt.rules.internalSpaceConstraintsList[i]->weightPercentage;
		}
		
	if(m<100)
		ok=false;
		
	if(!ok || m<100){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot generate, because you do not have a constraint of type basic compulsory space or its weight is lower than 100.0%.")
		 +" "+
		 GeneratePreTranslate::tr("Please add a constraint of this type with weight 100%.")
		 +" "+
		 GeneratePreTranslate::tr("You can add this constraint from the menu Data -> Space constraints -> Miscellaneous -> Basic compulsory space constraints.")
		 +"\n\n"+
		 GeneratePreTranslate::tr("Explanation:")
		 +" "+
		 GeneratePreTranslate::tr("Each time you create a new file, it contains an automatically added constraint of this type.")
		 +" "+
		 GeneratePreTranslate::tr("For complete flexibility, you are allowed to remove it (even if this is a wrong idea).")
		 +" "+
		 GeneratePreTranslate::tr("Maybe you removed it by mistake from your file.")
		 +" "+
		 GeneratePreTranslate::tr("By adding it again, everything should be all right.")
		 );
		return false;
	}
	return ok;
}

bool computeNotAllowedRoomTimePercentages()
{
	notAllowedRoomTimePercentages.resize(gt.rules.nInternalRooms, gt.rules.nHoursPerWeek);

	for(int i=0; i<gt.rules.nInternalRooms; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			notAllowedRoomTimePercentages[i][j]=-1;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* rn=(ConstraintRoomNotAvailableTimes*)gt.rules.internalSpaceConstraintsList[i];
			
			assert(rn->days.count()==rn->hours.count());
			for(int kk=0; kk<rn->days.count(); kk++){
				int d=rn->days.at(kk);
				int h=rn->hours.at(kk);

				if(notAllowedRoomTimePercentages[rn->room_ID][d+h*gt.rules.nDaysPerWeek]<rn->weightPercentage)
					notAllowedRoomTimePercentages[rn->room_ID][d+h*gt.rules.nDaysPerWeek]=rn->weightPercentage;
			}
		}
	}
	
	return true;
}

bool computeActivitiesRoomsPreferences(QWidget* parent)
{
	Matrix1D<QList<SpaceConstraint*> > constraintsForActivity;
	
	constraintsForActivity.resize(gt.rules.nInternalActivities);

	//to disallow duplicates
	QSet<QString> studentsSetHomeRoom;
	QSet<QString> teachersHomeRoom;
	/*QSet<QString> subjectsPreferredRoom;
	QSet<QPair<QString, QString> > subjectsActivityTagsPreferredRoom;*/
	//QSet<int> activitiesPreferredRoom;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constraintsForActivity[i].clear();
	
		unspecifiedPreferredRoom[i]=true;
		activitiesPreferredRoomsList[i].clear();
		//activitiesPreferredRoomsPercentage[i]=-1;

		unspecifiedHomeRoom[i]=true;
		activitiesHomeRoomsHomeRooms[i].clear();
		activitiesHomeRoomsPercentage[i]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
			ConstraintStudentsSetHomeRoom* spr=(ConstraintStudentsSetHomeRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			if(studentsSetHomeRoom.contains(spr->studentsName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "students set home room(s) for students set %1. Please leave only one of them")
				 .arg(spr->studentsName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			studentsSetHomeRoom.insert(spr->studentsName);
		
			foreach(int a, spr->_activities){
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
				
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					activitiesHomeRoomsHomeRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesHomeRoomsHomeRooms[a].indexOf(spr->_room);
					activitiesHomeRoomsHomeRooms[a].clear();
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesHomeRoomsHomeRooms[a].append(spr->_room);
					}
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
			ConstraintStudentsSetHomeRooms* spr=(ConstraintStudentsSetHomeRooms*)gt.rules.internalSpaceConstraintsList[i];
		
			if(studentsSetHomeRoom.contains(spr->studentsName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "students set home room(s) for students set %1. Please leave only one of them")
				 .arg(spr->studentsName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			studentsSetHomeRoom.insert(spr->studentsName);
		
			foreach(int a, spr->_activities){	
		//	for(int j=0; j<spr->_nActivities; j++){
		//		int a=spr->_activities[j];
				
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						activitiesHomeRoomsHomeRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						if(activitiesHomeRoomsHomeRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					activitiesHomeRoomsHomeRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_HOME_ROOM){
			ConstraintTeacherHomeRoom* spr=(ConstraintTeacherHomeRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			if(teachersHomeRoom.contains(spr->teacherName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "teacher home room(s) for teacher %1. Please leave only one of them")
				 .arg(spr->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			teachersHomeRoom.insert(spr->teacherName);
		
			foreach(int a, spr->_activities){
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
				
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					activitiesHomeRoomsHomeRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesHomeRoomsHomeRooms[a].indexOf(spr->_room);
					activitiesHomeRoomsHomeRooms[a].clear();
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesHomeRoomsHomeRooms[a].append(spr->_room);
					}
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_HOME_ROOMS){
			ConstraintTeacherHomeRooms* spr=(ConstraintTeacherHomeRooms*)gt.rules.internalSpaceConstraintsList[i];
		
			if(teachersHomeRoom.contains(spr->teacherName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "teacher home room(s) for teacher %1. Please leave only one of them")
				 .arg(spr->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			teachersHomeRoom.insert(spr->teacherName);
		
			foreach(int a, spr->_activities){	
		//	for(int j=0; j<spr->_nActivities; j++){
		//		int a=spr->_activities[j];
				
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						activitiesHomeRoomsHomeRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						if(activitiesHomeRoomsHomeRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					activitiesHomeRoomsHomeRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* spr=(ConstraintSubjectPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(subjectsPreferredRoom.contains(spr->subjectName)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject preferred room(s) for subject %1. Please leave only one of them")
				 .arg(spr->subjectName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsPreferredRoom.insert(spr->subjectName);*/
		
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
				
					if(unspecifiedPreferredRoom[a]){
						unspecifiedPreferredRoom[a]=false;
						//activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
						//assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
						//activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
					}
					else{
						//int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(spr->_room);
						//activitiesPreferredRoomsPreferredRooms[a].clear();
						//activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
						//if(t!=-1){
						//	activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
						//}
					}
				
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* spr=(ConstraintSubjectPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(subjectsPreferredRoom.contains(spr->subjectName)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject preferred room(s) for subject %1. Please leave only one of them")
				 .arg(spr->subjectName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsPreferredRoom.insert(spr->subjectName);*/
		
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					foreach(int k, spr->_rooms)
						it.preferredRooms.insert(k);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
				
					activitiesPreferredRoomsList[a].append(it);
				}

				/*if(unspecifiedPreferredRoom[a]){
					unspecifiedPreferredRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						activitiesPreferredRoomsPreferredRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					activitiesPreferredRoomsPreferredRooms[a]=shared;
				}*/
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* spr=(ConstraintSubjectActivityTagPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*QPair<QString, QString> pair=qMakePair(spr->subjectName, spr->activityTagName);
			if(subjectsActivityTagsPreferredRoom.contains(pair)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject activity tag preferred room(s) for subject %1, activity tag %2. Please leave only one of them")
				 .arg(spr->subjectName)
				 .arg(spr->activityTagName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsActivityTagsPreferredRoom.insert(pair);*/
		
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}

				/*if(unspecifiedPreferredRoom[a]){
					unspecifiedPreferredRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(spr->_room);
					activitiesPreferredRoomsPreferredRooms[a].clear();
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
					}
				}*/
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* spr=(ConstraintSubjectActivityTagPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*QPair<QString, QString> pair=qMakePair(spr->subjectName, spr->activityTagName);
			if(subjectsActivityTagsPreferredRoom.contains(pair)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject activity tag preferred room(s) for subject %1, activity tag %2. Please leave only one of them")
				 .arg(spr->subjectName)
				 .arg(spr->activityTagName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsActivityTagsPreferredRoom.insert(pair);*/
		
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					foreach(int k, spr->_rooms)
						it.preferredRooms.insert(k);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}
				
				/*if(unspecifiedPreferredRoom[a]){
					unspecifiedPreferredRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						activitiesPreferredRoomsPreferredRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					//for(int k=0; k<spr->_n_preferred_rooms; k++){
					//	int rm=spr->_rooms[k];
					foreach(int rm, spr->_rooms){
						if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					activitiesPreferredRoomsPreferredRooms[a]=shared;
				}*/
			}
		}

		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintActivityTagPreferredRoom* spr=(ConstraintActivityTagPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];

			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
			
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintActivityTagPreferredRooms* spr=(ConstraintActivityTagPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];

			foreach(int a, spr->_activities){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					foreach(int k, spr->_rooms)
						it.preferredRooms.insert(k);
			
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
				
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}

		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* apr=(ConstraintActivityPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(activitiesPreferredRoom.contains(apr->activityId)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "activity preferred room(s) for activity with id %1. Please leave only one of them")
				 .arg(apr->activityId),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			activitiesPreferredRoom.insert(apr->activityId);*/
		
			int a=apr->_activity;

			if(apr->weightPercentage==100.0){
				constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
			}
			else{
				PreferredRoomsItem it;
					
				it.percentage=apr->weightPercentage;
				it.preferredRooms.insert(apr->_room);
				
				if(unspecifiedPreferredRoom[a])
					unspecifiedPreferredRoom[a]=false;
			
				activitiesPreferredRoomsList[a].append(it);
			}
			/*if(unspecifiedPreferredRoom[a]){
				unspecifiedPreferredRoom[a]=false;
				activitiesPreferredRoomsPercentage[a]=apr->weightPercentage;
				assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
				activitiesPreferredRoomsPreferredRooms[a].append(apr->_room);
			}
			else{
				int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(apr->_room);
				activitiesPreferredRoomsPreferredRooms[a].clear();
				activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], apr->weightPercentage);
				if(t!=-1){
					activitiesPreferredRoomsPreferredRooms[a].append(apr->_room);
				}
			}*/
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* apr=(ConstraintActivityPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(activitiesPreferredRoom.contains(apr->activityId)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "activity preferred room(s) for activity with id %1. Please leave only one of them")
				 .arg(apr->activityId),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			activitiesPreferredRoom.insert(apr->activityId);*/
		
			int a=apr->_activity;

			if(apr->weightPercentage==100.0){
				constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
			}
			else{
				PreferredRoomsItem it;
					
				it.percentage=apr->weightPercentage;
				foreach(int k, apr->_rooms)
					it.preferredRooms.insert(k);
				
				if(unspecifiedPreferredRoom[a])
					unspecifiedPreferredRoom[a]=false;
					
				activitiesPreferredRoomsList[a].append(it);
			}
				
			/*if(unspecifiedPreferredRoom[a]){
				unspecifiedPreferredRoom[a]=false;
				activitiesPreferredRoomsPercentage[a]=apr->weightPercentage;
				assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
				foreach(int rm, apr->_rooms){
				//for(int k=0; k<apr->_n_preferred_rooms; k++){
				//	int rm=apr->_rooms[k];
					activitiesPreferredRoomsPreferredRooms[a].append(rm);
				}
			}
			else{
				QList<int> shared;
				foreach(int rm, apr->_rooms){
				//for(int k=0; k<apr->_n_preferred_rooms; k++){
				//	int rm=apr->_rooms[k];
					if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
						shared.append(rm);
				}
				activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], apr->weightPercentage);
				activitiesPreferredRoomsPreferredRooms[a]=shared;
			}*/
		}
	}
	
	for(int a=0; a<gt.rules.nInternalActivities; a++){
		QList<SpaceConstraint*> scl=constraintsForActivity[a];
		
		if(scl.count()==0){
			//nothing
		}
		else{
			if(unspecifiedPreferredRoom[a])
				unspecifiedPreferredRoom[a]=false;

			PreferredRoomsItem it;
			it.percentage=100.0;
			it.preferredRooms.clear();
	
			bool begin=true;
			foreach(SpaceConstraint* ctr, scl){
				if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
					ConstraintSubjectPreferredRoom* spr=(ConstraintSubjectPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
					ConstraintSubjectPreferredRooms* spr=(ConstraintSubjectPreferredRooms*)ctr;
					
					if(begin){
						it.preferredRooms=spr->_rooms.toSet();
						begin=false;
					}
					else{
						it.preferredRooms.intersect(spr->_rooms.toSet());
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
					ConstraintSubjectActivityTagPreferredRoom* spr=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
					ConstraintSubjectActivityTagPreferredRooms* spr=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
					
					if(begin){
						it.preferredRooms=spr->_rooms.toSet();
						begin=false;
					}
					else{
						it.preferredRooms.intersect(spr->_rooms.toSet());
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
					ConstraintActivityTagPreferredRoom* spr=(ConstraintActivityTagPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
					ConstraintActivityTagPreferredRooms* spr=(ConstraintActivityTagPreferredRooms*)ctr;
					
					if(begin){
						it.preferredRooms=spr->_rooms.toSet();
						begin=false;
					}
					else{
						it.preferredRooms.intersect(spr->_rooms.toSet());
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
					ConstraintActivityPreferredRoom* spr=(ConstraintActivityPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
					ConstraintActivityPreferredRooms* spr=(ConstraintActivityPreferredRooms*)ctr;
					
					if(begin){
						it.preferredRooms=spr->_rooms.toSet();
						begin=false;
					}
					else{
						it.preferredRooms.intersect(spr->_rooms.toSet());
					}
				}
			}
			
			activitiesPreferredRoomsList[a].append(it);
		}
	}
	
	/*for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(!unspecifiedPreferredRoom[i])
			if(activitiesPreferredRoomsPreferredRooms[i].count()==0){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed room (from constraints activity preferred room(s) and subject (activity tag) preferred room(s) )")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest of activities without rooms"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}*/

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(!unspecifiedHomeRoom[i])
			if(activitiesHomeRoomsHomeRooms[i].count()==0){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed home room (from constraints students set home room(s) and teacher home room(s))")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(!unspecifiedPreferredRoom[i]){
			for(int kk=0; kk<activitiesPreferredRoomsList[i].count(); kk++){
			//foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[i]){
				PreferredRoomsItem& it=activitiesPreferredRoomsList[i][kk];
		
				bool okinitial=true;
				if(it.preferredRooms.count()==0){
					okinitial=false;

					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
					 "you have no allowed preferred room (from preferred room(s) constraints).")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
	
					if(t==0)
						goto jumpOverPrefRoomsNStudents;

					//assert(0);
				}
				QSet<int> tmp=it.preferredRooms;
				foreach(int r, tmp){
					if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
						it.preferredRooms.remove(r);
					}
				}
				if(okinitial && it.preferredRooms.count()==0){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
					 "you have no allowed preferred room (from the allowed number of students and preferred room(s) constraints)")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
	
					if(t==0)
						goto jumpOverPrefRoomsNStudents;
				}		
			}
			/*bool okinitial=true;
			if(activitiesPreferredRoomsPreferredRooms[i].count()==0)
				okinitial=false;
			foreach(int r, activitiesPreferredRoomsPreferredRooms[i]){
				if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
					activitiesPreferredRoomsPreferredRooms[i].removeAll(r);
				}
			}
			if(okinitial && activitiesPreferredRoomsPreferredRooms[i].count()==0){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed room (from the allowed number of students)")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest of activities without rooms"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}*/
		}
	}
jumpOverPrefRoomsNStudents:
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(!unspecifiedHomeRoom[i]){
			bool okinitial=true;
			if(activitiesHomeRoomsHomeRooms[i].count()==0)
				okinitial=false;
			QList<int> tmp=activitiesHomeRoomsHomeRooms[i];
			foreach(int r, tmp){
				if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
					activitiesHomeRoomsHomeRooms[i].removeAll(r);
				}
			}
			if(okinitial && activitiesHomeRoomsHomeRooms[i].count()==0){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed home room (from the allowed number of students)")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
		}
	}
	
	
	//////////new test, 9 Sept. 2009
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			if(!unspecifiedPreferredRoom[i]){
				bool begin=true;
				QSet<int> allowedRooms;
				for(int kk=0; kk<activitiesPreferredRoomsList[i].count(); kk++){
					PreferredRoomsItem& it=activitiesPreferredRoomsList[i][kk];
					if(it.percentage==100.0){
						if(begin){
							allowedRooms=it.preferredRooms;
							begin=false;
						}
						else
							allowedRooms.intersect(it.preferredRooms);
					}
				}
				if(!begin && allowedRooms.count()==0){
					ok=false;
				
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate the timetable, because for activity with id==%1 "
					 "you have no allowed preferred room (considering rooms' capacities and constraints preferred"
					 " room(s) with 100.0% weight percentage)")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
		
					if(t==0)
						break;
				}
			}
		}
	}
	//////////
	
	return ok;
}

bool computeMaxBuildingChangesPerDayForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxBuildingChangesPerDayForStudentsPercentages[i]=-1;
		maxBuildingChangesPerDayForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxBuildingChangesPerDay* spr=(ConstraintStudentsSetMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			foreach(int sbg, spr->iSubgroupsList){
				maxBuildingChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerDayForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsMaxBuildingChangesPerDay* spr=(ConstraintStudentsMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxBuildingChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerDayForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerWeekForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxBuildingChangesPerWeekForStudentsPercentages[i]=-1;
		maxBuildingChangesPerWeekForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxBuildingChangesPerWeek* spr=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			foreach(int sbg, spr->iSubgroupsList){
				maxBuildingChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerWeek);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsMaxBuildingChangesPerWeek* spr=(ConstraintStudentsMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxBuildingChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerWeek);
			}
		}
	}
	
	return ok;
}

bool computeMinGapsBetweenBuildingChangesForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		minGapsBetweenBuildingChangesForStudentsPercentages[i]=-1;
		minGapsBetweenBuildingChangesForStudentsMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsSetMinGapsBetweenBuildingChanges* spr=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			foreach(int sbg, spr->iSubgroupsList){
				minGapsBetweenBuildingChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=max(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg], spr->minGapsBetweenBuildingChanges);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsMinGapsBetweenBuildingChanges* spr=(ConstraintStudentsMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				minGapsBetweenBuildingChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=max(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg], spr->minGapsBetweenBuildingChanges);
			}
		}
	}
	
	return ok;
}


bool computeMaxBuildingChangesPerDayForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxBuildingChangesPerDayForTeachersPercentages[i]=-1;
		maxBuildingChangesPerDayForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeacherMaxBuildingChangesPerDay* spr=(ConstraintTeacherMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxBuildingChangesPerDayForTeachersPercentages[spr->teacher_ID]=100;
			if(maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]<0)
				maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=spr->maxBuildingChangesPerDay;
			else
				maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=min(maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID], spr->maxBuildingChangesPerDay);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeachersMaxBuildingChangesPerDay* spr=(ConstraintTeachersMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				maxBuildingChangesPerDayForTeachersPercentages[tch]=100;
				if(maxBuildingChangesPerDayForTeachersMaxChanges[tch]<0)
					maxBuildingChangesPerDayForTeachersMaxChanges[tch]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForTeachersMaxChanges[tch]=min(maxBuildingChangesPerDayForTeachersMaxChanges[tch], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerWeekForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxBuildingChangesPerWeekForTeachersPercentages[i]=-1;
		maxBuildingChangesPerWeekForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeacherMaxBuildingChangesPerWeek* spr=(ConstraintTeacherMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxBuildingChangesPerWeekForTeachersPercentages[spr->teacher_ID]=100;
			if(maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]<0)
				maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=spr->maxBuildingChangesPerWeek;
			else
				maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=min(maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID], spr->maxBuildingChangesPerWeek);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeachersMaxBuildingChangesPerWeek* spr=(ConstraintTeachersMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){			
				maxBuildingChangesPerWeekForTeachersPercentages[tch]=100;
				if(maxBuildingChangesPerWeekForTeachersMaxChanges[tch]<0)
					maxBuildingChangesPerWeekForTeachersMaxChanges[tch]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForTeachersMaxChanges[tch]=min(maxBuildingChangesPerWeekForTeachersMaxChanges[tch], spr->maxBuildingChangesPerWeek);
			}
		}
	}

	return ok;
}

bool computeMinGapsBetweenBuildingChangesForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		minGapsBetweenBuildingChangesForTeachersPercentages[i]=-1;
		minGapsBetweenBuildingChangesForTeachersMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeacherMinGapsBetweenBuildingChanges* spr=(ConstraintTeacherMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			minGapsBetweenBuildingChangesForTeachersPercentages[spr->teacher_ID]=100;
			if(minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]<0)
				minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]=spr->minGapsBetweenBuildingChanges;
			else
				minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]=max(minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID], spr->minGapsBetweenBuildingChanges);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeachersMinGapsBetweenBuildingChanges* spr=(ConstraintTeachersMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				minGapsBetweenBuildingChangesForTeachersPercentages[tch]=100;
				if(minGapsBetweenBuildingChangesForTeachersMinGaps[tch]<0)
					minGapsBetweenBuildingChangesForTeachersMinGaps[tch]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForTeachersMinGaps[tch]=max(minGapsBetweenBuildingChangesForTeachersMinGaps[tch], spr->minGapsBetweenBuildingChanges);
			}
		}
	}
	
	return ok;
}

void computeMustComputeTimetableSubgroups()
{
	for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++)
		mustComputeTimetableSubgroup[sbg]=false;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
	
		mustComputeTimetableSubgroups[ai].clear();
		
		foreach(int sbg, act->iSubgroupsList)
			if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0 ||
			  subgroupsMaxGapsPerDayPercentage[sbg]>=0 ||
			  subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0 ||
			  subgroupsMaxHoursDailyPercentages1[sbg]>=0 ||
			  subgroupsMaxHoursDailyPercentages2[sbg]>=0 ||
			  subgroupsMaxHoursContinuouslyPercentages1[sbg]>=0 ||
			  subgroupsMaxHoursContinuouslyPercentages2[sbg]>=0 ||
			  subgroupsMinHoursDailyPercentages[sbg]>=0 ||
			  
			  subgroupsIntervalMaxDaysPerWeekPercentages1[sbg]>=0 ||
			  subgroupsIntervalMaxDaysPerWeekPercentages2[sbg]>=0 ||
			  subgroupsIntervalMaxDaysPerWeekPercentages3[sbg]>=0 ||
			  
			  maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0 ||
			  maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0 ||
			  minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 ||
			  
			  subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].count()>0 ||
			  subgroupsActivityTagMaxHoursDailyPercentage[sbg].count()>0
			  ){
			  
				mustComputeTimetableSubgroups[ai].append(sbg);
				mustComputeTimetableSubgroup[sbg]=true;
			}
	}
}

void computeMustComputeTimetableTeachers()
{
	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++)
		mustComputeTimetableTeacher[tch]=false;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
	
		mustComputeTimetableTeachers[ai].clear();
		
		foreach(int tch, act->iTeachersList)
			if(teachersMaxGapsPerWeekPercentage[tch]>=0 ||
			  teachersMaxGapsPerDayPercentage[tch]>=0 ||
			  teachersMaxHoursDailyPercentages1[tch]>=0 ||
			  teachersMaxHoursDailyPercentages2[tch]>=0 ||
			  teachersMaxHoursContinuouslyPercentages1[tch]>=0 ||
			  teachersMaxHoursContinuouslyPercentages2[tch]>=0 ||
			  teachersMinHoursDailyPercentages[tch]>=0 ||
			  
			  teachersMinDaysPerWeekPercentages[tch]>=0 ||
			  
			  teachersIntervalMaxDaysPerWeekPercentages1[tch]>=0 ||
			  teachersIntervalMaxDaysPerWeekPercentages2[tch]>=0 ||
			  teachersIntervalMaxDaysPerWeekPercentages3[tch]>=0 ||
			  
			  maxBuildingChangesPerDayForTeachersPercentages[tch]>=0 ||
			  maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0 ||
			  minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 ||
			  
			  teachersActivityTagMaxHoursContinuouslyPercentage[tch].count()>0 ||
			  teachersActivityTagMaxHoursDailyPercentage[tch].count()>0
			  ){
			  
				mustComputeTimetableTeachers[ai].append(tch);
				mustComputeTimetableTeacher[tch]=true;
			}
	}
}


bool computeFixedActivities(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		int notAllowedSlots=0;
		for(int tim=0; tim<gt.rules.nHoursPerWeek; tim++)
			if(notAllowedTimesPercentages[ai][tim]==100)
				notAllowedSlots++;
		
		if(notAllowedSlots==gt.rules.nHoursPerWeek){
			ok=false;
		
			QString s=GeneratePreTranslate::tr("Activity with id=%1 has no allowed slot - please correct that").arg(gt.rules.internalActivitiesList[ai].id);
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
		else if(notAllowedSlots==gt.rules.nHoursPerWeek-1)
			fixedTimeActivity[ai]=true;
		else
			fixedTimeActivity[ai]=false;
			
		//space
		fixedSpaceActivity[ai]=false;
		foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ai])
			if(it.percentage==100.0 && it.preferredRooms.count()==1){
				fixedSpaceActivity[ai]=true;
				break;
			}
	}
	
	return ok;
}


/*
This should be better, but in practice it is not :-)
void sortActivities()
{
	//const double THRESHOLD=80.0;
	
	double nIncompatible[MAX_ACTIVITIES];
	
		
	//rooms init
	double nRoomsIncompat[MAX_ROOMS];
	for(int j=0; j<gt.rules.nInternalRooms; j++){
		nRoomsIncompat[j]=0;
		for(int k=0; k<gt.rules.nHoursPerWeek; k++)
			if(allowedRoomTimePercentages[j][k]>=0)
				nRoomsIncompat[j]+=allowedRoomTimePercentages[j][k]/100.0;
	}
	double nHoursForRoom[MAX_ROOMS];

	for(int j=0; j<gt.rules.nInternalRooms; j++)
		nHoursForRoom[j]=0;

	for(int j=0; j<gt.rules.nInternalActivities; j++)
		if(activitiesPreferredRoomsPercentage[j]>=0){
			assert(!unspecifiedPreferredRoom[j]);
			foreach(int rm, activitiesPreferredRoomsPreferredRooms[j])
				nHoursForRoom[rm]+=gt.rules.internalActivitiesList[j].duration/activitiesPreferredRoomsPreferredRooms[j].count()
				  *activitiesPreferredRoomsPercentage[j]/100.0;
		}

	

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nIncompatible[i]=0;
		
		//basic
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			if(i!=j && activitiesConflictingPercentage[i][j]>=0){
				assert(activitiesConflictingPercentage[i][j]==100.0);
				nIncompatible[i]+=gt.rules.internalActivitiesList[j].duration; // *100/100.0
			}
				
		//not available, break, preferred time(s)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			if(notAllowedTimesPercentages[i][j]>=0)
				nIncompatible[i]+=notAllowedTimesPercentages[i][j]/100.0;
		
		//min days - no
		

		//teachers max days per week
		//foreach(int t, teachersWithMaxDaysPerWeekForActivities[i]){
		foreach(int t, gt.rules.internalActivitiesList[i].iTeachersList){
			if(teachersMaxDaysPerWeekWeightPercentages[t]>=0){
				assert(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t] >=0 );
				nIncompatible[i]+=(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t])*gt.rules.nHoursPerDay
				  *teachersMaxDaysPerWeekWeightPercentages[t]/100.0;
			}
		}

		
		//rooms
		if(activitiesPreferredRoomsPercentage[i]>=0){
			double cnt=0;
			assert(!unspecifiedPreferredRoom[i]);
			foreach(int rm, activitiesPreferredRoomsPreferredRooms[i])
				cnt+=(nRoomsIncompat[rm]+nHoursForRoom[rm]-gt.rules.internalActivitiesList[i].duration)
				  *activitiesPreferredRoomsPercentage[i]/100.0;
				  //- because we considered also current activity
				
			nIncompatible[i] += cnt / activitiesPreferredRoomsPreferredRooms[i].count(); //average for all the rooms
		}
				
		

		nIncompatible[i]*=gt.rules.internalActivitiesList[i].duration;
	}

	//same starting time - not computing, the algo takes care even without correct sorting
	//it is difficult to sort about same starting time
	
	//Sort activities in in-creasing order of number of the other activities with which
	//this activity does not conflict
	//Selection sort, based on a permutation
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			if(nIncompatible[permutation[i]]<nIncompatible[permutation[j]]){
				int t=permutation[i];
				permutation[i]=permutation[j];
				permutation[j]=t;
			}
		}
	}
	
	cout<<"The order of activities (id-s):"<<endl;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		cout<<"No: "<<i+1<<", nIncompatible[permutation[i]]=="<<nIncompatible[permutation[i]]<<", ";
	
		Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
		cout<<"id=="<<act->id;
		cout<<", teachers: ";
		foreach(QString s, act->teachersNames)
			cout<<qPrintable(s)<<" ";
		cout<<", subj=="<<qPrintable(act->subjectName);
		cout<<", students: ";
		foreach(QString s, act->studentsNames)
			cout<<qPrintable(s)<<" ";
		cout<<endl;
	}
	cout<<"End - the order of activities (id-s):"<<endl;
}
*/

bool homeRoomsAreOk(QWidget* parent)
{
	//int nHoursRequiredForRoom[MAX_ROOMS];

	for(int r=0; r<gt.rules.nInternalRooms; r++)
		nHoursRequiredForRoom[r]=0;
		
	for(int a=0; a<gt.rules.nInternalActivities; a++)
		if(unspecifiedPreferredRoom[a] && !unspecifiedHomeRoom[a] 
		  && activitiesHomeRoomsHomeRooms[a].count()==1 && activitiesHomeRoomsPercentage[a]==100.0){
		  	int r=activitiesHomeRoomsHomeRooms[a].at(0);
		  	nHoursRequiredForRoom[r]+=gt.rules.internalActivitiesList[a].duration;
		}
		
	//int nHoursAvailableForRoom[MAX_ROOMS];

	for(int r=0; r<gt.rules.nInternalRooms; r++){
		nHoursAvailableForRoom[r]=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && notAllowedRoomTimePercentages[r][d+h*gt.rules.nDaysPerWeek]<100.0)
					nHoursAvailableForRoom[r]++;
	}
	
	bool ok=true;

	for(int r=0; r<gt.rules.nInternalRooms; r++)
		if(nHoursRequiredForRoom[r]>nHoursAvailableForRoom[r]){
			ok=false;

			QString s=GeneratePreTranslate::tr("Room %1 has not enough slots for home rooms constraints (requested %2, available %3) - please correct that")
			  .arg(gt.rules.internalRoomsList[r]->name).arg(nHoursRequiredForRoom[r]).arg(nHoursAvailableForRoom[r]);
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
		
	return ok;
}

void sortActivities(QWidget* parent, const QHash<int, int> & reprSameStartingTime, const QHash<int, QSet<int> > & reprSameActivitiesSet, QTextStream* initialOrderStream)
{
	//I should take care of home rooms, but I don't want to change the routine below which works well

	//	const int INF  = 2000000000;
	const int INF  = 1500000000; //INF and INF2 values of variables may be increased, so they should be INF2>>INF and INF2<<MAXINT(2147.........)
	//	const int INF2 = 2000000001;
	const int INF2 = 2000000000;

	const double THRESHOLD=80.0;
	
	//int nIncompatible[MAX_ACTIVITIES];
	
	
	//rooms init
	//int nRoomsIncompat[MAX_ROOMS];
	for(int j=0; j<gt.rules.nInternalRooms; j++){
		nRoomsIncompat[j]=0;
		for(int k=0; k<gt.rules.nHoursPerWeek; k++)
			if(notAllowedRoomTimePercentages[j][k]>=THRESHOLD)
				nRoomsIncompat[j]++;
	}
	//double nHoursForRoom[MAX_ROOMS];

	for(int j=0; j<gt.rules.nInternalRooms; j++)
		nHoursForRoom[j]=0.0;

	//only consider for each activity the constraint preferred room(s) with highest percentage (and then lowest number of rooms)
	//PreferredRoomsItem maxPercentagePrefRooms[MAX_ACTIVITIES];
	maxPercentagePrefRooms.resize(gt.rules.nInternalActivities);
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxPercentagePrefRooms[j].percentage=-1;
		maxPercentagePrefRooms[j].preferredRooms.clear();
	
		double maxPercentage=-1;
		double minNRooms=INF;
		foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[j])
			if(maxPercentage<it.percentage || (maxPercentage==it.percentage && minNRooms>it.preferredRooms.count())){
				maxPercentage=it.percentage;
				minNRooms=it.preferredRooms.count();
				maxPercentagePrefRooms[j]=it;
			}
	}

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		PreferredRoomsItem it=maxPercentagePrefRooms[j];
		if(it.percentage>=THRESHOLD){
			assert(!unspecifiedPreferredRoom[j]);
			foreach(int rm, it.preferredRooms)
				nHoursForRoom[rm]+=double(gt.rules.internalActivitiesList[j].duration)/double(it.preferredRooms.count());
		}
	}
	
	
	//make those with same starting time have conflicts
	//for instance, if A1 simultaneous with A2 and A2 conflicts with A3. then A1 conflicts with A3
	//also, A1 will conflict with A2, but this conflict is not counted
	//idea of Volker Dirr, implementation of Liviu
	
	QSet<int> allRepresentants;
	foreach(int r, reprSameStartingTime) //only values, which are representants
		allRepresentants.insert(r);
	
	foreach(int r, allRepresentants){
		assert(reprSameActivitiesSet.contains(r));
		QSet<int> s=reprSameActivitiesSet.value(r);
		
		assert(s.count()>=1);
		if(s.count()>=2){
			//Faster
			QHash<int, int> conflHash;
			
			foreach(int i, s){
				QHashIterator<int, int> it(activitiesConflictingPercentage[i]);
				while(it.hasNext()){
					it.next();
					int j=it.key();
					int weight=it.value();
					if(conflHash.value(j, -1)<weight)
						conflHash.insert(j, weight);
				}
			}
			
			foreach(int i, s){
				QHashIterator<int, int> it(conflHash);
				while(it.hasNext()){
					it.next();
					int j=it.key();
					int weight=it.value();
					if(activitiesConflictingPercentage[i].value(j, -1)<weight)
						activitiesConflictingPercentage[i].insert(j, weight);

					if(activitiesConflictingPercentage[j].value(i, -1)<weight)
						activitiesConflictingPercentage[j].insert(i, weight);
				}
			}

			/*for(int i=0; i<gt.rules.nInternalActivities; i++){
				crth[i]=-1; //horizontal
				crtv[i]=-1; //vertical
			}
		
			foreach(int j, s){
				for(int i=0; i<gt.rules.nInternalActivities; i++){
					if(crth[i]<activitiesConflictingPercentage[j].value(i, -1))
						crth[i]=activitiesConflictingPercentage[j].value(i, -1);
					if(crtv[i]<activitiesConflictingPercentage[i].value(j, -1))
						crtv[i]=activitiesConflictingPercentage[i].value(j, -1);
				}
			}

			for(int i=0; i<gt.rules.nInternalActivities; i++)
				assert(crth[i]==crtv[i]);
		
			foreach(int j, s){
				for(int i=0; i<gt.rules.nInternalActivities; i++){
					assert(activitiesConflictingPercentage[j].value(i, -1)<=crth[i]);
					activitiesConflictingPercentage[j].insert(i, crth[i]);

					assert(activitiesConflictingPercentage[i].value(j, -1)<=crtv[i]);
					activitiesConflictingPercentage[i].insert(j, crtv[i]);
				}
			}*/
		}
	}
	//end same starting time

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nIncompatible[i]=0;
		
		assert(reprSameStartingTime.contains(i));
		
		//basic
		QHash<int, int> &hashConfl=activitiesConflictingPercentage[i];

		QHashIterator<int, int> iter(hashConfl);
		while(iter.hasNext()){
			iter.next();
			//cout<<it.key()<<": "<<it.value()<<endl;
			int j=iter.key();
			assert(reprSameStartingTime.contains(j));

			if(reprSameStartingTime.value(i)!=reprSameStartingTime.value(j)){
				//if(i!=j && activitiesConflictingPercentage[i].value(j, -1)>=THRESHOLD){
				if(i!=j && iter.value()>=THRESHOLD){
					nIncompatible[i]+=gt.rules.internalActivitiesList[j].duration;
				}
			}
		}

		/*for(int j=0; j<gt.rules.nInternalActivities; j++){
			assert(reprSameStartingTime.contains(j));
			
			if(reprSameStartingTime.value(i)!=reprSameStartingTime.value(j)){
				if(i!=j && activitiesConflictingPercentage[i].value(j, -1)>=THRESHOLD){
					//assert(activitiesConflictingPercentage[i][j]==100);
					nIncompatible[i]+=gt.rules.internalActivitiesList[j].duration;
				}
			}
		}*/
		
		//not available, break, preferred time(s)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			if(notAllowedTimesPercentages[i][j]>=THRESHOLD)
				nIncompatible[i]++;
		
		//min days - no
		

		//students max days per week
		foreach(int s, gt.rules.internalActivitiesList[i].iSubgroupsList){
			if(subgroupsMaxDaysPerWeekWeightPercentages[s]>=THRESHOLD){
				assert(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s] >=0 );
				nIncompatible[i]+=(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s])*gt.rules.nHoursPerDay;
			}
		}

		//teachers max days per week
		//foreach(int t, teachersWithMaxDaysPerWeekForActivities[i]){
		foreach(int t, gt.rules.internalActivitiesList[i].iTeachersList){
			if(teachersMaxDaysPerWeekWeightPercentages[t]>=THRESHOLD){
				assert(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t] >=0 );
				nIncompatible[i]+=(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t])*gt.rules.nHoursPerDay;
			}
		}

		
		//rooms
		PreferredRoomsItem it=maxPercentagePrefRooms[i];
		if(it.percentage>=THRESHOLD){
			double cnt=0.0;
			assert(!unspecifiedPreferredRoom[i]);
			foreach(int rm, it.preferredRooms)
				cnt+=double(nRoomsIncompat[rm])+nHoursForRoom[rm]-(double(gt.rules.internalActivitiesList[i].duration)/double(it.preferredRooms.count()));
				 //-duration because we considered also current activity
			
			if(cnt<0.0){
				//assert(cnt+0.01>=0.0); //maybe rouding error, but not too high
				if(!(cnt+0.01>=0.0)){
					cout<<"Probable ERROR in file "<<__FILE__<<", line "<<__LINE__<<", cnt should be >=0.0"<<endl;
				}
				cnt=0.0;
			}
			
			assert(cnt>=0.0);
			
			//2013-01-08 - So that generation is identical on all computers
			double t = cnt / double(it.preferredRooms.count()); //average for all the rooms
			
			t*=100000.0;
			t=floor(t+0.5);
			t/=100000.0;

			nIncompatible[i] += int(floor(t)); //average for all the rooms
		}
				
		
		nIncompatible[i]*=gt.rules.internalActivitiesList[i].duration;
		
		assert(nIncompatible[i]<INF);
		
		assert(nIncompatible[i]>=0);
		
		if(fixedTimeActivity[i]){
			nIncompatible[i]=INF;
			
			if(fixedSpaceActivity[i])
				nIncompatible[i]=INF2;
				
			//this is to avoid an "impossible to generate" bug in fixed timetables - does not eliminate completely the bug, unfortunately
			nIncompatible[i]+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
			 gt.rules.internalActivitiesList[i].iTeachersList.count();
			 
			assert(nIncompatible[i]>=INF);
		}
	}

	//DEPRECATED COMMENT
	//same starting time - not computing, the algo takes care even without correct sorting
	//it is difficult to sort about same starting time
	
	//IT IS POSSIBLE TO SORT ABOUT SAME STARTING TIME, see below, idea of Volker Dirr. It is much faster for some data sets.
	//(for some data sets, it is 7 times faster)
	
	//compute repr:
	/////////////////
	QMultiHash<int, int> adjMatrix;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
			for(int i=1; i<sst->_n_activities; i++){
				adjMatrix.insert(sst->_activities[0], sst->_activities[i]);
				adjMatrix.insert(sst->_activities[i], sst->_activities[0]);
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE
		 && gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintTwoActivitiesConsecutive* c2c=(ConstraintTwoActivitiesConsecutive*)gt.rules.internalTimeConstraintsList[i];
			
			adjMatrix.insert(c2c->firstActivityIndex, c2c->secondActivityIndex);
			adjMatrix.insert(c2c->secondActivityIndex, c2c->firstActivityIndex);
		}
	}
		
	QHash<int, int> repr;
	//repr.clear();
	
	QQueue<int> queue;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int start=i;
		
		if(repr.value(start, -1)==-1){ //not visited
			repr.insert(start, start);
			queue.enqueue(start);
			while(!queue.isEmpty()){
				int crtHead=queue.dequeue();
				assert(repr.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix.values(crtHead);
				foreach(int neigh, neighList){
					if(repr.value(neigh, -1)==-1){
						queue.enqueue(neigh);
						repr.insert(neigh, start);
					}
					else{
						assert(repr.value(neigh, -1)==start);
					}
				}
			}
		}
	}

	/////////////////
	
	//take care of chains of constraints with 100% weight
	//int reprNInc[MAX_ACTIVITIES];
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		reprNInc[i]=-1;
		
	//repr contains the representant of each activity
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		//int MM=nIncompatible[i];
		assert(repr.contains(i));
		int r=repr.value(i);
		
		int xx=nIncompatible[i];
		if(fixedTimeActivity[i] && fixedSpaceActivity[i]){
			assert(xx>=INF2);
			xx-=(INF2-INF);
		}
		
		assert(xx<INF2);
		assert(xx>=0);
		
		if(reprNInc[r]<xx)
			reprNInc[r]=xx;
	}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int r=repr.value(i);
		assert(reprNInc[r]>=0);
		if(nIncompatible[i]<reprNInc[r])
			nIncompatible[i]=reprNInc[r];
	}
	
	//new volker (start), modified by Liviu
	//takes care of 100% weight constraints (not necessary - already took care above) and also <100% and >=80% weight constraints
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
		if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME && tc->weightPercentage>=THRESHOLD){
			ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*) tc;
			
			int xx=nIncompatible[c->_activities[0]];
			if(fixedTimeActivity[c->_activities[0]] && fixedSpaceActivity[c->_activities[0]]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);

			int MM=xx;
			
			for(int a=1; a<c->_n_activities; a++){
				int yy=nIncompatible[c->_activities[a]];
				if(fixedTimeActivity[c->_activities[a]] && fixedSpaceActivity[c->_activities[a]]){
					assert(yy>=INF2);
					yy-=(INF2-INF);
				}
				assert(yy>=0);
				assert(yy<INF2);

				if(MM<yy)
					MM=yy;
			}
			
			for(int a=0; a<c->_n_activities; a++)
				if(nIncompatible[c->_activities[a]]<MM)
					nIncompatible[c->_activities[a]]=MM;
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE && tc->weightPercentage>=THRESHOLD){
			ConstraintTwoActivitiesConsecutive* c=(ConstraintTwoActivitiesConsecutive*) tc;
			
			int xx=nIncompatible[c->firstActivityIndex];
			if(fixedTimeActivity[c->firstActivityIndex] && fixedSpaceActivity[c->firstActivityIndex]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);
			
			int MM=xx;
			
			int yy=nIncompatible[c->secondActivityIndex];
			if(fixedTimeActivity[c->secondActivityIndex] && fixedSpaceActivity[c->secondActivityIndex]){
				assert(yy>=INF2);
				yy-=(INF2-INF);
			}
			assert(yy>=0);
			assert(yy<INF2);
			
			if(MM<yy)
				MM=yy;
				
			if(nIncompatible[c->firstActivityIndex] < MM)
				nIncompatible[c->firstActivityIndex] = MM;

			if(nIncompatible[c->secondActivityIndex] < MM)
				nIncompatible[c->secondActivityIndex] = MM;
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED && tc->weightPercentage>=THRESHOLD){
			ConstraintTwoActivitiesGrouped* c=(ConstraintTwoActivitiesGrouped*) tc;
			
			int xx=nIncompatible[c->firstActivityIndex];
			if(fixedTimeActivity[c->firstActivityIndex] && fixedSpaceActivity[c->firstActivityIndex]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);
			
			int MM=xx;
			
			int yy=nIncompatible[c->secondActivityIndex];
			if(fixedTimeActivity[c->secondActivityIndex] && fixedSpaceActivity[c->secondActivityIndex]){
				assert(yy>=INF2);
				yy-=(INF2-INF);
			}
			assert(yy>=0);
			assert(yy<INF2);
			
			if(MM<yy)
				MM=yy;
				
			if(nIncompatible[c->firstActivityIndex] < MM)
				nIncompatible[c->firstActivityIndex] = MM;

			if(nIncompatible[c->secondActivityIndex] < MM)
				nIncompatible[c->secondActivityIndex] = MM;
		}
	}
	//new volker (end)
	
	//2014-06-30 - group activities in initial order
	if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
		for(int i=0; i<gt.rules.nInternalActivities; i++)
			fatherActivityInInitialOrder[i]=-1;
		
		bool report=true;
		int j=0;
		
		foreach(GroupActivitiesInInitialOrderItem* item, gt.rules.groupActivitiesInInitialOrderList){
			j++;
			
			if(!item->active)
				continue;
			
			if(item->indices.count()<2){
				if(report){
					QString s=GeneratePreTranslate::tr("Group activities in initial order item number %1 is ignored, because it contains less than"
					 " two active activities").arg(j);
				
					int t=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				
					if(t==0)
						report=false;
				}
			}
			else{
				int index0=item->indices.at(0);
				int xx=nIncompatible[index0];
				/*if(fixedTimeActivity[index0] && fixedSpaceActivity[index0]){
					assert(xx>=INF2);
					xx-=(INF2-INF);
				}
				assert(xx>=0);
				assert(xx<INF2);*/
				
				int MM=xx;
				int father=0;
				
				for(int i=1; i<item->indices.count(); i++){
					int indexi=item->indices.at(i);

					int yy=nIncompatible[indexi];
					/*if(fixedTimeActivity[indexi] && fixedSpaceActivity[indexi]){
						assert(yy>=INF2);
						yy-=(INF2-INF);
					}
					assert(yy>=0);
					assert(yy<INF2);*/
					
					if(MM<yy){
						MM=yy;
						father=i;
					}
				}
				
				//if(MM>=INF)
				//	MM=INF-1;

				for(int i=0; i<item->indices.count(); i++)
					if(i!=father){
						assert(fatherActivityInInitialOrder[item->indices.at(i)]==-1);
						fatherActivityInInitialOrder[item->indices.at(i)]=item->indices.at(father);
					}
					
				/*for(int i=0; i<item.indices.count(); i++){
					int ai=item.indices.at(i);
					if(nIncompatible[ai]<MM)
						nIncompatible[ai]=MM;
				}*/
			}
		}
	}
	
	//this is to avoid an "impossible to generate" bug in fixed timetables - does not eliminate completely the bug, unfortunately
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(nIncompatible[i]>=INF && nIncompatible[i]<INF2){
			if(fixedTimeActivity[i]){
				nIncompatible[i]=INF;
		
				nIncompatible[i]+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
				 gt.rules.internalActivitiesList[i].iTeachersList.count();
			 
				assert(nIncompatible[i]>=INF);
			}
			else{
				nIncompatible[i]=INF;
			}
		}
		else if(nIncompatible[i]>=INF2){
			assert(fixedTimeActivity[i] && fixedSpaceActivity[i]);
		
			int xx=nIncompatible[i]-INF2;
			assert(xx==gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
			 gt.rules.internalActivitiesList[i].iTeachersList.count());
		}
	}
	
	
	QHash<int, int> allowedSlotForFixedActivity;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(fixedTimeActivity[i]){
			int cnt=0;
			int allowed=-1;
			for(int s=0; s<gt.rules.nHoursPerWeek; s++){
				if(notAllowedTimesPercentages[i][s]<100.0){
					allowed=s;
					cnt++;
				}
			}
			assert(cnt==1);
			allowedSlotForFixedActivity.insert(i, allowed);
		}
	}
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nMinDaysConstraintsBroken[i]=1.0;
		
		if(fixedTimeActivity[i]){
			assert(allowedSlotForFixedActivity.contains(i));
			int si=allowedSlotForFixedActivity.value(i);
			int di=si%gt.rules.nDaysPerWeek;
		
			for(int d=0; d<minDaysListOfActivities[i].count(); d++){
				int j=minDaysListOfActivities[i].at(d);
				if(!fixedTimeActivity[j])
					continue;
				int m=minDaysListOfMinDays[i].at(d);
				double w=minDaysListOfWeightPercentages[i].at(d)/100.0;
				
				assert(allowedSlotForFixedActivity.contains(j));
				int sj=allowedSlotForFixedActivity.value(j);
				int dj=sj%gt.rules.nDaysPerWeek;
				
				int dist=abs(di-dj);
				if(dist<m){
					//careful, don't assert weight is <100.0, because the data may be impossible and we get assert failed
					nMinDaysConstraintsBroken[i]*=(1.0-w);
				}
			}
		}
		
		nMinDaysConstraintsBroken[i]=1.0-nMinDaysConstraintsBroken[i];
		//the resultant weight of all broken constraints
		//(the probability that after one try, we are not OK)
	}
	
	//DEPRECATED
	//Sort activities in in-creasing order of number of the other activities with which
	//this activity does not conflict
	//Selection sort, based on a permutation
	
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
		
	/*for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			if(nIncompatible[permutation[i]]<nIncompatible[permutation[j]] ||
			 (nIncompatible[permutation[i]]==nIncompatible[permutation[j]] && nMinDaysConstraintsBroken[permutation[i]]<nMinDaysConstraintsBroken[permutation[j]])){
				int t=permutation[i];
				permutation[i]=permutation[j];
				permutation[j]=t;
			}
		}
	}*/
	
	//descending by nIncompatible, then by nMinDaysConstraintsBroken
	//(by nMinDaysConstraintsBroken to alleviate an 'impossible to generate' bug if generating for a fixed timetable).
	if(gt.rules.groupActivitiesInInitialOrderList.count()==0){
		//qStableSort(permutation+0, permutation+gt.rules.nInternalActivities, compareFunctionGeneratePre);
		std::stable_sort(permutation+0, permutation+gt.rules.nInternalActivities, compareFunctionGeneratePre);

		for(int q=1; q<gt.rules.nInternalActivities; q++){
			int i=permutation[q-1];
			int j=permutation[q];
			//DEPRECATED COMMENT: don't assert nMinDaysConstraintsBroken is descending (if nIncompatible is equal), because it is a double number and we may get rounding errors
			//assert(nIncompatible[permutation[i-1]]>=nIncompatible[permutation[i]]);
			assert(nIncompatible[i]>nIncompatible[j] || (nIncompatible[i]==nIncompatible[j] && nMinDaysConstraintsBroken[i]>=nMinDaysConstraintsBroken[j]));
		}
	}
	else{
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			int nInc_i;
			
			if(fatherActivityInInitialOrder[i]==-1)
				nInc_i=nIncompatible[i];
			else
				nInc_i=nIncompatible[fatherActivityInInitialOrder[i]];
			
			if(nInc_i>=INF2){
				if(fixedTimeActivity[i] && fixedSpaceActivity[i]){
					nInc_i=INF2;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF2);
				}
				else if(fixedTimeActivity[i]){
					nInc_i=INF;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF);
				}
				else{
					nInc_i=INF;
				}
			}
			else if(nInc_i>=INF && nInc_i<INF2){
				if(fixedTimeActivity[i]){
					nInc_i=INF;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF);
				}
				else{
					nInc_i=INF;
				}
			}
			
			nIncompatibleFromFather[i]=nInc_i;
		}

		//qStableSort(permutation+0, permutation+gt.rules.nInternalActivities, compareFunctionGeneratePreWithGroupedActivities);
		std::stable_sort(permutation+0, permutation+gt.rules.nInternalActivities, compareFunctionGeneratePreWithGroupedActivities);
		
		for(int q=1; q<gt.rules.nInternalActivities; q++){
			int i=permutation[q-1];
			int j=permutation[q];
			assert( nIncompatibleFromFather[i]>nIncompatibleFromFather[j]
			 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j])
			 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]==nMinDaysConstraintsBroken[j] && nIncompatible[i]>=nIncompatible[j] ) );
		}
	}
	
	if(VERBOSE){
		cout<<"The order of activities (id-s):"<<endl;
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			cout<<"No: "<<i+1;
		
			Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			cout<<", id="<<act->id;
			cout<<", teachers: ";
			QString tj=act->teachersNames.join(" ");
			//foreach(QString s, act->teachersNames)
			//	cout<<qPrintable(s)<<" ";
			cout<<qPrintable(tj);
			cout<<", subject: "<<qPrintable(act->subjectName);
			if(act->activityTagsNames.count()>0){
				QString atj=act->activityTagsNames.join(" ");
				cout<<", activity tags: "<<qPrintable(atj);
			}
			cout<<", students: ";
			QString sj=act->studentsNames.join(" ");
			//foreach(QString s, act->studentsNames)
			//	cout<<qPrintable(s)<<" ";
			cout<<qPrintable(sj);
			
			cout<<", nIncompatible[permutation[i]]="<<nIncompatible[permutation[i]];
			if(nMinDaysConstraintsBroken[permutation[i]]>0.0)
				cout<<", nMinDaysConstraintsBroken[permutation[i]]="<<nMinDaysConstraintsBroken[permutation[i]];
				
			if(gt.rules.groupActivitiesInInitialOrderList.count()>0 && fatherActivityInInitialOrder[permutation[i]]>=0)
				cout<<" (grouped with id "<<gt.rules.internalActivitiesList[fatherActivityInInitialOrder[permutation[i]]].id<<")";
			
			cout<<endl;
		}
		cout<<"End - the order of activities (id-s):"<<endl;
	}

	QString s="";
	s+=GeneratePreTranslate::tr("This is the initial evaluation order of activities computed by FET."
	 " If the generation gets stuck after a certain number of activities, please check the activity"
	 " corresponding to this number+1. You might find errors in your input.");
	s+="\n\n";
	s+=GeneratePreTranslate::tr("The initial order of activities (id-s):");
	s+="\n\n";
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		initialOrderOfActivitiesIndices[i]=permutation[i];
	
		s+=GeneratePreTranslate::tr("No: %1", "Number").arg(i+1);
		s+=", ";
	
		Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
		s+=GeneratePreTranslate::tr("Id: %1", "Id of activity").arg(act->id);
		s+=", ";
		
		s+=GeneratePreTranslate::tr("Teachers: %1").arg(act->teachersNames.join(", "));
		s+=", ";
		s+=GeneratePreTranslate::tr("Subject: %1").arg(act->subjectName);
		s+=", ";
		//if(act->activityTagsNames.count()>0)
		s+=GeneratePreTranslate::tr("Activity tags: %1").arg(act->activityTagsNames.join(", "));
		s+=", ";
		s+=GeneratePreTranslate::tr("Students: %1").arg(act->studentsNames.join(", "));

		if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
			s+=", ";
			s+=GeneratePreTranslate::tr("nIncompatible: %1").arg(nIncompatible[permutation[i]]);
			if(nMinDaysConstraintsBroken[permutation[i]]>0.0){
				s+=", ";
				s+=GeneratePreTranslate::tr("nMinDaysConstraintsBroken: %1").arg(nMinDaysConstraintsBroken[permutation[i]]);
			}
			
			if(fatherActivityInInitialOrder[permutation[i]]>=0){
				s+=" ";
				s+=GeneratePreTranslate::tr("(grouped with id %1)").arg(gt.rules.internalActivitiesList[fatherActivityInInitialOrder[permutation[i]]].id);
			}
		}
		
		s+="\n";
	}
	s+=GeneratePreTranslate::tr("End - the order of activities (id-s)");
	s+="\n";
	initialOrderOfActivities=s;
	
	if(initialOrderStream!=NULL){
		(*initialOrderStream)<<s;
		(*initialOrderStream).flush();
	}
}
