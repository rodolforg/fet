/*
File solution.cpp
*/

/***************************************************************************
                          solution.cpp  -  description
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

//Teachers free periods code contributed by Volker Dirr (http://timetabling.de/)

#include <QFile>
#include <QTextStream>

#include "timetable_defs.h"
#include "solution.h"
#include "rules.h"
#include "timeconstraint.h"

#include "matrix.h"

#include <QMap>
#include <QMultiMap>

//extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix2D<bool> breakDayHour;
//extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix3D<bool> teacherNotAvailableDayHour;

//critical function here - must be optimized for speed
void Solution::copy(Rules& r, Solution& c){
	this->_fitness=c._fitness;

	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i] = c.times[i];
		this->rooms[i]=c.rooms[i];
	}
	//memcpy(times, c.times, r.nActivities * sizeof(times[0]));
	
	this->changedForMatrixCalculation=c.changedForMatrixCalculation;

	//added in version 5.2.0
	conflictsWeightList=c.conflictsWeightList;
	conflictsDescriptionList=c.conflictsDescriptionList;
	conflictsTotal=c.conflictsTotal;
	
	teachersMatrixReady=c.teachersMatrixReady;
	subgroupsMatrixReady=c.subgroupsMatrixReady;
	roomsMatrixReady=c.roomsMatrixReady;
	
	nPlacedActivities=c.nPlacedActivities;
}

void Solution::init(Rules& r){
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_fitness=-1;
	
	this->changedForMatrixCalculation=true;
}

void Solution::makeUnallocated(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_fitness=-1;

	this->changedForMatrixCalculation=true;
}

double Solution::fitness(Rules& r, QString* conflictsString){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	if(this->_fitness>=0)
		assert(this->changedForMatrixCalculation==false);
		
	if(this->_fitness>=0 && conflictsString==NULL)
	//If you want to see the log, you have to recompute the fitness, even if it is
	//already computed
		return this->_fitness;
		
	this->changedForMatrixCalculation=true;
	
	this->_fitness=0;
	//I AM NOT SURE IF THE COMMENT BELOW IS DEPRECATED/FALSE NOW (IT IS OLD).
	//here we must not have compulsory activity preferred time nor
	//compulsory activities same time and/or hour
	//Also, here I compute soft fitness (for faster results,
	//I do not want to pass again through the constraints)
	
	this->conflictsDescriptionList.clear();
	this->conflictsWeightList.clear();
	
	this->teachersMatrixReady=false;
	this->subgroupsMatrixReady=false;
	this->roomsMatrixReady=false;
	
	this->nPlacedActivities=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME)
			this->nPlacedActivities++;
		
	for(int i=0; i<r.nInternalTimeConstraints; i++){
		QList<QString> sl;
		QList<double> cl;
		this->_fitness += r.internalTimeConstraintsList[i]->fitness(*this, r, cl, sl, conflictsString);
		
		conflictsWeightList+=cl;
		conflictsDescriptionList+=sl;
	}	
	for(int i=0; i<r.nInternalSpaceConstraints; i++){
		QList<QString> sl;
		QList<double> cl;
		this->_fitness += r.internalSpaceConstraintsList[i]->fitness(*this, r, cl, sl, conflictsString);
		conflictsWeightList+=cl;
		conflictsDescriptionList+=sl;
	}
		
	this->conflictsTotal=0;
	foreach(double cn, conflictsWeightList){
		//cout<<"cn=="<<cn<<endl;
		conflictsTotal+=cn;
	}
		
#if 0
	//I cannot put this test. I got situations of assert failed with 15.2 != 15.2 ??? Maybe rounding errors
	if(this->_fitness!=conflictsTotal){
		cout<<"this->_fitness=="<<this->_fitness<<endl;
		cout<<"conflictsTotal=="<<conflictsTotal<<endl;
	}
	assert(this->_fitness==conflictsTotal);//TODO
#endif
		
	//sort descending according to conflicts in O(n log n)
	int ttt=conflictsWeightList.count();
		
	QMultiMap<double, QString> map;
	assert(conflictsWeightList.count()==conflictsDescriptionList.count());
	for(int i=0; i<conflictsWeightList.count(); i++)
		map.insert(conflictsWeightList.at(i), conflictsDescriptionList.at(i));
		
	conflictsWeightList.clear();
	conflictsDescriptionList.clear();
	
	QMapIterator<double, QString> i(map);
	while (i.hasNext()) {
		i.next();
		conflictsWeightList.prepend(i.key());
		conflictsDescriptionList.prepend(i.value());
	}
	
	for(int i=0; i<conflictsWeightList.count()-1; i++)
		assert(conflictsWeightList.at(i) >= conflictsWeightList.at(i+1));
		
	assert(conflictsWeightList.count()==conflictsDescriptionList.count());
	assert(conflictsWeightList.count()==ttt);
	
	this->changedForMatrixCalculation=false;

	return this->_fitness;
}

int Solution::getTeachersMatrix(Rules& r, Matrix3D<int>& a){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	int conflicts=0;
	
	a.resize(r.nInternalTeachers, r.nDaysPerWeek, r.nHoursPerDay);

	int i;
	for(i=0; i<r.nInternalTeachers; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME) {
			int hour = this->times[i] / r.nDaysPerWeek;
			int day = this->times[i] % r.nDaysPerWeek;
			Activity* act=&r.internalActivitiesList[i];
			for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
				for(int it=0; it<act->iTeachersList.count(); it++){
					int tch=act->iTeachersList.at(it);
					int tmp=a[tch][day][hour+dd];
					conflicts += tmp==0 ? 0 : 1;
					a[tch][day][hour+dd]++;
				}
		}

	this->changedForMatrixCalculation=false;
		
	return conflicts;
}

int Solution::getSubgroupsMatrix(Rules& r, Matrix3D<int>& a){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	int conflicts=0;
	
	a.resize(r.nInternalSubgroups, r.nDaysPerWeek, r.nHoursPerDay);

	int i;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME){
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			Activity* act = &r.internalActivitiesList[i];
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++)
				for(int isg=0; isg < act->iSubgroupsList.count(); isg++){ //isg => index subgroup
					int sg = act->iSubgroupsList.at(isg); //sg => subgroup
					int tmp=a[sg][day][hour+dd];
					conflicts += tmp==0 ? 0 : 1;
					a[sg][day][hour+dd]++;
				}
		}
		
	this->changedForMatrixCalculation=false;
		
	return conflicts;
}

//The following 2 functions (GetTeachersTimetable & GetSubgroupsTimetable)
//are very similar to the above 2 ones (GetTeachersMatrix & GetSubgroupsMatrix)
void Solution::getTeachersTimetable(Rules& r, Matrix3D<int>& a, Matrix3D<QList<int> >& b){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	a.resize(r.nInternalTeachers, r.nDaysPerWeek, r.nHoursPerDay);
	b.resize(TEACHERS_FREE_PERIODS_N_CATEGORIES, r.nDaysPerWeek, r.nHoursPerDay);
	
	int i, j, k;
	for(i=0; i<r.nInternalTeachers; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) 
		if(this->times[i]!=UNALLOCATED_TIME) {
			act=&r.internalActivitiesList[i];
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			for(int dd=0; dd < act->duration; dd++){
				assert(hour+dd<r.nHoursPerDay);
				for(int ti=0; ti<act->iTeachersList.count(); ti++){
					int tch = act->iTeachersList.at(ti); //teacher index
					assert(a[tch][day][hour+dd]==UNALLOCATED_ACTIVITY);
					a[tch][day][hour+dd]=i;
				}
			}
		}

	//Prepare teachers free periods timetable.
	//Code contributed by Volker Dirr (http://timetabling.de/) BEGIN
	int d,h,tch;
	for(d=0; d<r.nDaysPerWeek; d++){
		for(h=0; h<r.nHoursPerDay; h++){
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				b[tfp][d][h].clear();
			}
		}
	}
	for(tch=0; tch<r.nInternalTeachers; tch++){
		for(d=0; d<r.nDaysPerWeek; d++){
			int firstPeriod=-1;
			int lastPeriod=-1;
			for(h=0; h<r.nHoursPerDay; h++){
				if(a[tch][d][h]!=UNALLOCATED_ACTIVITY){
					if(firstPeriod==-1)
						firstPeriod=h;
					lastPeriod=h;
				}
			}
			if(firstPeriod==-1){
				for(h=0; h<r.nHoursPerDay; h++){
					b[TEACHER_HAS_A_FREE_DAY][d][h]<<tch;
				}
			} else {
				for(h=0; h<firstPeriod; h++){
					if(firstPeriod-h==1){
						b[TEACHER_MUST_COME_EARLIER][d][h]<<tch;
					}
					else {
						b[TEACHER_MUST_COME_MUCH_EARLIER][d][h]<<tch;
					}
				}
				for(; h<lastPeriod+1; h++){
					if(a[tch][d][h]==UNALLOCATED_ACTIVITY){
						if(a[tch][d][h+1]==UNALLOCATED_ACTIVITY){
							if(a[tch][d][h-1]==UNALLOCATED_ACTIVITY){
								b[TEACHER_HAS_BIG_GAP][d][h]<<tch;
							} else {
								b[TEACHER_HAS_BORDER_GAP][d][h]<<tch;
							}
						} else {
							if(a[tch][d][h-1]==UNALLOCATED_ACTIVITY){
								b[TEACHER_HAS_BORDER_GAP][d][h]<<tch;
							} else {
								b[TEACHER_HAS_SINGLE_GAP][d][h]<<tch;
							}
						}
					}
				}
				for(; h<r.nHoursPerDay; h++){
					if(lastPeriod-h==-1){
						b[TEACHER_MUST_STAY_LONGER][d][h]<<tch;
					}
					else {
						b[TEACHER_MUST_STAY_MUCH_LONGER][d][h]<<tch;
					}
				}
			}
		}
	}
	//care about not available teacher and breaks
	for(tch=0; tch<r.nInternalTeachers; tch++){
		for(d=0; d<r.nDaysPerWeek; d++){
			for(h=0; h<r.nHoursPerDay; h++){
				if(teacherNotAvailableDayHour[tch][d][h]==true || breakDayHour[d][h]==true){
					int removed=0;
					for(int tfp=0; tfp<TEACHER_IS_NOT_AVAILABLE; tfp++){
						if(b[tfp][d][h].contains(tch)){
							removed+=b[tfp][d][h].removeAll(tch);
							if(breakDayHour[d][h]==false)
								b[TEACHER_IS_NOT_AVAILABLE][d][h]<<tch;
						}
					}
					assert(removed==1);
				}
			}
		}
	}
	//END of Code contributed by Volker Dirr (http://timetabling.de/) END
	//bool visited[MAX_TEACHERS];
	Matrix1D<bool> visited;
	visited.resize(r.nInternalTeachers);
	for(d=0; d<r.nDaysPerWeek; d++){
		for(h=0; h<r.nHoursPerDay; h++){
			for(tch=0; tch<r.nInternalTeachers; tch++)
				visited[tch]=false;
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				foreach(int tch, b[tfp][d][h]){
					assert(!visited[tch]);
					visited[tch]=true;
				}
			}
		}
	}
}

void Solution::getSubgroupsTimetable(Rules& r, Matrix3D<int>& a){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	a.resize(r.nInternalSubgroups, r.nDaysPerWeek, r.nHoursPerDay);
	
	int i, j, k;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME) {
			act=&r.internalActivitiesList[i];
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			for(int dd=0; dd < act->duration; dd++){
				assert(hour+dd<r.nHoursPerDay);
			
				for(int isg=0; isg < act->iSubgroupsList.count(); isg++){ //isg -> index subgroup
					int sg = act->iSubgroupsList.at(isg); //sg -> subgroup
					assert(a[sg][day][hour+dd]==UNALLOCATED_ACTIVITY);
					a[sg][day][hour+dd]=i;
				}
			}
		}
}

int Solution::getRoomsMatrix(
	Rules& r, 
	Matrix3D<int>& a)
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int conflicts=0;
	
	a.resize(r.nInternalRooms, r.nDaysPerWeek, r.nHoursPerDay);

	int i;
	for(i=0; i<r.nInternalRooms; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++){
		int room=this->rooms[i];
		
		if(times[i]!=UNALLOCATED_TIME && room!=UNALLOCATED_SPACE && room!=UNSPECIFIED_ROOM) {
			int hour=times[i]/r.nDaysPerWeek;
			int day=times[i]%r.nDaysPerWeek;
			
			Activity* act=&r.internalActivitiesList[i];
			for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++){
				int tmp=a[room][day][hour+dd];
				conflicts += tmp==0 ? 0 : 1;
				a[room][day][hour+dd]++;
			}
		}
	}
	
	this->changedForMatrixCalculation=false;
	
	return conflicts;
}

void Solution::getRoomsTimetable(
	Rules& r,
	Matrix3D<int>& a)
{
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	a.resize(r.nInternalRooms, r.nDaysPerWeek, r.nHoursPerDay);
	
	int i, j, k;
	for(i=0; i<r.nInternalRooms; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		int room=this->rooms[i];
		
		if(times[i]!=UNALLOCATED_TIME && room!=UNALLOCATED_SPACE && room!=UNSPECIFIED_ROOM){
			int hour=times[i]/r.nDaysPerWeek;
			int day=times[i]%r.nDaysPerWeek;
		
			for(int dd=0; dd < act->duration; dd++){
				assert(hour+dd<r.nHoursPerDay);
			
				assert(a[room][day][hour+dd]==UNALLOCATED_ACTIVITY);
				a[room][day][hour+dd]=i;
			}
		}
	}
}
