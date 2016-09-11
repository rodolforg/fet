/*
File timeconstraint.cpp
*/

/***************************************************************************
                          timeconstraint.cpp  -  description
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
#include "timeconstraint.h"
#include "rules.h"
#include "solution.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "studentsset.h"

#include "matrix.h"

#include <QString>

#include "messageboxes.h"

#include <QSet>

//for min max functions
#include <algorithm>
using namespace std;

static QString trueFalse(bool x){
	if(!x)
		return QString("false");
	else
		return QString("true");
}

static QString yesNoTranslated(bool x){
	if(!x)
		return QCoreApplication::translate("TimeConstraint", "no", "no - meaning negation");
	else
		return QCoreApplication::translate("TimeConstraint", "yes", "yes - meaning affirmative");
}

//The following 2 matrices are kept to make the computation faster
//They are calculated only at the beginning of the computation of the fitness
//of the solution.
/*static qint8 subgroupsMatrix[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint8 teachersMatrix[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
static Matrix3D<int> subgroupsMatrix;
static Matrix3D<int> teachersMatrix;

static int teachers_conflicts=-1;
static int subgroups_conflicts=-1;

//extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix2D<bool> breakDayHour;

/*extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
extern Matrix3D<bool> teacherNotAvailableDayHour;

extern Matrix3D<bool> subgroupNotAvailableDayHour;

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

QString getActivityDetailedDescription(Rules& r, int id)
{
	QString s;

	/*int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==id)
			break;
	
	if(ai==r.activitiesList.size()){
		s+=QCoreApplication::translate("Activity", "Invalid (inexistent) id for activity");
		return s;
	}
	assert(ai<r.activitiesList.size());

	Activity* act=r.activitiesList.at(ai);*/

	Activity* act=r.activitiesPointerHash.value(id, NULL);
	if(act==NULL){
		s+=QCoreApplication::translate("Activity", "Invalid (inexistent) id for activity");
		return s;
	}

	if(act->activityTagsNames.count()>0){
		s+=QCoreApplication::translate("Activity", "T:%1, S:%2, AT:%3, St:%4", "This is an important translation for an activity's detailed description, please take care (it appears in many places in constraints)."
		 "The abbreviations are: Teachers, Subject, Activity tags, Students. This variant includes activity tags").arg(act->teachersNames.join(",")).arg(act->subjectName).arg(act->activityTagsNames.join(",")).arg(act->studentsNames.join(","));
	}
	else{
		s+=QCoreApplication::translate("Activity", "T:%1, S:%2, St:%3", "This is an important translation for an activity's detailed description, please take care (it appears in many places in constraints)."
		 "The abbreviations are: Teachers, Subject, Students. There are no activity tags here").arg(act->teachersNames.join(",")).arg(act->subjectName).arg(act->studentsNames.join(","));
	}
	return s;
}

TimeConstraint::TimeConstraint()
{
	type=CONSTRAINT_GENERIC_TIME;
	
	active=true;
	comments=QString("");
}

TimeConstraint::~TimeConstraint()
{
}

TimeConstraint::TimeConstraint(double wp)
{
	type=CONSTRAINT_GENERIC_TIME;

	weightPercentage=wp;
	assert(wp<=100 && wp>=0);

	active=true;
	comments=QString("");
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(): TimeConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_TIME;
	this->weightPercentage=100;
}

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(double wp): TimeConstraint(wp)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_TIME;
}

bool ConstraintBasicCompulsoryTime::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintBasicCompulsoryTime::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintBasicCompulsoryTime::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s = "<ConstraintBasicCompulsoryTime>\n";
	assert(this->weightPercentage==100.0);
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintBasicCompulsoryTime>\n";
	return s;
}

QString ConstraintBasicCompulsoryTime::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	return begin+tr("Basic compulsory constraints (time)") + ", " + tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage))+end;
}

QString ConstraintBasicCompulsoryTime::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("These are the basic compulsory constraints (referring to time allocation) for any timetable");
	s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("The basic time constraints try to avoid:");s+="\n";
	s+=QString("- ");s+=tr("teachers assigned to more than one activity simultaneously");s+="\n";
	s+=QString("- ");s+=tr("students assigned to more than one activity simultaneously");s+="\n";
	
	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintBasicCompulsoryTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString){
	assert(r.internalStructureComputed);

	int teachersConflicts, subgroupsConflicts;
	
	assert(weightPercentage==100.0);

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the teacher and subgroups conflicts faster this way.
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	
		subgroups_conflicts = subgroupsConflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = teachersConflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	else{
		assert(subgroups_conflicts>=0);
		assert(teachers_conflicts>=0);
		subgroupsConflicts = subgroups_conflicts;
		teachersConflicts = teachers_conflicts;
	}

	int i,dd;

	qint64 unallocated; //unallocated activities
	int late; //late activities
	int nte; //number of teacher exhaustions
	int nse; //number of students exhaustions

	//Part without logging..................................................................
	if(conflictsString==NULL){
		//Unallocated or late activities
		unallocated=0;
		late=0;
		for(i=0; i<r.nInternalActivities; i++){
			if(c.times[i]==UNALLOCATED_TIME){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				//Take care: MAX_ACTIVITIES*this_constant <= INT_MAX
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
			}
			else{
				//Calculates the number of activities that are scheduled too late (in fact we
				//calculate a function that increases as the activity is getting late)
				int h=c.times[i]/r.nDaysPerWeek;
				dd=r.internalActivitiesList[i].duration;
				if(h+dd>r.nHoursPerDay){
					int tmp;
					tmp=1;
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].iSubgroupsList.count();
					//multiplied with the number
					//of subgroups implied, for seeing the importance of the
					//activity
				}
			}
		}
		
		assert(late==0);

		//Below, for teachers and students, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, if the matrix teachersMatrix[teacher][day][hour]==2, it is ok.

		//Calculates the number of teachers exhaustion (when he has to teach more than
		//one activity at the same time)
		/*nte=0;
		for(i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=teachersMatrix[i][j][k]-2;
					if(tmp>0)
						nte+=tmp;
				}*/
		nte = teachersConflicts; //faster
		
		assert(nte==0);

		//Calculates the number of subgroups exhaustion (a subgroup cannot attend two
		//activities at the same time)
		/*nse=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=subgroupsMatrix[i][j][k]-2;
					if(tmp>0)
						nse += tmp;
				}*/
		nse = subgroupsConflicts; //faster
		
		assert(nse==0);			
	}
	//part with logging....................................................................
	else{
		//Unallocated or late activities
		unallocated=0;
		late=0;
		for(i=0; i<r.nInternalActivities; i++){
			if(c.times[i]==UNALLOCATED_TIME){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				//Take care: MAX_ACTIVITIES*this_constant <= INT_MAX
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
				if(conflictsString!=NULL){
					QString s= tr("Time constraint basic compulsory broken: unallocated activity with id=%1 (%2)",
						"%2 is the detailed description of activity - teachers, subject, students")
						.arg(r.internalActivitiesList[i].id).arg(getActivityDetailedDescription(r, r.internalActivitiesList[i].id));
					s+=" - ";
					s += tr("this increases the conflicts total by %1")
					 .arg(CustomFETString::number(weightPercentage/100 * 10000));
					//s += "\n";
					
					dl.append(s);
					cl.append(weightPercentage/100 * 10000);

					(*conflictsString) += s + "\n";
				}
			}
			else{
				//Calculates the number of activities that are scheduled too late (in fact we
				//calculate a function that increases as the activity is getting late)
				int h=c.times[i]/r.nDaysPerWeek;
				dd=r.internalActivitiesList[i].duration;
				if(h+dd>r.nHoursPerDay){
					assert(0);	
				
					int tmp;
					tmp=1;
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].iSubgroupsList.count();
					//multiplied with the number
					//of subgroups implied, for seeing the importance of the
					//activity

					if(conflictsString!=NULL){
						QString s=tr("Time constraint basic compulsory");
						s+=": ";
						s+=tr("activity with id=%1 is late.")
						 .arg(r.internalActivitiesList[i].id);
						s+=" ";
						s+=tr("This increases the conflicts total by %1")
						 .arg(CustomFETString::number((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].iSubgroupsList.count()*weightPercentage/100));
						s+="\n";
						
						dl.append(s);
						cl.append((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].iSubgroupsList.count()*weightPercentage/100);

						(*conflictsString) += s+"\n";
					}
				}
			}
		}

		//Below, for teachers and students, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, if the matrix teachersMatrix[teacher][day][hour]==2,
		//that is ok.

		//Calculates the number of teachers exhaustion (when he has to teach more than
		//one activity at the same time)
		nte=0;
		for(i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=teachersMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=tr("Time constraint basic compulsory");
							s+=": ";
							s+=tr("teacher with name %1 has more than one allocated activity on day %2, hour %3")
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[j])
							 .arg(r.hoursOfTheDay[k]);
							s+=". ";
							s+=tr("This increases the conflicts total by %1")
							 .arg(CustomFETString::number(tmp*weightPercentage/100));
						
							(*conflictsString)+= s+"\n";
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						}
						nte+=tmp;
					}
				}

		assert(nte==0);
		
		//Calculates the number of subgroups exhaustion (a subgroup cannot attend two
		//activities at the same time)
		nse=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=subgroupsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=tr("Time constraint basic compulsory");
							s+=": ";
							s+=tr("subgroup %1 has more than one allocated activity on day %2, hour %3")
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[j])
							 .arg(r.hoursOfTheDay[k]);
							s+=". ";
							s+=tr("This increases the conflicts total by %1")
							 .arg(CustomFETString::number((subgroupsMatrix[i][j][k]-1)*weightPercentage/100));
							 
							dl.append(s);
							cl.append((subgroupsMatrix[i][j][k]-1)*weightPercentage/100);
						
							*conflictsString += s+"\n";
						}
						nse += tmp;
					}
				}
			
		assert(nse==0);
	}

	/*if(nte!=teachersConflicts){
		cout<<"nte=="<<nte<<", teachersConflicts=="<<teachersConflicts<<endl;
		cout<<c.getTeachersMatrix(r, teachersMatrix)<<endl;
	}
	if(nse!=subgroupsConflicts){
		cout<<"nse=="<<nse<<", subgroupsConflicts=="<<subgroupsConflicts<<endl;
		cout<<c.getSubgroupsMatrix(r, subgroupsMatrix)<<endl;
	}*/
	
	/*assert(nte==teachersConflicts); //just a check, works only on logged fitness calculation
	assert(nse==subgroupsConflicts);*/

	//return int (ceil ( weight * (unallocated + late + nte + nse) ) ); //conflicts factor
	return weightPercentage/100 * (unallocated + qint64(late) + qint64(nte) + qint64(nse)); //conflicts factor
}

bool ConstraintBasicCompulsoryTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsoryTime::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintBasicCompulsoryTime::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintBasicCompulsoryTime::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherNotAvailableTimes::ConstraintTeacherNotAvailableTimes()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES;
}

ConstraintTeacherNotAvailableTimes::ConstraintTeacherNotAvailableTimes(double wp, const QString& tn, QList<int> d, QList<int> h)
	: TimeConstraint(wp)
{
	this->teacher=tn;
	assert(d.count()==h.count());
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES;
}

QString ConstraintTeacherNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintTeacherNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacher)+"</Teacher>\n";

	s+="	<Number_of_Not_Available_Times>"+CustomFETString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Not_Available_Time>\n";
	}

	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherNotAvailableTimes>\n";
	return s;
}

QString ConstraintTeacherNotAvailableTimes::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Teacher not available");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacher);s+=", ";

	s+=tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}

	return begin+s+end;
}

QString ConstraintTeacherNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher is not available");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacher);s+="\n";

	s+=tr("Not available at:", "It refers to a teacher");
	s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintTeacherNotAvailableTimes::computeInternalStructure(QWidget* parent, Rules& r){
	//this->teacher_ID=r.searchTeacher(this->teacher);
	teacher_ID=r.teachersHash.value(teacher, -1);

	if(this->teacher_ID<0){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher not available times is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint teacher not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint teacher not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

double ConstraintTeacherNotAvailableTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;

		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the teacher is supposed to be teaching, but he is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory restriction - we only
	//are allowed 1 weekly course for a certain teacher at a certain hour) we calculate
	//5 broken restrictions for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int tch=this->teacher_ID;

	int nbroken;

	nbroken=0;

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);
		
		if(teachersMatrix[tch][d][h]>0){
			nbroken+=teachersMatrix[tch][d][h];
	
			if(conflictsString!=NULL){
				QString s= tr("Time constraint teacher not available");
				s += " ";
				s += tr("broken for teacher: %1 on day %2, hour %3")
				 .arg(r.internalTeachersList[tch]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]);
				s += ". ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number(teachersMatrix[tch][d][h]*weightPercentage/100));
				 
				dl.append(s);
				cl.append(teachersMatrix[tch][d][h]*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	if(this->teacher==t->name)
		return true;
	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintTeacherNotAvailableTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherNotAvailableTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetNotAvailableTimes::ConstraintStudentsSetNotAvailableTimes()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES;
}

ConstraintStudentsSetNotAvailableTimes::ConstraintStudentsSetNotAvailableTimes(double wp, const QString& sn, QList<int> d, QList<int> h)
	 : TimeConstraint(wp){
	this->students = sn;
	assert(d.count()==h.count());
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES;
}

bool ConstraintStudentsSetNotAvailableTimes::computeInternalStructure(QWidget* parent, Rules& r){
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set not available is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	
	
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint students set not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint students set not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	
	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
	return true;
}

bool ConstraintStudentsSetNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintStudentsSetNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";

	s+="	<Number_of_Not_Available_Times>"+CustomFETString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Not_Available_Time>\n";
	}

	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetNotAvailableTimes>\n";
	return s;
}

QString ConstraintStudentsSetNotAvailableTimes::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s=tr("Students set not available");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Students").arg(this->students);s+=", ";

	s+=tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}

	return begin+s+end;
}

QString ConstraintStudentsSetNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set is not available");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->students);s+="\n";

	s+=tr("Not available at:", "It refers to a students set");s+="\n";
	
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetNotAvailableTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	for(int m=0; m<this->iSubgroupsList.count(); m++){
		int sbg=this->iSubgroupsList.at(m);
		
		assert(days.count()==hours.count());
		for(int k=0; k<days.count(); k++){
			int d=days.at(k);
			int h=hours.at(k);
			
			if(subgroupsMatrix[sbg][d][h]>0){
				nbroken+=subgroupsMatrix[sbg][d][h];

				if(conflictsString!=NULL){
					QString s= tr("Time constraint students set not available");
					s += " ";
					s += tr("broken for subgroup: %1 on day %2, hour %3")
					 .arg(r.internalSubgroupsList[sbg]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(r.hoursOfTheDay[h]);
					s += ". ";
					s += tr("This increases the conflicts total by %1")
					 .arg(CustomFETString::number(subgroupsMatrix[sbg][d][h]*weightPercentage/100));
					 
					dl.append(s);
					cl.append(subgroupsMatrix[sbg][d][h]*weightPercentage/100);
				
					*conflictsString += s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetNotAvailableTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetNotAvailableTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingTime::ConstraintActivitiesSameStartingTime()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME;
}

ConstraintActivitiesSameStartingTime::ConstraintActivitiesSameStartingTime(double wp, int nact, const QList<int>& act)
 : TimeConstraint(wp)
 {
	assert(nact>=2);
	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME;
}

bool ConstraintActivitiesSameStartingTime::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingTime::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintActivitiesSameStartingTime::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingTime::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesSameStartingTime>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesSameStartingTime>\n";
	return s;
}

QString ConstraintActivitiesSameStartingTime::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);

	QString s;
	s+=tr("Activities same starting time");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);
		if(i<this->n_activities-1)
			s+=", ";
	}

	return begin+s+end;
}

QString ConstraintActivitiesSameStartingTime::getDetailedDescription(Rules& r){
	QString s;
	
	s=tr("Time constraint");s+="\n";
	s+=tr("Activities must have the same starting time");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity").arg(this->activitiesId[i]).arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled time for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						tmp = abs(day1-day2) + abs(hour1-hour2);
							
						if(tmp>0)
							tmp=1;

						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						tmp = abs(day1-day2) + abs(hour1-hour2);
							
						if(tmp>0)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=tr("Time constraint activities same starting time broken, because activity with id=%1 (%2) is not at the same starting time with activity with id=%3 (%4)",
							"%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]));
							s+=". ";
							s+=tr("Conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
							
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingTime::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesSameStartingTime::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivitiesSameStartingTime::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesNotOverlapping::ConstraintActivitiesNotOverlapping()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING;
}

ConstraintActivitiesNotOverlapping::ConstraintActivitiesNotOverlapping(double wp, int nact, const QList<int>& act)
 : TimeConstraint(wp)
 {
  	assert(nact>=2);
  	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	this->type=CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING;
}

bool ConstraintActivitiesNotOverlapping::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesNotOverlapping::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintActivitiesNotOverlapping::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesNotOverlapping::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesNotOverlapping>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesNotOverlapping>\n";
	return s;
}

QString ConstraintActivitiesNotOverlapping::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Activities not overlapping");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);
		if(i<this->n_activities-1)
			s+=", ";
	}

	return begin+s+end;
}

QString ConstraintActivitiesNotOverlapping::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activities must not overlap");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
			.arg(this->activitiesId[i]).arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesNotOverlapping::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the overlapping hours for all pairs of activities.
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;

						//the number of overlapping hours
						int tt=0;
						if(day1==day2){
							int start=max(hour1, hour2);
							int stop=min(hour1+duration1, hour2+duration2);
							if(stop>start)
								tt+=stop-start;
						}
						
						nbroken+=tt;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
	
						//the number of overlapping hours
						int tt=0;
						if(day1==day2){
							int start=max(hour1, hour2);
							int stop=min(hour1+duration1, hour2+duration2);
							if(stop>start)
								tt+=stop-start;
						}

						//The overlapping hours, considering weekly activities more important than fortnightly ones
						int tmp=tt;

						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){

							QString s=tr("Time constraint activities not overlapping broken: activity with id=%1 (%2) overlaps with activity with id=%3 (%4) on a number of %5 periods",
							 "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]))
							 .arg(tt);
							s+=", ";
							s+=tr("conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesNotOverlapping::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesNotOverlapping::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivitiesNotOverlapping::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinDaysBetweenActivities::ConstraintMinDaysBetweenActivities()
	: TimeConstraint()
{
	type=CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES;
}

ConstraintMinDaysBetweenActivities::ConstraintMinDaysBetweenActivities(double wp, bool cisd, int nact, const QList<int>& act, int n)
 : TimeConstraint(wp)
 {
	this->consecutiveIfSameDay=cisd;

	assert(nact>=2);
	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	assert(n>0);
	this->minDays=n;

	this->type=CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES;
}

bool ConstraintMinDaysBetweenActivities::operator==(ConstraintMinDaysBetweenActivities& c){
	assert(this->n_activities==this->activitiesId.count());
	assert(c.n_activities==c.activitiesId.count());

	if(this->n_activities!=c.n_activities)
		return false;
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]!=c.activitiesId[i])
			return false;
	if(this->minDays!=c.minDays)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	if(this->consecutiveIfSameDay!=c.consecutiveIfSameDay)
		return false;
	return true;
}

bool ConstraintMinDaysBetweenActivities::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintMinDaysBetweenActivities::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintMinDaysBetweenActivities::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintMinDaysBetweenActivities::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintMinDaysBetweenActivities>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Consecutive_If_Same_Day>";s+=trueFalse(this->consecutiveIfSameDay);s+="</Consecutive_If_Same_Day>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<MinDays>"+CustomFETString::number(this->minDays)+"</MinDays>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintMinDaysBetweenActivities>\n";
	return s;
}

QString ConstraintMinDaysBetweenActivities::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Min days between activities");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);s+=", ";
	}
	s+=tr("mD:%1", "Min days").arg(this->minDays);s+=", ";
	s+=tr("CSD:%1", "Consecutive if same day").arg(yesNoTranslated(this->consecutiveIfSameDay));

	return begin+s+end;
}

QString ConstraintMinDaysBetweenActivities::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Minimum number of days between activities");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
			.arg(this->activitiesId[i])
			.arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}
	s+=tr("Minimum number of days=%1").arg(this->minDays);s+="\n";
	s+=tr("Consecutive if same day=%1").arg(yesNoTranslated(this->consecutiveIfSameDay));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintMinDaysBetweenActivities::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the overlapping hours for all pairs of activities.
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);
						if(dist<minDays){
							tt=minDays-dist;
							
							if(this->consecutiveIfSameDay && day1==day2)
								assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}
						
						tmp=tt;
	
						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);

						if(dist<minDays){
							tt=minDays-dist;
							
							if(this->consecutiveIfSameDay && day1==day2)
								assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}

						tmp=tt;
	
						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							QString s=tr("Time constraint min days between activities broken: activity with id=%1 (%2) conflicts with activity with id=%3 (%4), being %5 days too close, on days %6 and %7",
							 "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr. Close here means near")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]))
							 .arg(tt)
							 .arg(r.daysOfTheWeek[day1])
							 .arg(r.daysOfTheWeek[day2]);
							 ;

							s+=", ";
							s+=tr("conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
							s+=".";
							
							if(this->consecutiveIfSameDay && day1==day2){
								s+=" ";
								s+=tr("The activities are placed consecutively in the timetable, because you selected this option"
								 " in case the activities are in the same day");
							}
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
							
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintMinDaysBetweenActivities::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMinDaysBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintMinDaysBetweenActivities::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinDaysBetweenActivities::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinDaysBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinDaysBetweenActivities::hasWrongDayOrHour(Rules& r)
{
	if(minDays>=r.nDaysPerWeek)
		return true;
		
	return false;
}

bool ConstraintMinDaysBetweenActivities::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintMinDaysBetweenActivities::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minDays>=r.nDaysPerWeek)
		minDays=r.nDaysPerWeek-1;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMaxDaysBetweenActivities::ConstraintMaxDaysBetweenActivities()
	: TimeConstraint()
{
	type=CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES;
}

ConstraintMaxDaysBetweenActivities::ConstraintMaxDaysBetweenActivities(double wp, int nact, const QList<int>& act, int n)
 : TimeConstraint(wp)
 {
  	assert(nact>=2);
  	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	assert(n>=0);
	this->maxDays=n;

	this->type=CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES;
}

bool ConstraintMaxDaysBetweenActivities::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintMaxDaysBetweenActivities::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintMaxDaysBetweenActivities::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintMaxDaysBetweenActivities::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintMaxDaysBetweenActivities>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<MaxDays>"+CustomFETString::number(this->maxDays)+"</MaxDays>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintMaxDaysBetweenActivities>\n";
	return s;
}

QString ConstraintMaxDaysBetweenActivities::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Max days between activities");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);s+=", ";
	}
	s+=tr("MD:%1", "Abbreviation for maximum days").arg(this->maxDays);

	return begin+s+end;
}

QString ConstraintMaxDaysBetweenActivities::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Maximum number of days between activities");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
			.arg(this->activitiesId[i])
			.arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}
	s+=tr("Maximum number of days=%1").arg(this->maxDays);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintMaxDaysBetweenActivities::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the overlapping hours for all pairs of activities.
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				//int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						//int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);
						if(dist>maxDays){
							tt=dist-maxDays;
							
							//if(this->consecutiveIfSameDay && day1==day2)
							//	assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}
						
						tmp=tt;
	
						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				//int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						//int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);

						if(dist>maxDays){
							tt=dist-maxDays;
							
							//if(this->consecutiveIfSameDay && day1==day2)
							//	assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}

						tmp=tt;
	
						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							QString s=tr("Time constraint max days between activities broken: activity with id=%1 (%2) conflicts with activity with id=%3 (%4), being %5 days too far away"
							 ", on days %6 and %7", "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]))
							 .arg(tt)
							 .arg(r.daysOfTheWeek[day1])
							 .arg(r.daysOfTheWeek[day2]);
							 
							s+=", ";
							s+=tr("conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
							s+=".";
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
							
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintMaxDaysBetweenActivities::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMaxDaysBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintMaxDaysBetweenActivities::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMaxDaysBetweenActivities::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMaxDaysBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintMaxDaysBetweenActivities::hasWrongDayOrHour(Rules& r)
{
	if(maxDays>=r.nDaysPerWeek)
		return true;
		
	return false;
}

bool ConstraintMaxDaysBetweenActivities::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintMaxDaysBetweenActivities::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxDays>=r.nDaysPerWeek)
		maxDays=r.nDaysPerWeek-1;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinGapsBetweenActivities::ConstraintMinGapsBetweenActivities()
	: TimeConstraint()
{
	type=CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES;
}

ConstraintMinGapsBetweenActivities::ConstraintMinGapsBetweenActivities(double wp, int nact, const QList<int>& actList, int ngaps)
 : TimeConstraint(wp)
 {
	this->n_activities=nact;
	assert(nact==actList.count());
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(actList.at(i));

	assert(ngaps>0);
	this->minGaps=ngaps;

	this->type=CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES;
}

bool ConstraintMinGapsBetweenActivities::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintMinGapsBetweenActivities::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintMinGapsBetweenActivities::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintMinGapsBetweenActivities::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintMinGapsBetweenActivities>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<MinGaps>"+CustomFETString::number(this->minGaps)+"</MinGaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintMinGapsBetweenActivities>\n";
	return s;
}

QString ConstraintMinGapsBetweenActivities::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Min gaps between activities");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);s+=", ";
	}
	s+=tr("mG:%1", "Minimum number of gaps").arg(this->minGaps);

	return begin+s+end;
}

QString ConstraintMinGapsBetweenActivities::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Minimum gaps between activities (if activities on the same day)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
			.arg(this->activitiesId[i])
			.arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}
	s+=tr("Minimum number of gaps=%1").arg(this->minGaps);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintMinGapsBetweenActivities::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	nbroken=0;
	for(int i=1; i<this->_n_activities; i++){
		int t1=c.times[this->_activities[i]];
		if(t1!=UNALLOCATED_TIME){
			int day1=t1%r.nDaysPerWeek;
			int hour1=t1/r.nDaysPerWeek;
			int duration1=r.internalActivitiesList[this->_activities[i]].duration;

			for(int j=0; j<i; j++){
				int t2=c.times[this->_activities[j]];
				if(t2!=UNALLOCATED_TIME){
					int day2=t2%r.nDaysPerWeek;
					int hour2=t2/r.nDaysPerWeek;
					int duration2=r.internalActivitiesList[this->_activities[j]].duration;
				
					int tmp;
					int tt=0;
					int dist=abs(day1-day2);
					
					if(dist==0){ //same day
						assert(day1==day2);
						if(hour2>=hour1){
							//assert(hour1+duration1<=hour2); not true for activities which are not incompatible
							if(hour1+duration1+minGaps > hour2)
								tt = (hour1+duration1+minGaps) - hour2;
						}
						else{
							//assert(hour2+duration2<=hour1); not true for activities which are not incompatible
							if(hour2+duration2+minGaps > hour1)
								tt = (hour2+duration2+minGaps) - hour1;
						}
					}

					tmp=tt;
	
					nbroken+=tmp;

					if(tt>0 && conflictsString!=NULL){
						QString s=tr("Time constraint min gaps between activities broken: activity with id=%1 (%2) conflicts with activity with id=%3 (%4), they are on the same day %5 and there are %6 extra hours between them",
							"%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
						 .arg(this->activitiesId[i])
						 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
						 .arg(this->activitiesId[j])
						 .arg(getActivityDetailedDescription(r, this->activitiesId[j]))
						 .arg(r.daysOfTheWeek[day1])
						 .arg(tt);

						s+=", ";
						s+=tr("conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
						s+=".";
							
						dl.append(s);
						cl.append(tmp*weightPercentage/100);
							
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintMinGapsBetweenActivities::hasWrongDayOrHour(Rules& r)
{
	if(minGaps>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintMinGapsBetweenActivities::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintMinGapsBetweenActivities::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGaps>r.nHoursPerDay)
		minGaps=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxHoursDaily::ConstraintTeachersMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_DAILY;
}

ConstraintTeachersMaxHoursDaily::ConstraintTeachersMaxHoursDaily(double wp, int maxhours)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;

	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_DAILY;
}

bool ConstraintTeachersMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintTeachersMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers max hours daily"), s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MH:%1", "Maximum hours (daily)").arg(this->maxHoursDaily);

	return begin+s+end;
}

QString ConstraintTeachersMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>this->maxHoursDaily)
					nbroken++;
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>this->maxHoursDaily){
					nbroken++;

					if(conflictsString!=NULL){
						QString s=(tr(
						 "Time constraint teachers max %1 hours daily broken for teacher %2, on day %3, length=%4.")
						 .arg(CustomFETString::number(this->maxHoursDaily))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 .arg(n_hours_daily)
						 )
						 +
						 " "
						 +
						 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
						
						dl.append(s);
						cl.append(weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeachersMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxHoursDaily::ConstraintTeacherMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_HOURS_DAILY;
}

ConstraintTeacherMaxHoursDaily::ConstraintTeacherMaxHoursDaily(double wp, int maxhours, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MAX_HOURS_DAILY;
}

bool ConstraintTeacherMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeacherMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher max hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=", ";
	s+=tr("MH:%1", "Maximum hours (daily)").arg(this->maxHoursDaily);

	return begin+s+end;
}

QString ConstraintTeacherMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>this->maxHoursDaily){
				nbroken++;
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint teacher max %1 hours daily broken for teacher %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->maxHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(n_hours_daily)
					 )
					 +" "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
						
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeacherMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously(double wp, int maxhours)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;

	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers max hours continuously");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MH:%1", "Maximum hours (continuously)").arg(this->maxHoursContinuously);

	return begin+s+end;
}

QString ConstraintTeachersMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	for(int i=0; i<r.nInternalTeachers; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(teachersMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint teachers max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint teachers max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxHoursContinuously::ConstraintTeacherMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeacherMaxHoursContinuously::ConstraintTeacherMaxHoursContinuously(double wp, int maxhours, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeacherMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeacherMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher max hours continuously");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=", ";
	s+=tr("MH:%1", "Maximum hours continuously").arg(this->maxHoursContinuously);

	return begin+s+end;
}

QString ConstraintTeacherMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	int i=this->teacher_ID;
	for(int d=0; d<r.nDaysPerWeek; d++){
		int nc=0;
		for(int h=0; h<r.nHoursPerDay; h++){
			if(teachersMatrix[i][d][h]>0)
				nc++;
			else{
				if(nc>this->maxHoursContinuously){
					nbroken++;

					if(conflictsString!=NULL){
						QString s=(tr(
						 "Time constraint teacher max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
						 .arg(CustomFETString::number(this->maxHoursContinuously))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 .arg(nc)
						 )
						 +
						 " "
						 +
						 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
						
						dl.append(s);
						cl.append(weightPercentage/100);
			
						*conflictsString+= s+"\n";
					}
				}
			
				nc=0;
			}
		}

		if(nc>this->maxHoursContinuously){
			nbroken++;

			if(conflictsString!=NULL){
				QString s=(tr(
				 "Time constraint teacher max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
				 .arg(CustomFETString::number(this->maxHoursContinuously))
				 .arg(r.internalTeachersList[i]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(nc)
				 )
				 +
				 " "
				 +
				 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
						
				dl.append(s);
				cl.append(weightPercentage/100);
			
				*conflictsString+= s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersActivityTagMaxHoursContinuously::ConstraintTeachersActivityTagMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersActivityTagMaxHoursContinuously::ConstraintTeachersActivityTagMaxHoursContinuously(double wp, int maxhours, const QString& activityTag)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;
	this->activityTagName=activityTag;

	this->type=CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);
	
	this->canonicalTeachersList.clear();
	for(int i=0; i<r.nInternalTeachers; i++){
		bool found=false;
	
		Teacher* tch=r.internalTeachersList[i];
		foreach(int actIndex, tch->activitiesForTeacher){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalTeachersList.append(i);
	}

	return true;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersActivityTagMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersActivityTagMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersActivityTagMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersActivityTagMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers for activity tag %1 have max %2 hours continuously").arg(this->activityTagName).arg(this->maxHoursContinuously);s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTeachersActivityTagMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers, for an activity tag, must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName); s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersActivityTagMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->canonicalTeachersList){
		Teacher* tch=r.internalTeachersList[i];
		int crtTeacherTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtTeacherTimetableActivityTag[d][h]=-1;
		foreach(int ai, tch->activitiesForTeacher)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtTeacherTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtTeacherTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}
	
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				bool inc=false;
				if(crtTeacherTimetableActivityTag[d][h]==this->activityTagIndex)
					inc=true;
				
				if(inc){
					nc++;
				}
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint teachers activity tag %1 max %2 hours continuously broken for teacher %3, on day %4, length=%5.")
							 .arg(this->activityTagName)
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint teachers activity tag %1 max %2 hours continuously broken for teacher %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	return s->name==this->activityTagName;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersActivityTagMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
ConstraintTeacherActivityTagMaxHoursContinuously::ConstraintTeacherActivityTagMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeacherActivityTagMaxHoursContinuously::ConstraintTeacherActivityTagMaxHoursContinuously(double wp, int maxhours, const QString& teacher, const QString& activityTag)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;
	this->teacherName=teacher;
	this->activityTagName=activityTag;

	this->type=CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);

	this->canonicalTeachersList.clear();
	int i=this->teacher_ID;
	bool found=false;
	
	Teacher* tch=r.internalTeachersList[i];
	foreach(int actIndex, tch->activitiesForTeacher){
		if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
			found=true;
			break;
		}
	}
		
	if(found)
		this->canonicalTeachersList.append(i);

	return true;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherActivityTagMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherActivityTagMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherActivityTagMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeacherActivityTagMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher %1 for activity tag %2 has max %3 hours continuously").arg(this->teacherName).arg(this->activityTagName).arg(this->maxHoursContinuously);s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTeacherActivityTagMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher for an activity tag must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherActivityTagMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->canonicalTeachersList){
		Teacher* tch=r.internalTeachersList[i];
		int crtTeacherTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtTeacherTimetableActivityTag[d][h]=-1;
		foreach(int ai, tch->activitiesForTeacher)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtTeacherTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtTeacherTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}

		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				bool inc=false;

				if(crtTeacherTimetableActivityTag[d][h]==this->activityTagIndex)
					inc=true;
				
				if(inc)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint teacher activity tag max %1 hours continuously broken for teacher %2, activity tag %3, on day %4, length=%5.")
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalTeachersList[i]->name)
							 .arg(this->activityTagName)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint teacher activity tag max %1 hours continuously broken for teacher %2, activity tag %3, on day %4, length=%5.")
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(this->activityTagName)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	return this->activityTagName==s->name;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherActivityTagMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek(double wp, int maxnd, QString tn)
	 : TimeConstraint(wp)
{
	this->teacherName = tn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK;
}

bool ConstraintTeacherMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Teacher max days per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=", ";
	s+=tr("MD:%1", "Max days (per week)").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher must respect the maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		int t=this->teacher_ID;
		int nd[MAX_HOURS_PER_DAY + 1];
		for(int h=0; h<=r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=r.nHoursPerDay; k++){
			if(nd[k]>0){
				if(i>nd[k]){
					i-=nd[k];
					nbroken+=nd[k]*k;
				}
				else{
					nbroken+=i*k;
					break;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		//count sort
		int t=this->teacher_ID;
		int nd[MAX_HOURS_PER_DAY + 1];
		for(int h=0; h<=r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=r.nHoursPerDay; k++){
			if(nd[k]>0){
				if(i>nd[k]){
					i-=nd[k];
					nbroken+=nd[k]*k;
				}
				else{
					nbroken+=i*k;
					break;
				}
			}
		}

		if(nbroken>0){
			QString s= tr("Time constraint teacher max days per week broken for teacher: %1.")
			 .arg(r.internalTeachersList[t]->name);
			s += tr("This increases the conflicts total by %1")
			 .arg(CustomFETString::number(nbroken*weightPercentage/100));
			 
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxDaysPerWeek>r.nDaysPerWeek)
		maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxDaysPerWeek::ConstraintTeachersMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK;
}

ConstraintTeachersMaxDaysPerWeek::ConstraintTeachersMaxDaysPerWeek(double wp, int maxnd)
	 : TimeConstraint(wp)
{
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK;
}

bool ConstraintTeachersMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeachersMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Teachers max days per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MD:%1", "Max days (per week)").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		
		for(int t=0; t<r.nInternalTeachers; t++){
			int nd[MAX_HOURS_PER_DAY + 1];
			for(int h=0; h<=r.nHoursPerDay; h++)
				nd[h]=0;
			for(int d=0; d<r.nDaysPerWeek; d++){
				int nh=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
				nd[nh]++;
			}
			//return the minimum occupied days which do not respect this constraint
			int i = r.nDaysPerWeek - this->maxDaysPerWeek;
			for(int k=0; k<=r.nHoursPerDay; k++){
				if(nd[k]>0){
					if(i>nd[k]){
						i-=nd[k];
						nbroken+=nd[k]*k;
					}
					else{
						nbroken+=i*k;
						break;
					}
				}
			}
		
		}
	}
	//with logging
	else{
		nbroken=0;

		for(int t=0; t<r.nInternalTeachers; t++){
			int nbr=0;

			//count sort
			int nd[MAX_HOURS_PER_DAY + 1];
			for(int h=0; h<=r.nHoursPerDay; h++)
				nd[h]=0;
			for(int d=0; d<r.nDaysPerWeek; d++){
				int nh=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
				nd[nh]++;
			}
			//return the minimum occupied days which do not respect this constraint
			int i = r.nDaysPerWeek - this->maxDaysPerWeek;
			for(int k=0; k<=r.nHoursPerDay; k++){
				if(nd[k]>0){
					if(i>nd[k]){
						i-=nd[k];
						nbroken+=nd[k]*k;
						nbr+=nd[k]*k;
					}
					else{
						nbroken+=i*k;
						nbr+=i*k;
						break;
					}
				}
			}

			if(nbr>0){
				QString s= tr("Time constraint teachers max days per week broken for teacher: %1.")
				.arg(r.internalTeachersList[t]->name);
				s += tr("This increases the conflicts total by %1")
				.arg(CustomFETString::number(nbr*weightPercentage/100));
				
				dl.append(s);
				cl.append(nbr*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		
		}
		
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxDaysPerWeek>r.nDaysPerWeek)
		maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxGapsPerWeek::ConstraintTeachersMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK;
}

ConstraintTeachersMaxGapsPerWeek::ConstraintTeachersMaxGapsPerWeek(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
}

bool ConstraintTeachersMaxGapsPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxGapsPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintTeachersMaxGapsPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers max gaps per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MG:%1", "Max gaps (per week)").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintTeachersMaxGapsPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the maximum number of gaps per week");s+="\n";
	s+=tr("(breaks and teacher not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per week=%1").arg(this->maxGaps); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);
		
		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
	for(i=0; i<r.nInternalTeachers; i++){
		tg=0;
		for(j=0; j<r.nDaysPerWeek; j++){
			for(k=0; k<r.nHoursPerDay; k++)
				if(teachersMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
					break;
				}

			int cnt=0;
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
				if(teachersMatrix[i][j][k]>0){
					tg+=cnt;
					cnt=0;
				}
				else
					cnt++;
			}
		}
		if(tg>this->maxGaps){
			totalGaps+=tg-maxGaps;
			//assert(this->weightPercentage<100); partial solutions might break this rule
			if(conflictsString!=NULL){
				QString s=tr("Time constraint teachers max gaps per week broken for teacher: %1, conflicts factor increase=%2")
					.arg(r.internalTeachersList[i]->name)
					.arg(CustomFETString::number((tg-maxGaps)*weightPercentage/100));
					
				*conflictsString+= s+"\n";
						
				dl.append(s);
				cl.append((tg-maxGaps)*weightPercentage/100);
			}
		}
	}
	
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100){
			assert(totalGaps==0); //for partial solutions this rule might be broken
		}
			
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		maxGaps=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxGapsPerWeek::ConstraintTeacherMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK;
}

ConstraintTeacherMaxGapsPerWeek::ConstraintTeacherMaxGapsPerWeek(double wp, QString tn, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK;
	this->teacherName=tn;
	this->maxGaps=mg;
}

bool ConstraintTeacherMaxGapsPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacherIndex=r.searchTeacher(this->teacherName);
	teacherIndex=r.teachersHash.value(teacherName, -1);
	assert(this->teacherIndex>=0);
	return true;
}

bool ConstraintTeacherMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxGapsPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxGapsPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher max gaps per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName); s+=", ";
	s+=tr("MG:%1", "Max gaps (per week").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintTeacherMaxGapsPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint"); s+="\n";
	s+=tr("A teacher must respect the maximum number of gaps per week"); s+="\n";
	s+=tr("(breaks and teacher not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName); s+="\n";
	s+=tr("Maximum gaps per week=%1").arg(this->maxGaps); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
		
	i=this->teacherIndex;
	
	tg=0;
	for(j=0; j<r.nDaysPerWeek; j++){
		for(k=0; k<r.nHoursPerDay; k++)
			if(teachersMatrix[i][j][k]>0){
				assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
				break;
			}

		int cnt=0;
		for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
			if(teachersMatrix[i][j][k]>0){
				tg+=cnt;
				cnt=0;
			}
			else
				cnt++;
		}
	}
	if(tg>this->maxGaps){
		totalGaps+=tg-maxGaps;
		//assert(this->weightPercentage<100); partial solutions might break this rule
		if(conflictsString!=NULL){
			QString s=tr("Time constraint teacher max gaps per week broken for teacher: %1, conflicts factor increase=%2")
				.arg(r.internalTeachersList[i]->name)
				.arg(CustomFETString::number((tg-maxGaps)*weightPercentage/100));
					
			*conflictsString+= s+"\n";
						
			dl.append(s);
			cl.append((tg-maxGaps)*weightPercentage/100);
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMaxGapsPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		maxGaps=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxGapsPerDay::ConstraintTeachersMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY;
}

ConstraintTeachersMaxGapsPerDay::ConstraintTeachersMaxGapsPerDay(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY;
	this->maxGaps=mg;
}

bool ConstraintTeachersMaxGapsPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintTeachersMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxGapsPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMaxGapsPerDay>\n";
	return s;
}

QString ConstraintTeachersMaxGapsPerDay::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers max gaps per day");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MG:%1", "Max gaps (per day)").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintTeachersMaxGapsPerDay::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the maximum gaps per day");s+="\n";
	s+=tr("(breaks and teacher not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per day=%1").arg(this->maxGaps); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
	for(i=0; i<r.nInternalTeachers; i++){
		for(j=0; j<r.nDaysPerWeek; j++){
			tg=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(teachersMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
					break;
				}

			int cnt=0;
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
				if(teachersMatrix[i][j][k]>0){
					tg+=cnt;
					cnt=0;
				}
				else
					cnt++;
			}
			if(tg>this->maxGaps){
				totalGaps+=tg-maxGaps;
				//assert(this->weightPercentage<100); partial solutions might break this rule
				if(conflictsString!=NULL){
					QString s=tr("Time constraint teachers max gaps per day broken for teacher: %1, day: %2, conflicts factor increase=%3")
						.arg(r.internalTeachersList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(CustomFETString::number((tg-maxGaps)*weightPercentage/100));
					
					*conflictsString+= s+"\n";
								
					dl.append(s);
					cl.append((tg-maxGaps)*weightPercentage/100);
				}
			}
		}
	}
	
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{	
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxGapsPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMaxGapsPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nHoursPerDay)
		maxGaps=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxGapsPerDay::ConstraintTeacherMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY;
}

ConstraintTeacherMaxGapsPerDay::ConstraintTeacherMaxGapsPerDay(double wp, QString tn, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY;
	this->teacherName=tn;
	this->maxGaps=mg;
}

bool ConstraintTeacherMaxGapsPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacherIndex=r.searchTeacher(this->teacherName);
	teacherIndex=r.teachersHash.value(teacherName, -1);
	assert(this->teacherIndex>=0);
	return true;
}

bool ConstraintTeacherMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxGapsPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMaxGapsPerDay>\n";
	return s;
}

QString ConstraintTeacherMaxGapsPerDay::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher max gaps per day");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName); s+=", ";
	s+=tr("MG:%1", "Max gaps (per day)").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintTeacherMaxGapsPerDay::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint"); s+="\n";
	s+=tr("A teacher must respect the maximum number of gaps per day"); s+="\n";
	s+=tr("(breaks and teacher not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName); s+="\n";
	s+=tr("Maximum gaps per day=%1").arg(this->maxGaps); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
		
	i=this->teacherIndex;
	
	for(j=0; j<r.nDaysPerWeek; j++){
		tg=0;
		for(k=0; k<r.nHoursPerDay; k++)
			if(teachersMatrix[i][j][k]>0){
				assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
				break;
			}

		int cnt=0;
		for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
			if(teachersMatrix[i][j][k]>0){
				tg+=cnt;
				cnt=0;
			}
			else
				cnt++;
		}
		if(tg>this->maxGaps){
			totalGaps+=tg-maxGaps;
			//assert(this->weightPercentage<100); partial solutions might break this rule
			if(conflictsString!=NULL){
				QString s=tr("Time constraint teacher max gaps per day broken for teacher: %1, day: %2, conflicts factor increase=%3")
					.arg(r.internalTeachersList[i]->name)
					.arg(r.daysOfTheWeek[j])
					.arg(CustomFETString::number((tg-maxGaps)*weightPercentage/100));
						
				*conflictsString+= s+"\n";
							
				dl.append(s);
				cl.append((tg-maxGaps)*weightPercentage/100);
			}
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxGapsPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMaxGapsPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nHoursPerDay)
		maxGaps=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBreakTimes::ConstraintBreakTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_BREAK_TIMES;
}

ConstraintBreakTimes::ConstraintBreakTimes(double wp, QList<int> d, QList<int> h)
	: TimeConstraint(wp)
{
	this->days = d;
	this->hours = h;
	this->type = CONSTRAINT_BREAK_TIMES;
}

bool ConstraintBreakTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintBreakTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintBreakTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";

	s+="	<Number_of_Break_Times>"+CustomFETString::number(this->days.count())+"</Number_of_Break_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Break_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Break_Time>\n";
	}

	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintBreakTimes>\n";
	return s;
}

QString ConstraintBreakTimes::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Break times");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";

	s+=tr("B at:", "Break at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}
	
	return begin+s+end;
}

QString ConstraintBreakTimes::getDetailedDescription(Rules& r){
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Break times");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Break at:"); s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

bool ConstraintBreakTimes::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(r);
	
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint break times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint break times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	return true;
}

double ConstraintBreakTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	//DEPRECATED COMMENT
	//For the moment, this function sums the number of hours each teacher
	//is teaching in this break period.
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken hard restriction - we only
	//are allowed 1 weekly course for a certain teacher at a certain hour) we calculate
	//5 broken restrictions for this break period.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	
	int nbroken;
	
	nbroken=0;
		
	for(int i=0; i<r.nInternalActivities; i++){
		int dayact=c.times[i]%r.nDaysPerWeek;
		int houract=c.times[i]/r.nDaysPerWeek;
		
		assert(days.count()==hours.count());
		for(int kk=0; kk<days.count(); kk++){
			int d=days.at(kk);
			int h=hours.at(kk);
			
			int dur=r.internalActivitiesList[i].duration;
			if(d==dayact && !(houract+dur<=h || houract>h))
			{			
				nbroken++;

				if(conflictsString!=NULL){
					QString s=tr("Time constraint break not respected for activity with id %1, on day %2, hours %3")
						.arg(r.internalActivitiesList[i].id)
						.arg(r.daysOfTheWeek[dayact])
						.arg(r.daysOfTheWeek[houract]);
					s+=". ";
					s+=tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100));
					
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintBreakTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintBreakTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintBreakTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBreakTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBreakTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintBreakTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintBreakTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintBreakTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxGapsPerWeek::ConstraintStudentsMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK;
}

ConstraintStudentsMaxGapsPerWeek::ConstraintStudentsMaxGapsPerWeek(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
}

bool ConstraintStudentsMaxGapsPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxGapsPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintStudentsMaxGapsPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students max gaps per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MG:%1", "Max gaps (per week)").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintStudentsMaxGapsPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students must respect the maximum number of gaps per week");s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per week=%1").arg(this->maxGaps);s+="\n";
	
	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//returns a number equal to the number of gaps of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);
		
		c.changedForMatrixCalculation=false;
	}

	int nGaps;
	int tmp;
	int i;
	
	int tIllegalGaps=0;

	for(i=0; i<r.nInternalSubgroups; i++){
		nGaps=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					nGaps+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		}
		
		int illegalGaps=nGaps-this->maxGaps;
		if(illegalGaps<0)
			illegalGaps=0;

		if(illegalGaps>0 && conflictsString!=NULL){
			QString s=tr("Time constraint students max gaps per week broken for subgroup: %1, it has %2 extra gaps, conflicts increase=%3")
			 .arg(r.internalSubgroupsList[i]->name)
			 .arg(illegalGaps)
			 .arg(CustomFETString::number(illegalGaps*weightPercentage/100));
						 
			dl.append(s);
			cl.append(illegalGaps*weightPercentage/100);
					
			*conflictsString+= s+"\n";
		}
		
		tIllegalGaps+=illegalGaps;
	}
		
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //for partial solutions it might be broken
			assert(tIllegalGaps==0);
	return weightPercentage/100 * tIllegalGaps;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxGapsPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMaxGapsPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		maxGaps=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxGapsPerWeek::ConstraintStudentsSetMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK;
}

ConstraintStudentsSetMaxGapsPerWeek::ConstraintStudentsSetMaxGapsPerWeek(double wp, int mg, const QString& st )
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
	this->students = st;
}

bool ConstraintStudentsSetMaxGapsPerWeek::computeInternalStructure(QWidget* parent, Rules& r){
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);

	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max gaps per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Students>"; s+=protect(this->students); s+="</Students>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students set max gaps per week"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage)); s+=", ";
	s+=tr("MG:%1", "Max gaps (per week)").arg(this->maxGaps);s+=", ";
	s+=tr("St:%1", "Students").arg(this->students);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set must respect the maximum number of gaps per week");s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per week=%1").arg(this->maxGaps);s+="\n";
	s+=tr("Students=%1").arg(this->students); s+="\n";
	
	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintStudentsSetMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//OLD COMMENT
	//returns a number equal to the number of gaps of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int nGaps;
	int tmp;
	
	int tIllegalGaps=0;
	
	for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
		nGaps=0;
		int i=this->iSubgroupsList.at(sg);
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					nGaps+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		}
		
		int illegalGaps=nGaps-this->maxGaps;
		if(illegalGaps<0)
			illegalGaps=0;

		if(illegalGaps>0 && conflictsString!=NULL){
			QString s=tr("Time constraint students set max gaps per week broken for subgroup: %1, extra gaps=%2, conflicts increase=%3")
			 .arg(r.internalSubgroupsList[i]->name)
			 .arg(illegalGaps)
			 .arg(CustomFETString::number(weightPercentage/100*illegalGaps));
						 
			dl.append(s);
			cl.append(weightPercentage/100*illegalGaps);
				
			*conflictsString+= s+"\n";
		}
		
		tIllegalGaps+=illegalGaps;
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)     //for partial solutions it might be broken
			assert(tIllegalGaps==0);
	return weightPercentage/100 * tIllegalGaps;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMaxGapsPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetMaxGapsPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nDaysPerWeek*r.nHoursPerDay)
		maxGaps=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsEarlyMaxBeginningsAtSecondHour::ConstraintStudentsEarlyMaxBeginningsAtSecondHour()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
}

ConstraintStudentsEarlyMaxBeginningsAtSecondHour::ConstraintStudentsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
	this->maxBeginningsAtSecondHour=mBSH;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsEarlyMaxBeginningsAtSecondHour>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Beginnings_At_Second_Hour>"+CustomFETString::number(this->maxBeginningsAtSecondHour)+"</Max_Beginnings_At_Second_Hour>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsEarlyMaxBeginningsAtSecondHour>\n";
	return s;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students must arrive early, respecting maximum %1 arrivals at second hour")
	 .arg(this->maxBeginningsAtSecondHour);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students must begin their activities early, respecting maximum %1 later arrivals, at second hour")
	 .arg(this->maxBeginningsAtSecondHour);s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsEarlyMaxBeginningsAtSecondHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//considers the condition that the hours of subgroups begin as early as possible

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int conflTotal=0;
	
	for(int i=0; i<r.nInternalSubgroups; i++){
		int nGapsFirstHour=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			for(k=0; k<r.nHoursPerDay; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					break;
				
			bool firstHourOccupied=false;
			if(k<r.nHoursPerDay && subgroupsMatrix[i][j][k]>0)
				firstHourOccupied=true;
					
			bool dayOccupied=firstHourOccupied;
			
			bool illegalGap=false;
				
			for(k++; k<r.nHoursPerDay && !dayOccupied; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
					if(subgroupsMatrix[i][j][k]>0)
						dayOccupied=true;
					else
						illegalGap=true;
				}
				
			if(dayOccupied && illegalGap){
				if(conflictsString!=NULL){
					QString s=tr("Constraint students early max %1 beginnings at second hour broken for subgroup %2, on day %3,"
					 " because students have an illegal gap, increases conflicts total by %4")
					 .arg(this->maxBeginningsAtSecondHour)
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(CustomFETString::number(1*weightPercentage/100));
					 
					dl.append(s);
					cl.append(1*weightPercentage/100);
						
					*conflictsString+= s+"\n";
					
					conflTotal+=1;
				}
				
				if(c.nPlacedActivities==r.nInternalActivities){
					assert(0);
				}
			}
			
			if(dayOccupied && !firstHourOccupied)
				nGapsFirstHour++;
		}
		
		if(nGapsFirstHour>this->maxBeginningsAtSecondHour){
			if(conflictsString!=NULL){
				QString s=tr("Constraint students early max %1 beginnings at second hour broken for subgroup %2,"
				 " because students have too many arrivals at second hour, increases conflicts total by %3")
				 .arg(this->maxBeginningsAtSecondHour)
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg(CustomFETString::number((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100));
				 
				dl.append(s);
				cl.append((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
					
				*conflictsString+= s+"\n";
				
				conflTotal+=(nGapsFirstHour-this->maxBeginningsAtSecondHour);
			}
			
			if(c.nPlacedActivities==r.nInternalActivities){
				assert(0);
			}
		}
	}
					
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //might be broken for partial solutions
			assert(conflTotal==0);
	return weightPercentage/100 * conflTotal;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::hasWrongDayOrHour(Rules& r)
{
	if(maxBeginningsAtSecondHour>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBeginningsAtSecondHour>r.nDaysPerWeek)
		maxBeginningsAtSecondHour=r.nDaysPerWeek;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
}

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& students)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
	this->students=students;
	this->maxBeginningsAtSecondHour=mBSH;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);

	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set early is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
	return true;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Beginnings_At_Second_Hour>"+CustomFETString::number(this->maxBeginningsAtSecondHour)+"</Max_Beginnings_At_Second_Hour>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour>\n";
	return s;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;

	s+=tr("Students set must arrive early, respecting maximum %1 arrivals at second hour")
	 .arg(this->maxBeginningsAtSecondHour); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Students set").arg(this->students);

	return begin+s+end;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";

	s+=tr("A students set must begin its activities early, respecting a maximum number of later arrivals, at second hour"); s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students); s+="\n";
	s+=tr("Maximum number of arrivals at the second hour=%1").arg(this->maxBeginningsAtSecondHour);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//considers the condition that the hours of subgroups begin as early as possible

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int conflTotal=0;

	foreach(int i, this->iSubgroupsList){
		int nGapsFirstHour=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			for(k=0; k<r.nHoursPerDay; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					break;
				
			bool firstHourOccupied=false;
			if(k<r.nHoursPerDay && subgroupsMatrix[i][j][k]>0)
				firstHourOccupied=true;
					
			bool dayOccupied=firstHourOccupied;
			
			bool illegalGap=false;
				
			for(k++; k<r.nHoursPerDay && !dayOccupied; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
					if(subgroupsMatrix[i][j][k]>0)
						dayOccupied=true;
					else
						illegalGap=true;
				}
				
			if(dayOccupied && illegalGap){
				if(conflictsString!=NULL){
					QString s=tr("Constraint students set early max %1 beginnings at second hour broken for subgroup %2, on day %3,"
					 " because students have an illegal gap, increases conflicts total by %4")
					 .arg(this->maxBeginningsAtSecondHour)
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(CustomFETString::number(1*weightPercentage/100));
					 
					dl.append(s);
					cl.append(1*weightPercentage/100);
						
					*conflictsString+= s+"\n";
					
					conflTotal+=1;
				}
				
				if(c.nPlacedActivities==r.nInternalActivities)
					assert(0);
			}
			
			if(dayOccupied && !firstHourOccupied)
				nGapsFirstHour++;
		}
		
		if(nGapsFirstHour>this->maxBeginningsAtSecondHour){
			if(conflictsString!=NULL){
				QString s=tr("Constraint students set early max %1 beginnings at second hour broken for subgroup %2,"
				 " because students have too many arrivals at second hour, increases conflicts total by %3")
				 .arg(this->maxBeginningsAtSecondHour)
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg(CustomFETString::number((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100));
				 
				dl.append(s);
				cl.append((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
					
				*conflictsString+= s+"\n";
				
				conflTotal+=(nGapsFirstHour-this->maxBeginningsAtSecondHour);
			}
			
			if(c.nPlacedActivities==r.nInternalActivities)
				assert(0);
		}
	}
					
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //might be broken for partial solutions
			assert(conflTotal==0);
	return weightPercentage/100 * conflTotal;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::hasWrongDayOrHour(Rules& r)
{
	if(maxBeginningsAtSecondHour>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBeginningsAtSecondHour>r.nDaysPerWeek)
		maxBeginningsAtSecondHour=r.nDaysPerWeek;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxHoursDaily::ConstraintStudentsMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsMaxHoursDaily::ConstraintStudentsMaxHoursDaily(double wp, int maxnh)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_DAILY;
}

bool ConstraintStudentsMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	if(this->maxHoursDaily>=0)
		s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	else
		assert(0);
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsMaxHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students max hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MH:%1", "Max hours (daily)").arg(this->maxHoursDaily);

	return begin+s+end;
}

QString ConstraintStudentsMaxHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;
	
	assert(this->maxHoursDaily>=0);

	if(1){
		too_much=0;
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//OLD COMMENT
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than maxHoursDaily hours per day.
					too_much += 1; //tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=tr("Time constraint students max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflict increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(CustomFETString::number(tmp))
						 .arg(CustomFETString::number(weightPercentage/100*1));
						 
						dl.append(s);
						cl.append(weightPercentage/100*1);
					
						*conflictsString+= s+"\n";
					}
				}
			}
	}

	assert(too_much>=0);
	if(weightPercentage==100)
		assert(too_much==0);
	return too_much * weightPercentage/100;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxHoursDaily::ConstraintStudentsSetMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsSetMaxHoursDaily::ConstraintStudentsSetMaxHoursDaily(double wp, int maxnh, QString s)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY;
}

bool ConstraintStudentsSetMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetMaxHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students set max hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Students (set)").arg(this->students); s+=", ";
	s+=tr("MH:%1", "Max hours (daily)").arg(this->maxHoursDaily);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintStudentsSetMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max hours daily is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;

	assert(this->maxHoursDaily>=0);

	if(1){
		too_much=0;
		for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
			int i=iSubgroupsList.at(sg);
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					too_much += 1; //tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=tr("Time constraint students set max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflicts increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(CustomFETString::number(tmp))
						 .arg(CustomFETString::number( 1 *weightPercentage/100));
						 
						dl.append(s);
						cl.append( 1 *weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}
	
	assert(too_much>=0);
	if(weightPercentage==100)
		assert(too_much==0);
	return too_much * weightPercentage / 100.0;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxHoursContinuously::ConstraintStudentsMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsMaxHoursContinuously::ConstraintStudentsMaxHoursContinuously(double wp, int maxnh)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxHoursContinuously::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	if(this->maxHoursContinuously>=0)
		s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	else
		assert(0);
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsMaxHoursContinuously::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students max hours continuously");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MH:%1", "Max hours (continuously)").arg(this->maxHoursContinuously);

	return begin+s+end;
}

QString ConstraintStudentsMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int nbroken;

	nbroken=0;
	for(int i=0; i<r.nInternalSubgroups; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint students max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxHoursContinuously::ConstraintStudentsSetMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsSetMaxHoursContinuously::ConstraintStudentsSetMaxHoursContinuously(double wp, int maxnh, QString s)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsSetMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxHoursContinuously::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsSetMaxHoursContinuously::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students set max hours continuously");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Students (set)").arg(this->students);s+=", ";
	s+=tr("MH:%1", "Max hours (continuously)").arg(this->maxHoursContinuously);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set must respect the maximum number of hours continuously");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintStudentsSetMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max hours continuously is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->iSubgroupsList){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsActivityTagMaxHoursContinuously::ConstraintStudentsActivityTagMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsActivityTagMaxHoursContinuously::ConstraintStudentsActivityTagMaxHoursContinuously(double wp, int maxnh, const QString& activityTag)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->activityTagName=activityTag;
	this->type = CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);
	
	this->canonicalSubgroupsList.clear();
	for(int i=0; i<r.nInternalSubgroups; i++){
		bool found=false;
	
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		foreach(int actIndex, sbg->activitiesForSubgroup){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalSubgroupsList.append(i);
	}

	return true;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsActivityTagMaxHoursContinuously::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsActivityTagMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	
	if(this->maxHoursContinuously>=0)
		s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	else
		assert(0);
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsActivityTagMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsActivityTagMaxHoursContinuously::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students for activity tag %1 have max %2 hours continuously")
		.arg(this->activityTagName).arg(this->maxHoursContinuously); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintStudentsActivityTagMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students, for an activity tag, must respect the maximum number of hours continuously"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsActivityTagMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);
		
		c.changedForMatrixCalculation=false;
	}
	
	int nbroken;

	nbroken=0;
	
	foreach(int i, this->canonicalSubgroupsList){
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		int crtSubgroupTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtSubgroupTimetableActivityTag[d][h]=-1;
		foreach(int ai, sbg->activitiesForSubgroup)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtSubgroupTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtSubgroupTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}

		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				bool inc=false;
				
				if(crtSubgroupTimetableActivityTag[d][h]==this->activityTagIndex)
					inc=true;
				
				if(inc)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint students, activity tag %1, max %2 hours continuously, broken for subgroup %3, on day %4, length=%5.")
							 .arg(this->activityTagName)
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students, activity tag %1, max %2 hours continuously, broken for subgroup %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	return s->name==this->activityTagName;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsActivityTagMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetActivityTagMaxHoursContinuously::ConstraintStudentsSetActivityTagMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsSetActivityTagMaxHoursContinuously::ConstraintStudentsSetActivityTagMaxHoursContinuously(double wp, int maxnh, const QString& s, const QString& activityTag)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->students = s;
	this->activityTagName=activityTag;
	this->type = CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetActivityTagMaxHoursContinuously::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetActivityTagMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Continuously>"+CustomFETString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetActivityTagMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsSetActivityTagMaxHoursContinuously::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Students set %1 for activity tag %2 has max %3 hours continuously").arg(this->students).arg(this->activityTagName).arg(this->maxHoursContinuously);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintStudentsSetActivityTagMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set, for an activity tag, must respect the maximum number of hours continuously"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);

	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max hours continuously is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	/////////////
	this->canonicalSubgroupsList.clear();
	foreach(int i, this->iSubgroupsList){
		bool found=false;
	
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		foreach(int actIndex, sbg->activitiesForSubgroup){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalSubgroupsList.append(i);
	}

		
	return true;
}

double ConstraintStudentsSetActivityTagMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;

	foreach(int i, this->canonicalSubgroupsList){
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		int crtSubgroupTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtSubgroupTimetableActivityTag[d][h]=-1;
		foreach(int ai, sbg->activitiesForSubgroup)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtSubgroupTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtSubgroupTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}

		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				bool inc=false;
				
				if(crtSubgroupTimetableActivityTag[d][h]==this->activityTagIndex)
					inc=true;
			
				if(inc)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(tr(
							 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
							 .arg(CustomFETString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursContinuously>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetActivityTagMaxHoursContinuously::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursContinuously>r.nHoursPerDay)
		maxHoursContinuously=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinHoursDaily::ConstraintStudentsMinHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MIN_HOURS_DAILY;
	this->minHoursDaily = -1;
	
	this->allowEmptyDays=false;
}

ConstraintStudentsMinHoursDaily::ConstraintStudentsMinHoursDaily(double wp, int minnh, bool _allowEmptyDays)
	: TimeConstraint(wp)
{
	this->minHoursDaily = minnh;
	this->type = CONSTRAINT_STUDENTS_MIN_HOURS_DAILY;
	
	this->allowEmptyDays=_allowEmptyDays;
}

bool ConstraintStudentsMinHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMinHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	if(this->minHoursDaily>=0)
		s+="	<Minimum_Hours_Daily>"+CustomFETString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	else
		assert(0);
	if(this->allowEmptyDays)
		s+="	<Allow_Empty_Days>true</Allow_Empty_Days>\n";
	else
		s+="	<Allow_Empty_Days>false</Allow_Empty_Days>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMinHoursDaily>\n";
	return s;
}

QString ConstraintStudentsMinHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;

	if(this->allowEmptyDays)
		s+="! ";
	s+=tr("Students min hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("mH:%1", "Min hours (daily)").arg(this->minHoursDaily);s+=", ";
	s+=tr("AED:%1", "Allow empty days").arg(yesNoTranslated(this->allowEmptyDays));

	return begin+s+end;
}

QString ConstraintStudentsMinHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	if(this->allowEmptyDays==true){
		s+=tr("(nonstandard, students may have empty days)");
		s+="\n";
	}
	s+=tr("All students must respect the minimum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Minimum hours daily=%1").arg(this->minHoursDaily);s+="\n";
	s+=tr("Allow empty days=%1").arg(yesNoTranslated(this->allowEmptyDays));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_little;
	
	assert(this->minHoursDaily>=0);

	too_little=0;
	for(int i=0; i<r.nInternalSubgroups; i++)
		for(int j=0; j<r.nDaysPerWeek; j++){
			tmp=0;
			for(int k=0; k<r.nHoursPerDay; k++){
				if(subgroupsMatrix[i][j][k]>=1)
					tmp++;
			}
			
			bool searchDay;
			if(this->allowEmptyDays==true)
				searchDay=(tmp>0);
			else
				searchDay=true;
			
			if(/*tmp>0*/ searchDay && this->minHoursDaily>=0 && tmp < this->minHoursDaily){ //we would like no less than minHoursDaily hours per day.
				too_little += - tmp + this->minHoursDaily;

				if(conflictsString!=NULL){
					QString s=tr("Time constraint students min hours daily broken for subgroup: %1, day: %2, lenght=%3, conflict increase=%4")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(CustomFETString::number(tmp))
					 .arg(CustomFETString::number(weightPercentage/100*(-tmp+this->minHoursDaily)));
						 
					dl.append(s);
					cl.append(weightPercentage/100*(-tmp+this->minHoursDaily));
					
					*conflictsString+= s+"\n";
				}
			}
		}

	//should not consider for empty days
	
	assert(too_little>=0);

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100) //does not work for partial solutions
			assert(too_little==0);

	return too_little * weightPercentage/100;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMinHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(minHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsMinHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMinHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minHoursDaily>r.nHoursPerDay)
		minHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinHoursDaily::ConstraintStudentsSetMinHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY;
	this->minHoursDaily = -1;
	
	this->allowEmptyDays=false;
}

ConstraintStudentsSetMinHoursDaily::ConstraintStudentsSetMinHoursDaily(double wp, int minnh, QString s, bool _allowEmptyDays)
	: TimeConstraint(wp)
{
	this->minHoursDaily = minnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY;
	
	this->allowEmptyDays=_allowEmptyDays;
}

bool ConstraintStudentsSetMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMinHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Minimum_Hours_Daily>"+CustomFETString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	if(this->allowEmptyDays)
		s+="	<Allow_Empty_Days>true</Allow_Empty_Days>\n";
	else
		s+="	<Allow_Empty_Days>false</Allow_Empty_Days>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMinHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetMinHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	if(this->allowEmptyDays)
		s+="! ";
	s+=tr("Students set min hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Students (set)").arg(this->students);s+=", ";
	s+=tr("mH:%1", "Min hours (daily)").arg(this->minHoursDaily);s+=", ";
	s+=tr("AED:%1", "Allow empty days").arg(yesNoTranslated(this->allowEmptyDays));

	return begin+s+end;
}

QString ConstraintStudentsSetMinHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	if(this->allowEmptyDays==true){
		s+=tr("(nonstandard, students may have empty days)");
		s+="\n";
	}
	s+=tr("A students set must respect the minimum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Minimum hours daily=%1").arg(this->minHoursDaily);s+="\n";
	s+=tr("Allow empty days=%1").arg(yesNoTranslated(this->allowEmptyDays));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintStudentsSetMinHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set min hours daily is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_little;
	
	assert(this->minHoursDaily>=0);

	too_little=0;
	for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
		int i=iSubgroupsList.at(sg);
		for(int j=0; j<r.nDaysPerWeek; j++){
			tmp=0;
			for(int k=0; k<r.nHoursPerDay; k++){
				if(subgroupsMatrix[i][j][k]>=1)
					tmp++;
			}
			
			bool searchDay;
			if(this->allowEmptyDays==true)
				searchDay=(tmp>0);
			else
				searchDay=true;
			
			if(/*tmp>0*/ searchDay && this->minHoursDaily>=0 && tmp < this->minHoursDaily){
				too_little += - tmp + this->minHoursDaily;

				if(conflictsString!=NULL){
					QString s=tr("Time constraint students set min hours daily broken for subgroup: %1, day: %2, lenght=%3, conflicts increase=%4")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(CustomFETString::number(tmp))
					 .arg(CustomFETString::number((-tmp+this->minHoursDaily)*weightPercentage/100));
						 
					dl.append(s);
					cl.append((-tmp+this->minHoursDaily)*weightPercentage/100);
					
					*conflictsString+= s+"\n";
				}
			}
		}
	}
	
	assert(too_little>=0);

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100) //does not work for partial solutions
			assert(too_little==0);

	return too_little * weightPercentage / 100.0;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMinHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(minHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsSetMinHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetMinHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minHoursDaily>r.nHoursPerDay)
		minHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredStartingTime::ConstraintActivityPreferredStartingTime()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME;
}

ConstraintActivityPreferredStartingTime::ConstraintActivityPreferredStartingTime(double wp, int actId, int d, int h, bool perm)
	: TimeConstraint(wp)
{
	this->activityId = actId;
	this->day = d;
	this->hour = h;
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME;
	this->permanentlyLocked=perm;
}

bool ConstraintActivityPreferredStartingTime::operator==(ConstraintActivityPreferredStartingTime& c){
	if(this->day!=c.day)
		return false;
	if(this->hour!=c.hour)
		return false;
	if(this->activityId!=c.activityId)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	if(this->active!=c.active)
		return false;
	//no need to care about permanently locked
	return true;
}

bool ConstraintActivityPreferredStartingTime::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(activityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because it refers to invalid activity id). Please correct it (maybe removing it is a solution):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	if(this->day >= r.nDaysPerWeek){
		TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
		 tr("Constraint activity preferred starting time is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->hour == r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
		 tr("Constraint activity preferred starting time is wrong because preferred hour is too late (after the last acceptable slot). Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->hour > r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
		 tr("Constraint activity preferred starting time is wrong because it refers to removed hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}

	this->activityIndex=i;
	return true;
}

bool ConstraintActivityPreferredStartingTime::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredStartingTime::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintActivityPreferredStartingTime>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	if(this->day>=0)
		s+="	<Preferred_Day>"+protect(r.daysOfTheWeek[this->day])+"</Preferred_Day>\n";
	if(this->hour>=0)
		s+="	<Preferred_Hour>"+protect(r.hoursOfTheDay[this->hour])+"</Preferred_Hour>\n";
	s+="	<Permanently_Locked>";s+=trueFalse(this->permanentlyLocked);s+="</Permanently_Locked>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityPreferredStartingTime>\n";
	return s;
}

QString ConstraintActivityPreferredStartingTime::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Act. id: %1 (%2) has a preferred starting time: %3", "%1 is the id, %2 is the detailed description of the activity. %3 is time (day and hour)")
	 .arg(this->activityId)
	 .arg(getActivityDetailedDescription(r, this->activityId))
	 .arg(r.daysOfTheWeek[this->day]+" "+r.hoursOfTheDay[this->hour]);

	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));
	s+=", ";
	s+=tr("PL:%1", "Abbreviation for permanently locked").arg(yesNoTranslated(this->permanentlyLocked));

	return begin+s+end;
}

QString ConstraintActivityPreferredStartingTime::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+="\n";

	s+=tr("has a preferred starting time:");
	s+="\n";
	s+=tr("Day=%1").arg(r.daysOfTheWeek[this->day]);
	s+="\n";
	s+=tr("Hour=%1").arg(r.hoursOfTheDay[this->hour]);
	s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	if(this->permanentlyLocked){
		s+=tr("This activity is permanently locked, which means you cannot unlock it from the 'Timetable' menu"
		" (you can unlock this activity by removing the constraint from the constraints dialog or by setting the 'permanently"
		" locked' attribute false when editing this constraint)");
	}
	else{
		s+=tr("This activity is not permanently locked, which means you can unlock it from the 'Timetable' menu");
	}
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredStartingTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		if(this->day>=0)
			nbroken+=abs(this->day-d);
		if(this->hour>=0)
			nbroken+=abs(this->hour-h);
	}
	if(nbroken>0)
		nbroken=1;

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint activity preferred starting time broken for activity with id=%1 (%2), increases conflicts total by %3",
			"%1 is the id, %2 is the detailed description of the activity")
			.arg(this->activityId)
			.arg(getActivityDetailedDescription(r, this->activityId))
			.arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivityPreferredStartingTime::hasWrongDayOrHour(Rules& r)
{
	if(day<0 || day>=r.nDaysPerWeek || hour<0 || hour>=r.nHoursPerDay)
		return true;
	return false;
}

bool ConstraintActivityPreferredStartingTime::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return false;
}

bool ConstraintActivityPreferredStartingTime::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredTimeSlots::ConstraintActivityPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS;
}

ConstraintActivityPreferredTimeSlots::ConstraintActivityPreferredTimeSlots(double wp, int actId, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->p_activityId=actId;
	this->p_nPreferredTimeSlots_L=nPT_L;
	this->p_days_L=d_L;
	this->p_hours_L=h_L;
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS;
}

bool ConstraintActivityPreferredTimeSlots::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->p_activityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(p_activityId, r.nInternalActivities);

	if(i==r.nInternalActivities){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because it refers to invalid activity id). Please correct it (maybe removing it is a solution):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	for(int k=0; k<p_nPreferredTimeSlots_L; k++){
		if(this->p_days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->p_hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}

		if(this->p_hours_L[k]<0 || this->p_days_L[k]<0){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred time slots is wrong because it has hour or day not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	this->p_activityIndex=i;
	return true;
}

bool ConstraintActivityPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->p_activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredTimeSlots::getXmlDescription(Rules& r)
{
	QString s="<ConstraintActivityPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->p_activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+CustomFETString::number(this->p_nPreferredTimeSlots_L)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots_L; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days_L[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days_L[i]])+"</Preferred_Day>\n";
		if(this->p_hours_L[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours_L[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityPreferredTimeSlots>\n";
	return s;
}

QString ConstraintActivityPreferredTimeSlots::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Act. id: %1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		.arg(this->p_activityId)
		.arg(getActivityDetailedDescription(r, this->p_activityId));
	s+=" ";

	s+=tr("has a set of preferred time slots:");
	s+=" ";
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+="; ";
	}
	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivityPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		.arg(this->p_activityId)
		.arg(getActivityDetailedDescription(r, this->p_activityId));
	s+="\n";

	s+=tr("has a set of preferred time slots (all hours of the activity must be in the allowed slots):");
	s+="\n";
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);
	
	Matrix2D<bool> allowed;
	allowed.resize(r.nDaysPerWeek, r.nHoursPerDay);
	//bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0 && this->p_hours_L[i]>=0)
			allowed[this->p_days_L[i]][this->p_hours_L[i]]=true;
		else
			assert(0);
	}

	nbroken=0;
	if(c.times[this->p_activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->p_activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->p_activityIndex]/r.nDaysPerWeek; //the hour
		for(int dur=0; dur<r.internalActivitiesList[this->p_activityIndex].duration; dur++)
			if(!allowed[d][h+dur])
				nbroken++;
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint activity preferred time slots broken for activity with id=%1 (%2) on %3 hours, increases conflicts total by %4",
		 "%1 is the id, %2 is the detailed description of the activity.")
		 .arg(this->p_activityId)
		 .arg(getActivityDetailedDescription(r, this->p_activityId))
		 .arg(nbroken)
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));
		 
		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->p_activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredTimeSlots::hasWrongDayOrHour(Rules& r)
{
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)<0 || p_days_L.at(i)>=r.nDaysPerWeek
		 || p_hours_L.at(i)<0 || p_hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintActivityPreferredTimeSlots::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivityPreferredTimeSlots::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)>=0 && p_days_L.at(i)<r.nDaysPerWeek
		 && p_hours_L.at(i)>=0 && p_hours_L.at(i)<r.nHoursPerDay){
			newDays.append(p_days_L.at(i));
			newHours.append(p_hours_L.at(i));
			newNPref++;
		}
	
	p_nPreferredTimeSlots_L=newNPref;
	p_days_L=newDays;
	p_hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesPreferredTimeSlots::ConstraintActivitiesPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS;
}

ConstraintActivitiesPreferredTimeSlots::ConstraintActivitiesPreferredTimeSlots(double wp, QString te,
	QString st, QString su, QString sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(dur==-1 || dur>=1);
	duration=dur;

	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->p_teacherName=te;
	this->p_subjectName=su;
	this->p_activityTagName=sut;
	this->p_studentsName=st;
	this->p_nPreferredTimeSlots_L=nPT_L;
	this->p_days_L=d_L;
	this->p_hours_L=h_L;
	this->type=CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS;
}

bool ConstraintActivitiesPreferredTimeSlots::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->p_nActivities=0;
	this->p_activitiesIndices.clear();

	int it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.indexOf(this->p_teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.augmentedSetsShareStudentsFaster(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
			continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && !act->activityTagsNames.contains(this->p_activityTagName)){
			continue;
		}

		if(duration>=1 && act->duration!=duration)
			continue;
	
		assert(this->p_nActivities < MAX_ACTIVITIES);
		this->p_nActivities++;
		this->p_activitiesIndices.append(i);
	}
	
	assert(this->p_nActivities==this->p_activitiesIndices.count());

	//////////////////////	
	for(int k=0; k<p_nPreferredTimeSlots_L; k++){
		if(this->p_days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k]<0 || this->p_days_L[k]<0){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred time slots is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->p_nActivities>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	QList<int> localActiveActs;
	QList<int> localAllActs;

	//returns true if all activities are inactive
	int it;
	Activity* act;
	int i;
	for(i=0; i<r.activitiesList.count(); i++){
		act=r.activitiesList.at(i);

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.indexOf(this->p_teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.setsShareStudents(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && !act->activityTagsNames.contains(this->p_activityTagName)){
				continue;
		}

		if(duration>=1 && act->duration!=duration)
			continue;
	
		if(!r.inactiveActivities.contains(act->id))
			localActiveActs.append(act->id);
			
		localAllActs.append(act->id);
	}

	if(localActiveActs.count()==0 && localAllActs.count()>0)
	//because if this constraint does not refer to any activity,
	//it should be reported as incorrect
		return true;
	else
		return false;
}

QString ConstraintActivitiesPreferredTimeSlots::getXmlDescription(Rules& r)
{
	QString s="<ConstraintActivitiesPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->p_teacherName)+"</Teacher_Name>\n";
	s+="	<Students_Name>"+protect(this->p_studentsName)+"</Students_Name>\n";
	s+="	<Subject_Name>"+protect(this->p_subjectName)+"</Subject_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->p_activityTagName)+"</Activity_Tag_Name>\n";
	if(duration>=1)
		s+="	<Duration>"+CustomFETString::number(duration)+"</Duration>\n";
	else
		s+="	<Duration></Duration>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+CustomFETString::number(this->p_nPreferredTimeSlots_L)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots_L; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days_L[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days_L[i]])+"</Preferred_Day>\n";
		if(this->p_hours_L[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours_L[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesPreferredTimeSlots>\n";
	return s;
}

QString ConstraintActivitiesPreferredTimeSlots::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;

	QString tc, st, su, at, dur;
	
	if(this->p_teacherName!="")
		tc=tr("teacher=%1").arg(this->p_teacherName);
	else
		tc=tr("all teachers");
		
	if(this->p_studentsName!="")
		st=tr("students=%1").arg(this->p_studentsName);
	else
		st=tr("all students");
		
	if(this->p_subjectName!="")
		su=tr("subject=%1").arg(this->p_subjectName);
	else
		su=tr("all subjects");
		
	if(this->p_activityTagName!="")
		at=tr("activity tag=%1").arg(this->p_activityTagName);
	else
		at=tr("all activity tags");
	
	if(duration>=1)
		dur=tr("duration=%1").arg(duration);
	else
		dur=tr("all durations");

	s+=tr("Activities with %1, %2, %3, %4, %5, have a set of preferred time slots:", "%1...%5 are conditions for the activities").arg(tc).arg(st).arg(su).arg(at).arg(dur);
	s+=" ";
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+="; ";
	}
	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivitiesPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activities with:");s+="\n";

	if(this->p_teacherName!="")
		s+=tr("Teacher=%1").arg(this->p_teacherName);
	else
		s+=tr("All teachers");
	s+="\n";
	if(this->p_studentsName!="")
		s+=tr("Students=%1").arg(this->p_studentsName);
	else
		s+=tr("All students");
	s+="\n";
	if(this->p_subjectName!="")
		s+=tr("Subject=%1").arg(this->p_subjectName);
	else
		s+=tr("All subjects");
	s+="\n";
	if(this->p_activityTagName!="")
		s+=tr("Activity tag=%1").arg(this->p_activityTagName);
	else
		s+=tr("All activity tags");
	s+="\n";

	if(duration>=1)
		s+=tr("Duration=%1").arg(duration);
	else
		s+=tr("All durations");
	s+="\n";

	s+=tr("have a set of preferred time slots (all hours of each affected activity must be in the allowed slots):");
	s+="\n";
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

///////////////////
	Matrix2D<bool> allowed;
	allowed.resize(r.nDaysPerWeek, r.nHoursPerDay);
	//bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0 && this->p_hours_L[i]>=0)
			allowed[this->p_days_L[i]][this->p_hours_L[i]]=true;
		else
			assert(0);
	}
////////////////////

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->p_nActivities; i++){
		tmp=0;
		int ai=this->p_activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++)
				if(!allowed[d][h+dur])
					tmp++;
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=tr("Time constraint activities preferred time slots broken"
			 " for activity with id=%1 (%2) on %3 hours,"
			 " increases conflicts total by %4", "%1 is the id, %2 is the detailed description of the activity.")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(getActivityDetailedDescription(r, r.internalActivitiesList[ai].id))
			 .arg(tmp)
			 .arg(CustomFETString::number(weightPercentage/100*tmp));
				 
			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	int it;

	//check if this activity has the corresponding teacher
	if(this->p_teacherName!=""){
		it = a->teachersNames.indexOf(this->p_teacherName);
		if(it==-1)
			return false;
	}
	//check if this activity has the corresponding students
	if(this->p_studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.setsShareStudents(st, this->p_studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->p_subjectName!="" && a->subjectName!=this->p_subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->p_activityTagName!="" && !a->activityTagsNames.contains(this->p_activityTagName))
		return false;

	if(duration>=1 && a->duration!=duration)
		return false;

	return true;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::hasWrongDayOrHour(Rules& r)
{
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)<0 || p_days_L.at(i)>=r.nDaysPerWeek
		 || p_hours_L.at(i)<0 || p_hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivitiesPreferredTimeSlots::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)>=0 && p_days_L.at(i)<r.nDaysPerWeek
		 && p_hours_L.at(i)>=0 && p_hours_L.at(i)<r.nHoursPerDay){
			newDays.append(p_days_L.at(i));
			newHours.append(p_hours_L.at(i));
			newNPref++;
		}
	
	p_nPreferredTimeSlots_L=newNPref;
	p_days_L=newDays;
	p_hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubactivitiesPreferredTimeSlots::ConstraintSubactivitiesPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS;
}

ConstraintSubactivitiesPreferredTimeSlots::ConstraintSubactivitiesPreferredTimeSlots(double wp, int compNo, QString te,
	QString st, QString su, QString sut, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->componentNumber=compNo;
	this->p_teacherName=te;
	this->p_subjectName=su;
	this->p_activityTagName=sut;
	this->p_studentsName=st;
	this->p_nPreferredTimeSlots_L=nPT_L;
	this->p_days_L=d_L;
	this->p_hours_L=h_L;
	this->type=CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS;
}

bool ConstraintSubactivitiesPreferredTimeSlots::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->p_nActivities=0;
	this->p_activitiesIndices.clear();

	int it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		
		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.indexOf(this->p_teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.augmentedSetsShareStudentsFaster(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
			continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && !act->activityTagsNames.contains(this->p_activityTagName)){
			continue;
		}
	
		assert(this->p_nActivities < MAX_ACTIVITIES);	
		this->p_nActivities++;
		this->p_activitiesIndices.append(i);
	}

	assert(this->p_nActivities==this->p_activitiesIndices.count());

	//////////////////////	
	for(int k=0; k<p_nPreferredTimeSlots_L; k++){
		if(this->p_days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours_L[k]<0 || this->p_days_L[k]<0){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred time slots is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->p_nActivities>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintSubactivitiesPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	QList<int> localActiveActs;
	QList<int> localAllActs;

	//returns true if all activities are inactive
	int it;
	Activity* act;
	int i;
	for(i=0; i<r.activitiesList.count(); i++){
		act=r.activitiesList.at(i);

		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.indexOf(this->p_teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.setsShareStudents(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && !act->activityTagsNames.contains(this->p_activityTagName)){
				continue;
		}
	
		if(!r.inactiveActivities.contains(act->id))
			localActiveActs.append(act->id);
			
		localAllActs.append(act->id);
	}

	if(localActiveActs.count()==0 && localAllActs.count()>0)
		return true;
	else
		return false;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getXmlDescription(Rules& r)
{
	QString s="<ConstraintSubactivitiesPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Component_Number>"+CustomFETString::number(this->componentNumber)+"</Component_Number>\n";
	s+="	<Teacher_Name>"+protect(this->p_teacherName)+"</Teacher_Name>\n";
	s+="	<Students_Name>"+protect(this->p_studentsName)+"</Students_Name>\n";
	s+="	<Subject_Name>"+protect(this->p_subjectName)+"</Subject_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->p_activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+CustomFETString::number(this->p_nPreferredTimeSlots_L)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots_L; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days_L[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days_L[i]])+"</Preferred_Day>\n";
		if(this->p_hours_L[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours_L[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubactivitiesPreferredTimeSlots>\n";
	return s;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	QString tc, st, su, at;
	
	if(this->p_teacherName!="")
		tc=tr("teacher=%1").arg(this->p_teacherName);
	else
		tc=tr("all teachers");
		
	if(this->p_studentsName!="")
		st=tr("students=%1").arg(this->p_studentsName);
	else
		st=tr("all students");
		
	if(this->p_subjectName!="")
		su=tr("subject=%1").arg(this->p_subjectName);
	else
		su=tr("all subjects");
		
	if(this->p_activityTagName!="")
		at=tr("activity tag=%1").arg(this->p_activityTagName);
	else
		at=tr("all activity tags");
	
	s+=tr("Subactivities with %1, %2, %3, %4, %5, have a set of preferred time slots:", "%1...%5 are conditions for the subactivities")
		.arg(tr("component number=%1").arg(this->componentNumber)).arg(tc).arg(st).arg(su).arg(at);
		
	s+=" ";
	
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+="; ";
	}
	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Subactivities with:");s+="\n";
	
	s+=tr("Component number=%1").arg(this->componentNumber);
	s+="\n";

	if(this->p_teacherName!="")
		s+=tr("Teacher=%1").arg(this->p_teacherName);
	else
		s+=tr("All teachers");
	s+="\n";
		
	if(this->p_studentsName!="")
		s+=tr("Students=%1").arg(this->p_studentsName);
	else
		s+=tr("All students");
	s+="\n";
	
	if(this->p_subjectName!="")
		s+=tr("Subject=%1").arg(this->p_subjectName);
	else
		s+=tr("All subjects");
	s+="\n";
	
	if(this->p_activityTagName!="")
		s+=tr("Activity tag=%1").arg(this->p_activityTagName);
	else
		s+=tr("All activity tags");
	s+="\n";

	s+=tr("have a set of preferred time slots (all hours of each affected subactivity must be in the allowed slots):");
	s+="\n";
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0){
			s+=r.daysOfTheWeek[this->p_days_L[i]];
			s+=" ";
		}
		if(this->p_hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours_L[i]];
		}
		if(i<this->p_nPreferredTimeSlots_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubactivitiesPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

///////////////////
	Matrix2D<bool> allowed;
	allowed.resize(r.nDaysPerWeek, r.nHoursPerDay);
	//bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots_L; i++){
		if(this->p_days_L[i]>=0 && this->p_hours_L[i]>=0)
			allowed[this->p_days_L[i]][this->p_hours_L[i]]=true;
		else
			assert(0);
	}
////////////////////

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->p_nActivities; i++){
		tmp=0;
		int ai=this->p_activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++)
				if(!allowed[d][h+dur])
					tmp++;
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=tr("Time constraint subactivities preferred time slots broken"
			 " for activity with id=%1 (%2), component number %3, on %4 hours,"
			 " increases conflicts total by %5", "%1 is the id, %2 is the detailed description of the activity.")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(getActivityDetailedDescription(r, r.internalActivitiesList[ai].id))
			 .arg(componentNumber)
			 .arg(tmp)
			 .arg(CustomFETString::number(weightPercentage/100*tmp));

			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	if(!a->representsComponentNumber(this->componentNumber))
		return false;

	int it;

	//check if this activity has the corresponding teacher
	if(this->p_teacherName!=""){
		it = a->teachersNames.indexOf(this->p_teacherName);
		if(it==-1)
			return false;
	}
	//check if this activity has the corresponding students
	if(this->p_studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.setsShareStudents(st, this->p_studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->p_subjectName!="" && a->subjectName!=this->p_subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->p_activityTagName!="" && !a->activityTagsNames.contains(this->p_activityTagName))
		return false;

	return true;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::hasWrongDayOrHour(Rules& r)
{
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)<0 || p_days_L.at(i)>=r.nDaysPerWeek
		 || p_hours_L.at(i)<0 || p_hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintSubactivitiesPreferredTimeSlots::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(p_nPreferredTimeSlots_L==p_days_L.count());
	assert(p_nPreferredTimeSlots_L==p_hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<p_nPreferredTimeSlots_L; i++)
		if(p_days_L.at(i)>=0 && p_days_L.at(i)<r.nDaysPerWeek
		 && p_hours_L.at(i)>=0 && p_hours_L.at(i)<r.nHoursPerDay){
			newDays.append(p_days_L.at(i));
			newHours.append(p_hours_L.at(i));
			newNPref++;
		}
	
	p_nPreferredTimeSlots_L=newNPref;
	p_days_L=newDays;
	p_hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredStartingTimes::ConstraintActivityPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES;
}

ConstraintActivityPreferredStartingTimes::ConstraintActivityPreferredStartingTimes(double wp, int actId, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->activityId=actId;
	this->nPreferredStartingTimes_L=nPT_L;
	this->days_L=d_L;
	this->hours_L=h_L;
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES;
}

bool ConstraintActivityPreferredStartingTimes::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(activityId, r.nInternalActivities);

	if(i==r.nInternalActivities){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because it refers to invalid activity id). Please correct it (maybe removing it is a solution):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	for(int k=0; k<nPreferredStartingTimes_L; k++){
		if(this->days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activity preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	this->activityIndex=i;	
	return true;
}

bool ConstraintActivityPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredStartingTimes::getXmlDescription(Rules& r)
{
	QString s="<ConstraintActivityPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+CustomFETString::number(this->nPreferredStartingTimes_L)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes_L; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days_L[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days_L[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours_L[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours_L[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityPreferredStartingTimes>\n";
	return s;
}

QString ConstraintActivityPreferredStartingTimes::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Act. id: %1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	
	s+=" ";
	s+=tr("has a set of preferred starting times:");
	s+=" ";
	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<nPreferredStartingTimes_L-1)
			s+="; ";
	}
	s+=", ";

	s+=tr("WP:%1%", "Weight Percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivityPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	
	s+="\n";
	s+=tr("has a set of preferred starting times:");
	s+="\n";
	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<this->nPreferredStartingTimes_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		int i;
		for(i=0; i<this->nPreferredStartingTimes_L; i++){
			if(this->days_L[i]>=0 && this->days_L[i]!=d)
				continue;
			if(this->hours_L[i]>=0 && this->hours_L[i]!=h)
				continue;
			break;
		}
		if(i==this->nPreferredStartingTimes_L){
			nbroken=1;
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint activity preferred starting times broken for activity with id=%1 (%2), increases conflicts total by %3",
		 "%1 is the id, %2 is the detailed description of the activity")
		 .arg(this->activityId)
		 .arg(getActivityDetailedDescription(r, this->activityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivityPreferredStartingTimes::hasWrongDayOrHour(Rules& r)
{
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)<0 || days_L.at(i)>=r.nDaysPerWeek
		 || hours_L.at(i)<0 || hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintActivityPreferredStartingTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivityPreferredStartingTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)>=0 && days_L.at(i)<r.nDaysPerWeek
		 && hours_L.at(i)>=0 && hours_L.at(i)<r.nHoursPerDay){
			newDays.append(days_L.at(i));
			newHours.append(hours_L.at(i));
			newNPref++;
		}
	
	nPreferredStartingTimes_L=newNPref;
	days_L=newDays;
	hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesPreferredStartingTimes::ConstraintActivitiesPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES;
}

ConstraintActivitiesPreferredStartingTimes::ConstraintActivitiesPreferredStartingTimes(double wp, QString te,
	QString st, QString su, QString sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(dur==-1 || dur>=1);
	duration=dur;

	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->nPreferredStartingTimes_L=nPT_L;
	this->days_L=d_L;
	this->hours_L=h_L;
	this->type=CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES;
}

bool ConstraintActivitiesPreferredStartingTimes::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->nActivities=0;
	this->activitiesIndices.clear();

	int it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.indexOf(this->teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.augmentedSetsShareStudentsFaster(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && !act->activityTagsNames.contains(this->activityTagName)){
				continue;
		}
	
		if(duration>=1 && act->duration!=duration)
			continue;

		assert(this->nActivities < MAX_ACTIVITIES);
		//this->activitiesIndices[this->nActivities++]=i;
		this->activitiesIndices.append(i);
		this->nActivities++;
	}
	
	assert(this->activitiesIndices.count()==this->nActivities);

	//////////////////////	
	for(int k=0; k<nPreferredStartingTimes_L; k++){
		if(this->days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->nActivities>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	QList<int> localActiveActs;
	QList<int> localAllActs;

	//returns true if all activities are inactive
	int it;
	Activity* act;
	int i;
	for(i=0; i<r.activitiesList.count(); i++){
		act=r.activitiesList.at(i);

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.indexOf(this->teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.setsShareStudents(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && !act->activityTagsNames.contains(this->activityTagName)){
				continue;
		}

		if(duration>=1 && act->duration!=duration)
			continue;
	
		if(!r.inactiveActivities.contains(act->id))
			localActiveActs.append(act->id);
			
		localAllActs.append(act->id);
	}

	if(localActiveActs.count()==0 && localAllActs.count()>0)
		return true;
	else
		return false;
}

QString ConstraintActivitiesPreferredStartingTimes::getXmlDescription(Rules& r)
{
	QString s="<ConstraintActivitiesPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	if(duration>=1)
		s+="	<Duration>"+CustomFETString::number(duration)+"</Duration>\n";
	else
		s+="	<Duration></Duration>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+CustomFETString::number(this->nPreferredStartingTimes_L)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes_L; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days_L[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days_L[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours_L[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours_L[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesPreferredStartingTimes>\n";
	return s;
}

QString ConstraintActivitiesPreferredStartingTimes::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;

	QString tc, st, su, at, dur;
	
	if(this->teacherName!="")
		tc=tr("teacher=%1").arg(this->teacherName);
	else
		tc=tr("all teachers");
		
	if(this->studentsName!="")
		st=tr("students=%1").arg(this->studentsName);
	else
		st=tr("all students");
		
	if(this->subjectName!="")
		su=tr("subject=%1").arg(this->subjectName);
	else
		su=tr("all subjects");
		
	if(this->activityTagName!="")
		at=tr("activity tag=%1").arg(this->activityTagName);
	else
		at=tr("all activity tags");

	if(duration>=1)
		dur=tr("duration=%1").arg(duration);
	else
		dur=tr("all durations");

	s+=tr("Activities with %1, %2, %3, %4, %5, have a set of preferred starting times:", "%1...%5 are conditions for the activities").arg(tc).arg(st).arg(su).arg(at).arg(dur);
	s+=" ";

	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<this->nPreferredStartingTimes_L-1)
			s+="; ";
	}
	s+=", ";
	
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivitiesPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activities with:");s+="\n";

	if(this->teacherName!="")
		s+=tr("Teacher=%1").arg(this->teacherName);
	else
		s+=tr("All teachers");
	s+="\n";
	
	if(this->studentsName!="")
		s+=tr("Students=%1").arg(this->studentsName);
	else
		s+=tr("All students");
	s+="\n";
		
	if(this->subjectName!="")
		s+=tr("Subject=%1").arg(this->subjectName);
	else
		s+=tr("All subjects");
	s+="\n";
	
	if(this->activityTagName!="")
		s+=tr("Activity tag=%1").arg(this->activityTagName);
	else
		s+=tr("All activity tags");
	s+="\n";

	if(duration>=1)
		s+=tr("Duration=%1").arg(duration);
	else
		s+=tr("All durations");
	s+="\n";

	s+=tr("have a set of preferred starting times:");
	s+="\n";
	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<this->nPreferredStartingTimes_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->nActivities; i++){
		tmp=0;
		int ai=this->activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			int i;
			for(i=0; i<this->nPreferredStartingTimes_L; i++){
				if(this->days_L[i]>=0 && this->days_L[i]!=d)
					continue;
				if(this->hours_L[i]>=0 && this->hours_L[i]!=h)
					continue;
				break;
			}
			if(i==this->nPreferredStartingTimes_L){
				tmp=1;
			}
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=tr("Time constraint activities preferred starting times broken"
			 " for activity with id=%1 (%2),"
			 " increases conflicts total by %3", "%1 is the id, %2 is the detailed description of the activity")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(getActivityDetailedDescription(r, r.internalActivitiesList[ai].id))
			 .arg(CustomFETString::number(weightPercentage/100*tmp));
			 
			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	int it;

	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.indexOf(this->teacherName);
		if(it==-1)
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.setsShareStudents(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && !a->activityTagsNames.contains(this->activityTagName))
		return false;

	if(duration>=1 && a->duration!=duration)
		return false;

	return true;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::hasWrongDayOrHour(Rules& r)
{
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)<0 || days_L.at(i)>=r.nDaysPerWeek
		 || hours_L.at(i)<0 || hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivitiesPreferredStartingTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)>=0 && days_L.at(i)<r.nDaysPerWeek
		 && hours_L.at(i)>=0 && hours_L.at(i)<r.nHoursPerDay){
			newDays.append(days_L.at(i));
			newHours.append(hours_L.at(i));
			newNPref++;
		}
	
	nPreferredStartingTimes_L=newNPref;
	days_L=newDays;
	hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubactivitiesPreferredStartingTimes::ConstraintSubactivitiesPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES;
}

ConstraintSubactivitiesPreferredStartingTimes::ConstraintSubactivitiesPreferredStartingTimes(double wp, int compNo, QString te,
	QString st, QString su, QString sut, int nPT_L, QList<int> d_L, QList<int> h_L)
	: TimeConstraint(wp)
{
	assert(d_L.count()==nPT_L);
	assert(h_L.count()==nPT_L);

	this->componentNumber=compNo;
	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->nPreferredStartingTimes_L=nPT_L;
	this->days_L=d_L;
	this->hours_L=h_L;
	this->type=CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES;
}

bool ConstraintSubactivitiesPreferredStartingTimes::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->nActivities=0;
	this->activitiesIndices.clear();

	int it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		
		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.indexOf(this->teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.augmentedSetsShareStudentsFaster(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && !act->activityTagsNames.contains(this->activityTagName)){
				continue;
		}
	
		assert(this->nActivities < MAX_ACTIVITIES);	
		//this->activitiesIndices[this->nActivities++]=i;
		this->nActivities++;
		this->activitiesIndices.append(i);
	}
	
	assert(this->activitiesIndices.count()==this->nActivities);

	//////////////////////	
	for(int k=0; k<nPreferredStartingTimes_L; k++){
		if(this->days_L[k] >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours_L[k] == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours_L[k] > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint subactivities preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->nActivities>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintSubactivitiesPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	QList<int> localActiveActs;
	QList<int> localAllActs;

	//returns true if all activities are inactive
	int it;
	Activity* act;
	int i;
	for(i=0; i<r.activitiesList.count(); i++){
		act=r.activitiesList.at(i);

		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.indexOf(this->teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.setsShareStudents(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && !act->activityTagsNames.contains(this->activityTagName)){
				continue;
		}
	
		if(!r.inactiveActivities.contains(act->id))
			localActiveActs.append(act->id);
			
		localAllActs.append(act->id);
	}

	if(localActiveActs.count()==0 && localAllActs.count()>0)
		return true;
	else
		return false;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getXmlDescription(Rules& r)
{
	QString s="<ConstraintSubactivitiesPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Component_Number>"+CustomFETString::number(this->componentNumber)+"</Component_Number>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+CustomFETString::number(this->nPreferredStartingTimes_L)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes_L; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days_L[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days_L[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours_L[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours_L[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintSubactivitiesPreferredStartingTimes>\n";
	return s;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString tc, st, su, at;
	
	if(this->teacherName!="")
		tc=tr("teacher=%1").arg(this->teacherName);
	else
		tc=tr("all teachers");
		
	if(this->studentsName!="")
		st=tr("students=%1").arg(this->studentsName);
	else
		st=tr("all students");
		
	if(this->subjectName!="")
		su=tr("subject=%1").arg(this->subjectName);
	else
		su=tr("all subjects");
		
	if(this->activityTagName!="")
		at=tr("activity tag=%1").arg(this->activityTagName);
	else
		at=tr("all activity tags");
		
	QString s;
	
	s+=tr("Subactivities with %1, %2, %3, %4, %5, have a set of preferred starting times:", "%1...%5 are conditions for the subactivities")
		.arg(tr("component number=%1").arg(this->componentNumber)).arg(tc).arg(st).arg(su).arg(at);
	s+=" ";

	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<this->nPreferredStartingTimes_L-1)
			s+="; ";
	}
	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Subactivities with:");s+="\n";

	s+=tr("Component number=%1").arg(this->componentNumber);s+="\n";

	if(this->teacherName!="")
		s+=tr("Teacher=%1").arg(this->teacherName);
	else
		s+=tr("All teachers");
	s+="\n";
		
	if(this->studentsName!="")
		s+=tr("Students=%1").arg(this->studentsName);
	else
		s+=tr("All students");
	s+="\n";
		
	if(this->subjectName!="")
		s+=tr("Subject=%1").arg(this->subjectName);
	else
		s+=tr("All subjects");
	s+="\n";
	
	if(this->activityTagName!="")
		s+=tr("Activity tag=%1").arg(this->activityTagName);
	else
		s+=tr("All activity tags");
	s+="\n";

	s+=tr("have a set of preferred starting times:");
	s+="\n";
	for(int i=0; i<this->nPreferredStartingTimes_L; i++){
		if(this->days_L[i]>=0){
			s+=r.daysOfTheWeek[this->days_L[i]];
			s+=" ";
		}
		if(this->hours_L[i]>=0){
			s+=r.hoursOfTheDay[this->hours_L[i]];
		}
		if(i<this->nPreferredStartingTimes_L-1)
			s+=";  ";
	}
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubactivitiesPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->nActivities; i++){
		tmp=0;
		int ai=this->activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			int i;
			for(i=0; i<this->nPreferredStartingTimes_L; i++){
				if(this->days_L[i]>=0 && this->days_L[i]!=d)
					continue;
				if(this->hours_L[i]>=0 && this->hours_L[i]!=h)
					continue;
				break;
			}
			if(i==this->nPreferredStartingTimes_L){
				tmp=1;
			}
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=tr("Time constraint subactivities preferred starting times broken"
			 " for activity with id=%1 (%2), component number %3,"
			 " increases conflicts total by %4", "%1 is the id, %2 is the detailed description of the activity")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(getActivityDetailedDescription(r, r.internalActivitiesList[ai].id))
			 .arg(this->componentNumber)
			 .arg(CustomFETString::number(weightPercentage/100*tmp));

			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	if(!a->representsComponentNumber(this->componentNumber))
		return false;

	int it;
	
	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.indexOf(this->teacherName);
		if(it==-1)
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.setsShareStudents(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && !a->activityTagsNames.contains(this->activityTagName))
		return false;

	return true;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::hasWrongDayOrHour(Rules& r)
{
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)<0 || days_L.at(i)>=r.nDaysPerWeek
		 || hours_L.at(i)<0 || hours_L.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintSubactivitiesPreferredStartingTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(nPreferredStartingTimes_L==days_L.count());
	assert(nPreferredStartingTimes_L==hours_L.count());
	
	QList<int> newDays;
	QList<int> newHours;
	int newNPref=0;
	
	for(int i=0; i<nPreferredStartingTimes_L; i++)
		if(days_L.at(i)>=0 && days_L.at(i)<r.nDaysPerWeek
		 && hours_L.at(i)>=0 && hours_L.at(i)<r.nHoursPerDay){
			newDays.append(days_L.at(i));
			newHours.append(hours_L.at(i));
			newNPref++;
		}
	
	nPreferredStartingTimes_L=newNPref;
	days_L=newDays;
	hours_L=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingHour::ConstraintActivitiesSameStartingHour()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR;
}

ConstraintActivitiesSameStartingHour::ConstraintActivitiesSameStartingHour(double wp, int nact, const QList<int>& act)
 : TimeConstraint(wp)
 {
	assert(nact>=2);
	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR;
}

bool ConstraintActivitiesSameStartingHour::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingHour::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintActivitiesSameStartingHour::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingHour::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesSameStartingHour>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesSameStartingHour>\n";
	return s;
}

QString ConstraintActivitiesSameStartingHour::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Activities same starting hour");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);
		if(i<this->n_activities-1)
			s+=", ";
	}

	return begin+s+end;
}

QString ConstraintActivitiesSameStartingHour::getDetailedDescription(Rules& r){
	QString s;
	
	s=tr("Time constraint");s+="\n";
	s+=tr("Activities must have the same starting hour");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
			.arg(this->activitiesId[i])
			.arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled hour for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				//int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						//int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//	tmp = abs(hour1-hour2);
						if(hour1!=hour2)
							tmp=1;

						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				//int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						//int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//	tmp = abs(hour1-hour2);						
						if(hour1!=hour2)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=tr("Time constraint activities same starting hour broken, because activity with id=%1 (%2) is not at the same hour with activity with id=%3 (%4)"
							 , "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]));
							s+=". ";
							s+=tr("Conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivitiesSameStartingHour::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesSameStartingHour::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivitiesSameStartingHour::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingDay::ConstraintActivitiesSameStartingDay()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY;
}

ConstraintActivitiesSameStartingDay::ConstraintActivitiesSameStartingDay(double wp, int nact, const QList<int>& act)
 : TimeConstraint(wp)
 {
	assert(nact>=2);
	assert(act.count()==nact);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act.at(i));

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY;
}

bool ConstraintActivitiesSameStartingDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//compute the indices of the activities,
	//based on their unique ID

	assert(this->n_activities==this->activitiesId.count());

	this->_activities.clear();
	for(int i=0; i<this->n_activities; i++){
		int j=r.activitiesHash.value(activitiesId.at(i), -1);
		//assert(j>=0);
		if(j>=0)
			_activities.append(j);
		/*int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				break;
			}
		}*/
	}
	this->_n_activities=this->_activities.count();
	
	if(this->_n_activities<=1){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because you need 2 or more activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingDay::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)
	
	assert(this->n_activities==this->activitiesId.count());

	QList<int> tmpList;

	for(int i=0; i<this->n_activities; i++){
		Activity* act=r.activitiesPointerHash.value(activitiesId[i], NULL);
		if(act!=NULL)
			tmpList.append(act->id);
		/*for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				tmpList.append(act->id);
				break;
			}
		}*/
	}
	
	this->activitiesId=tmpList;
	this->n_activities=this->activitiesId.count();

	r.internalStructureComputed=false;
}

bool ConstraintActivitiesSameStartingDay::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesSameStartingDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+CustomFETString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesSameStartingDay>\n";
	return s;
}

QString ConstraintActivitiesSameStartingDay::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Activities same starting day");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("NA:%1", "Number of activities").arg(this->n_activities);s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Id:%1", "Id of activity").arg(this->activitiesId[i]);
		if(i<this->n_activities-1)
			s+=", ";
	}

	return begin+s+end;
}

QString ConstraintActivitiesSameStartingDay::getDetailedDescription(Rules& r){
	QString s;
	
	s=tr("Time constraint");s+="\n";
	s+=tr("Activities must have the same starting day");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Number of activities=%1").arg(this->n_activities);s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
			.arg(this->activitiesId[i])
			.arg(getActivityDetailedDescription(r, this->activitiesId[i]));
		s+="\n";
	}

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled hour for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						if(day1!=day2)
							tmp=1;

						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						if(day1!=day2)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=tr("Time constraint activities same starting day broken, because activity with id=%1 (%2) is not in the same day with activity with id=%3 (%4)"
							 , "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
							 .arg(this->activitiesId[i])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[i]))
							 .arg(this->activitiesId[j])
							 .arg(getActivityDetailedDescription(r, this->activitiesId[j]));
							s+=". ";
							s+=tr("Conflicts factor increase=%1").arg(CustomFETString::number(tmp*weightPercentage/100));
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivitiesSameStartingDay::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesSameStartingDay::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivitiesSameStartingDay::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTwoActivitiesConsecutive::ConstraintTwoActivitiesConsecutive()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE;
}

ConstraintTwoActivitiesConsecutive::ConstraintTwoActivitiesConsecutive(double wp, int firstActId, int secondActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->type = CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE;
}

bool ConstraintTwoActivitiesConsecutive::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(firstActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;

	////////
	
	/*for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}*/
	
	i=r.activitiesHash.value(secondActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex){	
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex);
	
	return true;
}

bool ConstraintTwoActivitiesConsecutive::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	return false;
}

QString ConstraintTwoActivitiesConsecutive::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTwoActivitiesConsecutive>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<First_Activity_Id>"+CustomFETString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+CustomFETString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTwoActivitiesConsecutive>\n";
	return s;
}

QString ConstraintTwoActivitiesConsecutive::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	s=tr("Two activities consecutive:");
	s+=" ";
	
	s+=tr("first act. id: %1", "act.=activity").arg(this->firstActivityId);
	s+=", ";
	s+=tr("second act. id: %1", "act.=activity").arg(this->secondActivityId);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTwoActivitiesConsecutive::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Two activities consecutive (second activity must be placed immediately after the first"
	 " activity, in the same day, possibly separated by breaks)"); s+="\n";
	
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("First activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->firstActivityId)
		.arg(getActivityDetailedDescription(r, this->firstActivityId));
	s+="\n";

	s+=tr("Second activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->secondActivityId)
		.arg(getActivityDetailedDescription(r, this->secondActivityId));
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintTwoActivitiesConsecutive::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek; //the hour
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour
		
		if(fd!=sd)
			nbroken=1;
		else if(fh+r.internalActivitiesList[this->firstActivityIndex].duration>sh)
			nbroken=1;
		else{
			assert(fd==sd);
			int h;
			int d=fd;
			assert(d==sd);
			for(h=fh+r.internalActivitiesList[this->firstActivityIndex].duration; h<r.nHoursPerDay; h++)
				if(!breakDayHour[d][h])
					break;
					
			assert(h<=sh);
				
			if(h!=sh)
				nbroken=1;
		}
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint two activities consecutive broken for first activity with id=%1 (%2) and "
		 "second activity with id=%3 (%4), increases conflicts total by %5", "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
		 .arg(this->firstActivityId)
		 .arg(getActivityDetailedDescription(r, this->firstActivityId))
		 .arg(this->secondActivityId)
		 .arg(getActivityDetailedDescription(r, this->secondActivityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintTwoActivitiesConsecutive::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool ConstraintTwoActivitiesConsecutive::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintTwoActivitiesConsecutive::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesConsecutive::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesConsecutive::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesConsecutive::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTwoActivitiesConsecutive::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintTwoActivitiesConsecutive::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTwoActivitiesGrouped::ConstraintTwoActivitiesGrouped()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TWO_ACTIVITIES_GROUPED;
}

ConstraintTwoActivitiesGrouped::ConstraintTwoActivitiesGrouped(double wp, int firstActId, int secondActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->type = CONSTRAINT_TWO_ACTIVITIES_GROUPED;
}

bool ConstraintTwoActivitiesGrouped::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(firstActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;	

	////////
	
	/*for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}*/

	i=r.activitiesHash.value(secondActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex){	
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex);
	
	return true;
}

bool ConstraintTwoActivitiesGrouped::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	return false;
}

QString ConstraintTwoActivitiesGrouped::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTwoActivitiesGrouped>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<First_Activity_Id>"+CustomFETString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+CustomFETString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTwoActivitiesGrouped>\n";
	return s;
}

QString ConstraintTwoActivitiesGrouped::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	s=tr("Two activities grouped:");
	s+=" ";
	
	s+=tr("first act. id: %1", "act.=activity").arg(this->firstActivityId);
	s+=", ";
	s+=tr("second act. id: %1", "act.=activity").arg(this->secondActivityId);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTwoActivitiesGrouped::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Two activities grouped (the activities must be placed in the same day, "
	 "one immediately following the other, in any order, possibly separated by breaks)"); s+="\n";
	
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("First activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->firstActivityId)
		.arg(getActivityDetailedDescription(r, this->firstActivityId));
	s+="\n";

	s+=tr("Second activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->secondActivityId)
		.arg(getActivityDetailedDescription(r, this->secondActivityId));
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintTwoActivitiesGrouped::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek; //the hour
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour
		
		if(fd!=sd)
			nbroken=1;
		else if(fd==sd && fh+r.internalActivitiesList[this->firstActivityIndex].duration <= sh){
			int h;
			int d=fd;
			assert(d==sd);
			for(h=fh+r.internalActivitiesList[this->firstActivityIndex].duration; h<r.nHoursPerDay; h++)
				if(!breakDayHour[d][h])
					break;
					
			assert(h<=sh);	
				
			if(h!=sh)
				nbroken=1;
		}
		else if(fd==sd && sh+r.internalActivitiesList[this->secondActivityIndex].duration <= fh){
			int h;
			int d=sd;
			assert(d==fd);
			for(h=sh+r.internalActivitiesList[this->secondActivityIndex].duration; h<r.nHoursPerDay; h++)
				if(!breakDayHour[d][h])
					break;
					
			assert(h<=fh);	
				
			if(h!=fh)
				nbroken=1;
		}
		else
			nbroken=1;
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint two activities grouped broken for first activity with id=%1 (%2) and "
		 "second activity with id=%3 (%4), increases conflicts total by %5", "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
		 .arg(this->firstActivityId)
		 .arg(getActivityDetailedDescription(r, this->firstActivityId))
		 .arg(this->secondActivityId)
		 .arg(getActivityDetailedDescription(r, this->secondActivityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintTwoActivitiesGrouped::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool ConstraintTwoActivitiesGrouped::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintTwoActivitiesGrouped::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesGrouped::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesGrouped::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintTwoActivitiesGrouped::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTwoActivitiesGrouped::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintTwoActivitiesGrouped::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintThreeActivitiesGrouped::ConstraintThreeActivitiesGrouped()
	: TimeConstraint()
{
	this->type = CONSTRAINT_THREE_ACTIVITIES_GROUPED;
}

ConstraintThreeActivitiesGrouped::ConstraintThreeActivitiesGrouped(double wp, int firstActId, int secondActId, int thirdActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->thirdActivityId=thirdActId;
	this->type = CONSTRAINT_THREE_ACTIVITIES_GROUPED;
}

bool ConstraintThreeActivitiesGrouped::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(firstActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;

	////////
	
	/*for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}*/

	i=r.activitiesHash.value(secondActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	////////
	
	/*for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->thirdActivityId)
			break;
	}*/

	i=r.activitiesHash.value(thirdActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->thirdActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex || firstActivityIndex==thirdActivityIndex || secondActivityIndex==thirdActivityIndex){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex && firstActivityIndex!=thirdActivityIndex && secondActivityIndex!=thirdActivityIndex);
	
	return true;
}

bool ConstraintThreeActivitiesGrouped::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	if(r.inactiveActivities.contains(this->thirdActivityId))
		return true;
	return false;
}

QString ConstraintThreeActivitiesGrouped::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintThreeActivitiesGrouped>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<First_Activity_Id>"+CustomFETString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+CustomFETString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="	<Third_Activity_Id>"+CustomFETString::number(this->thirdActivityId)+"</Third_Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintThreeActivitiesGrouped>\n";
	return s;
}

QString ConstraintThreeActivitiesGrouped::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	s=tr("Three activities grouped:");
	s+=" ";
	
	s+=tr("first act. id: %1", "act.=activity").arg(this->firstActivityId);
	s+=", ";
	s+=tr("second act. id: %1", "act.=activity").arg(this->secondActivityId);
	s+=", ";
	s+=tr("third act. id: %1", "act.=activity").arg(this->thirdActivityId);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintThreeActivitiesGrouped::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Three activities grouped (the activities must be placed in the same day, "
	 "one immediately following the other, as a block of three activities, in any order, possibly separated by breaks)"); s+="\n";
	
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("First activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->firstActivityId)
		.arg(getActivityDetailedDescription(r, this->firstActivityId));
	s+="\n";

	s+=tr("Second activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->secondActivityId)
		.arg(getActivityDetailedDescription(r, this->secondActivityId));
	s+="\n";
	
	s+=tr("Third activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->thirdActivityId)
		.arg(getActivityDetailedDescription(r, this->thirdActivityId));
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintThreeActivitiesGrouped::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME && c.times[this->thirdActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek; //the hour
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour
		int td=c.times[this->thirdActivityIndex]%r.nDaysPerWeek; //the day when third activity was scheduled
		int th=c.times[this->thirdActivityIndex]/r.nDaysPerWeek; //the hour
		
		if(!(fd==sd && fd==td))
			nbroken=1;
		else{
			assert(fd==sd && fd==td && sd==td);
			int a1=-1,a2=-1,a3=-1;
			if(fh>=sh && fh>=th && sh>=th){
				a1=thirdActivityIndex;
				a2=secondActivityIndex;
				a3=firstActivityIndex;
				//out<<"321"<<endl;
			}
			else if(fh>=sh && fh>=th && th>=sh){
				a1=secondActivityIndex;
				a2=thirdActivityIndex;
				a3=firstActivityIndex;
				//out<<"231"<<endl;
			}
			else if(sh>=fh && sh>=th && fh>=th){
				a1=thirdActivityIndex;
				a2=firstActivityIndex;
				a3=secondActivityIndex;
				//out<<"312"<<endl;
			}
			else if(sh>=fh && sh>=th && th>=fh){
				a1=firstActivityIndex;
				a2=thirdActivityIndex;
				a3=secondActivityIndex;
				//out<<"132"<<endl;
			}
			else if(th>=fh && th>=sh && fh>=sh){
				a1=secondActivityIndex;
				a2=firstActivityIndex;
				a3=thirdActivityIndex;
				//out<<"213"<<endl;
			}
			else if(th>=fh && th>=sh && sh>=fh){
				a1=firstActivityIndex;
				a2=secondActivityIndex;
				a3=thirdActivityIndex;
				//out<<"123"<<endl;
			}
			else
				assert(0);
			
			int a1d=c.times[a1]%r.nDaysPerWeek; //the day for a1
			int a1h=c.times[a1]/r.nDaysPerWeek; //the day for a1
			int a1dur=r.internalActivitiesList[a1].duration;

			int a2d=c.times[a2]%r.nDaysPerWeek; //the day for a2
			int a2h=c.times[a2]/r.nDaysPerWeek; //the day for a2
			int a2dur=r.internalActivitiesList[a2].duration;

			int a3d=c.times[a3]%r.nDaysPerWeek; //the day for a3
			int a3h=c.times[a3]/r.nDaysPerWeek; //the day for a3
			//int a3dur=r.internalActivitiesList[a3].duration;
			
			int hoursBetweenThem=-1;
			
			assert(a1d==a2d && a1d==a3d);
			
			if(a1h+a1dur<=a2h && a2h+a2dur<=a3h){
				hoursBetweenThem=0;
				for(int hh=a1h+a1dur; hh<a2h; hh++)
					if(!breakDayHour[a1d][hh])
						hoursBetweenThem++;

				for(int hh=a2h+a2dur; hh<a3h; hh++)
					if(!breakDayHour[a2d][hh])
						hoursBetweenThem++;
			}
			
			if(hoursBetweenThem==0)
				nbroken=0;
			else
				nbroken=1;
		}
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint three activities grouped broken for first activity with id=%1 (%2), "
		 "second activity with id=%3 (%4) and third activity with id=%5 (%6), increases conflicts total by %7",
		 "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr., %5 id, %6 det. descr.")
		 .arg(this->firstActivityId)
		 .arg(getActivityDetailedDescription(r, this->firstActivityId))
		 .arg(this->secondActivityId)
		 .arg(getActivityDetailedDescription(r, this->secondActivityId))
		 .arg(this->thirdActivityId)
		 .arg(getActivityDetailedDescription(r, this->thirdActivityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintThreeActivitiesGrouped::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	if(this->thirdActivityId==a->id)
		return true;
	return false;
}

bool ConstraintThreeActivitiesGrouped::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintThreeActivitiesGrouped::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintThreeActivitiesGrouped::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintThreeActivitiesGrouped::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintThreeActivitiesGrouped::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintThreeActivitiesGrouped::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintThreeActivitiesGrouped::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTwoActivitiesOrdered::ConstraintTwoActivitiesOrdered()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TWO_ACTIVITIES_ORDERED;
}

ConstraintTwoActivitiesOrdered::ConstraintTwoActivitiesOrdered(double wp, int firstActId, int secondActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->type = CONSTRAINT_TWO_ACTIVITIES_ORDERED;
}

bool ConstraintTwoActivitiesOrdered::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(firstActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;	

	////////
	
	/*for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}*/

	i=r.activitiesHash.value(secondActivityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex){	
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex);
	
	return true;
}

bool ConstraintTwoActivitiesOrdered::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	return false;
}

QString ConstraintTwoActivitiesOrdered::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTwoActivitiesOrdered>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<First_Activity_Id>"+CustomFETString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+CustomFETString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTwoActivitiesOrdered>\n";
	return s;
}

QString ConstraintTwoActivitiesOrdered::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	
	s=tr("Two activities ordered:");
	s+=" ";
	
	s+=tr("first act. id: %1", "act.=activity").arg(this->firstActivityId);
	s+=", ";
	s+=tr("second act. id: %1", "act.=activity").arg(this->secondActivityId);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTwoActivitiesOrdered::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Two activities ordered (second activity must be placed at any time in the week after the first"
	 " activity)"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("First activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->firstActivityId)
		.arg(getActivityDetailedDescription(r, this->firstActivityId));
	s+="\n";

	s+=tr("Second activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->secondActivityId)
		.arg(getActivityDetailedDescription(r, this->secondActivityId));
	s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTwoActivitiesOrdered::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek
		  + r.internalActivitiesList[this->firstActivityIndex].duration-1; //the end hour of first activity
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the start hour of second activity
		
		if(!(fd<sd || (fd==sd && fh<sh)))
			nbroken=1;
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint two activities ordered broken for first activity with id=%1 (%2) and "
		 "second activity with id=%3 (%4), increases conflicts total by %5", "%1 is the id, %2 is the detailed description of the activity, %3 id, %4 det. descr.")
		 .arg(this->firstActivityId)
		 .arg(getActivityDetailedDescription(r, this->firstActivityId))
		 .arg(this->secondActivityId)
		 .arg(getActivityDetailedDescription(r, this->secondActivityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintTwoActivitiesOrdered::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool ConstraintTwoActivitiesOrdered::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintTwoActivitiesOrdered::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesOrdered::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTwoActivitiesOrdered::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintTwoActivitiesOrdered::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTwoActivitiesOrdered::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintTwoActivitiesOrdered::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityEndsStudentsDay::ConstraintActivityEndsStudentsDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY;
}

ConstraintActivityEndsStudentsDay::ConstraintActivityEndsStudentsDay(double wp, int actId)
	: TimeConstraint(wp)
{
	this->activityId = actId;
	this->type = CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY;
}

bool ConstraintActivityEndsStudentsDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	/*Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}*/
	
	int i=r.activitiesHash.value(activityId, r.nInternalActivities);
	
	if(i==r.nInternalActivities){
		//assert(0);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (because it refers to invalid activity id). Please correct it (maybe removing it is a solution):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->activityIndex=i;	
	return true;
}

bool ConstraintActivityEndsStudentsDay::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityEndsStudentsDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintActivityEndsStudentsDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivityEndsStudentsDay>\n";
	return s;
}

QString ConstraintActivityEndsStudentsDay::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Act. id: %1 (%2) must end students' day", 
		"%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+=", ";

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivityEndsStudentsDay::getDetailedDescription(Rules& r)
{
	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activity must end students' day");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity.")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityEndsStudentsDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		
		int i=this->activityIndex;
		for(int j=0; j<r.internalActivitiesList[i].iSubgroupsList.count(); j++){
			int sb=r.internalActivitiesList[i].iSubgroupsList.at(j);
			for(int hh=h+r.internalActivitiesList[i].duration; hh<r.nHoursPerDay; hh++)
				if(subgroupsMatrix[sb][d][hh]>0){
					nbroken=1;
					break;
				}
			if(nbroken)
				break;
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=tr("Time constraint activity ends students' day broken for activity with id=%1 (%2), increases conflicts total by %3",
		 "%1 is the id, %2 is the detailed description of the activity")
		 .arg(this->activityId)
		 .arg(getActivityDetailedDescription(r, this->activityId))
		 .arg(CustomFETString::number(weightPercentage/100*nbroken));

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivityEndsStudentsDay::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityEndsStudentsDay::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivityEndsStudentsDay::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
	
	this->allowEmptyDays=true;
}

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily(double wp, int minhours, bool _allowEmptyDays)
 : TimeConstraint(wp)
 {
	assert(minhours>0);
	this->minHoursDaily=minhours;
	
	this->allowEmptyDays=_allowEmptyDays;

	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
}

bool ConstraintTeachersMinHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(r);
	
	if(allowEmptyDays==false){
		QString s=tr("Cannot generate a timetable with a constraint teachers min hours daily with allow empty days=false. Please modify it,"
			" so that it allows empty days. If you need a facility like that, please use constraint teachers min days per week");
		s+="\n\n";
		s+=tr("Constraint is:")+"\n"+this->getDetailedDescription(r);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"), s);
		
		return false;
	}
	
	return true;
}

bool ConstraintTeachersMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMinHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Minimum_Hours_Daily>"+CustomFETString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	if(this->allowEmptyDays)
		s+="	<Allow_Empty_Days>true</Allow_Empty_Days>\n";
	else
		s+="	<Allow_Empty_Days>false</Allow_Empty_Days>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMinHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMinHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers min hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("mH:%1", "Min hours (daily)").arg(this->minHoursDaily);s+=", ";
	s+=tr("AED:%1", "Allow empty days").arg(yesNoTranslated(this->allowEmptyDays));

	return begin+s+end;
}

QString ConstraintTeachersMinHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the minimum number of hours daily"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Minimum hours daily=%1").arg(this->minHoursDaily);s+="\n";
	s+=tr("Allow empty days=%1").arg(yesNoTranslated(this->allowEmptyDays));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	assert(this->allowEmptyDays==true);

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
					nbroken++;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
					nbroken++;

					if(conflictsString!=NULL){
						QString s=(tr("Time constraint teachers min %1 hours daily broken for teacher %2, on day %3, length=%4.")
						 .arg(CustomFETString::number(this->minHoursDaily))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 .arg(n_hours_daily)
						 )
						 +
						 " "
						 +
						 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100)));
						
						dl.append(s);
						cl.append(weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(minHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeachersMinHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMinHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minHoursDaily>r.nHoursPerDay)
		minHoursDaily=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinHoursDaily::ConstraintTeacherMinHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_HOURS_DAILY;
	
	this->allowEmptyDays=true;
}

ConstraintTeacherMinHoursDaily::ConstraintTeacherMinHoursDaily(double wp, int minhours, const QString& teacher, bool _allowEmptyDays)
 : TimeConstraint(wp)
 {
	assert(minhours>0);
	this->minHoursDaily=minhours;
	this->teacherName=teacher;
	
	this->allowEmptyDays=_allowEmptyDays;

	this->type=CONSTRAINT_TEACHER_MIN_HOURS_DAILY;
}

bool ConstraintTeacherMinHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	
	if(allowEmptyDays==false){
		QString s=tr("Cannot generate a timetable with a constraint teacher min hours daily with allow empty days=false. Please modify it,"
			" so that it allows empty days. If you need a facility like that, please use constraint teacher min days per week");
		s+="\n\n";
		s+=tr("Constraint is:")+"\n"+this->getDetailedDescription(r);
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"), s);
		
		return false;
	}
	
	return true;
}

bool ConstraintTeacherMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMinHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Minimum_Hours_Daily>"+CustomFETString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	if(this->allowEmptyDays)
		s+="	<Allow_Empty_Days>true</Allow_Empty_Days>\n";
	else
		s+="	<Allow_Empty_Days>false</Allow_Empty_Days>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMinHoursDaily>\n";
	return s;
}

QString ConstraintTeacherMinHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher min hours daily");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=", ";
	s+=tr("mH:%1", "Minimum hours (daily)").arg(this->minHoursDaily);s+=", ";
	s+=tr("AED:%1", "Allow empty days").arg(yesNoTranslated(this->allowEmptyDays));

	return begin+s+end;
}

QString ConstraintTeacherMinHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher must respect the minimum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Minimum hours daily=%1").arg(this->minHoursDaily);s+="\n";
	s+=tr("Allow empty days=%1").arg(yesNoTranslated(this->allowEmptyDays));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	assert(this->allowEmptyDays==true);

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
				nbroken++;
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint teacher min %1 hours daily broken for teacher %2, on day %3, length=%4.")
					 .arg(CustomFETString::number(this->minHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(n_hours_daily)
					 )
					 +" "
					 +
					 tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100));
						
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(nbroken==0);
			
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(minHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeacherMinHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMinHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minHoursDaily>r.nHoursPerDay)
		minHoursDaily=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinDaysPerWeek::ConstraintTeacherMinDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK;
}

ConstraintTeacherMinDaysPerWeek::ConstraintTeacherMinDaysPerWeek(double wp, int mindays, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(mindays>0);
	this->minDaysPerWeek=mindays;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK;
}

bool ConstraintTeacherMinDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMinDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMinDaysPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMinDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Minimum_Days_Per_Week>"+CustomFETString::number(this->minDaysPerWeek)+"</Minimum_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherMinDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherMinDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teacher min days per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=", ";
	s+=tr("mD:%1", "Minimum days per week").arg(this->minDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherMinDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher must respect the minimum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Minimum days per week=%1").arg(this->minDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMinDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	int i=this->teacher_ID;
	int nd=0;
	for(int d=0; d<r.nDaysPerWeek; d++){
		for(int h=0; h<r.nHoursPerDay; h++){
			if(teachersMatrix[i][d][h]>0){
				nd++;
				break;
			}
		}
	}

	if(nd<this->minDaysPerWeek){
		nbroken+=this->minDaysPerWeek-nd;

		if(conflictsString!=NULL){
			QString s=(tr(
			 "Time constraint teacher min %1 days per week broken for teacher %2.")
			 .arg(CustomFETString::number(this->minDaysPerWeek))
			 .arg(r.internalTeachersList[i]->name)
			 )
			 +" "
			 +
			 tr("This increases the conflicts total by %1").arg(CustomFETString::number(double(nbroken)*weightPercentage/100));
				
			dl.append(s);
			cl.append(double(nbroken)*weightPercentage/100);
		
			*conflictsString+= s+"\n";
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(nbroken==0);
			
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMinDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(minDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeacherMinDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherMinDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minDaysPerWeek>r.nDaysPerWeek)
		minDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinDaysPerWeek::ConstraintTeachersMinDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK;
}

ConstraintTeachersMinDaysPerWeek::ConstraintTeachersMinDaysPerWeek(double wp, int mindays)
 : TimeConstraint(wp)
 {
	assert(mindays>0);
	this->minDaysPerWeek=mindays;

	this->type=CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK;
}

bool ConstraintTeachersMinDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMinDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMinDaysPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMinDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Minimum_Days_Per_Week>"+CustomFETString::number(this->minDaysPerWeek)+"</Minimum_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersMinDaysPerWeek>\n";
	return s;
}

QString ConstraintTeachersMinDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+=tr("Teachers min days per week");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("mD:%1", "Minimum days per week").arg(this->minDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersMinDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers must respect the minimum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Minimum days per week=%1").arg(this->minDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMinDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbrokentotal=0;
	for(int i=0; i<r.nInternalTeachers; i++){
		int nbroken;

		nbroken=0;
		//int i=this->teacher_ID;
		int nd=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			for(int h=0; h<r.nHoursPerDay; h++){
				if(teachersMatrix[i][d][h]>0){
					nd++;
					break;
				}
			}
		}

		if(nd<this->minDaysPerWeek){
			nbroken+=this->minDaysPerWeek-nd;
			nbrokentotal+=nbroken;

			if(conflictsString!=NULL){
				QString s=(tr(
				 "Time constraint teachers min %1 days per week broken for teacher %2.")
				 .arg(CustomFETString::number(this->minDaysPerWeek))
				 .arg(r.internalTeachersList[i]->name)
				 )
				 +" "
				 +
				 tr("This increases the conflicts total by %1").arg(CustomFETString::number(double(nbroken)*weightPercentage/100));
					
				dl.append(s);
				cl.append(double(nbroken)*weightPercentage/100);
			
				*conflictsString+= s+"\n";
			}
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(nbrokentotal==0);
			
	return weightPercentage/100 * nbrokentotal;
}

bool ConstraintTeachersMinDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMinDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return true;
}

bool ConstraintTeachersMinDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(minDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeachersMinDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersMinDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minDaysPerWeek>r.nDaysPerWeek)
		minDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek(double wp, int maxnd, QString tn, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->teacherName = tn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);
	if(this->startHour>=this->endHour){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher interval max days per week is wrong because start hour < first hour of the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	return true;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Teacher interval max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("T:%1", "Abbreviation for teacher").arg(this->teacherName);s+=", ";
	s+=tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(tr("End of the day"));
	s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A teacher respects working in an hourly interval a maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	int t=this->teacher_ID;

	nbroken=0;
	bool ocDay[MAX_DAYS_PER_WEEK];
	for(int d=0; d<r.nDaysPerWeek; d++){
		ocDay[d]=false;
		for(int h=startHour; h<endHour; h++){
			if(teachersMatrix[t][d][h]>0){
				ocDay[d]=true;
			}
		}
	}
	int nOcDays=0;
	for(int d=0; d<r.nDaysPerWeek; d++)
		if(ocDay[d])
			nOcDays++;
	if(nOcDays > this->maxDaysPerWeek){
		nbroken+=nOcDays-this->maxDaysPerWeek;

		if(nbroken>0){
			QString s= tr("Time constraint teacher interval max days per week broken for teacher: %1, allowed %2 days, required %3 days.")
			 .arg(r.internalTeachersList[t]->name)
			 .arg(this->maxDaysPerWeek)
			 .arg(nOcDays);
			s+=" ";
			s += tr("This increases the conflicts total by %1")
			 .arg(CustomFETString::number(nbroken*weightPercentage/100));
			 
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->startHour>=r.nHoursPerDay)
		return true;
	if(this->endHour>r.nHoursPerDay)
		return true;
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	assert(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay);

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersIntervalMaxDaysPerWeek::ConstraintTeachersIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintTeachersIntervalMaxDaysPerWeek::ConstraintTeachersIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	if(this->startHour>=this->endHour){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teachers interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teachers interval max days per week is wrong because start hour < first hour of the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teachers interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	return true;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintTeachersIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Teachers interval max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(tr("End of the day"));
	s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All teachers respect working in an hourly interval a maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int t=0; t<r.nInternalTeachers; t++){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(teachersMatrix[t][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if(nOcDays-this->maxDaysPerWeek>0){
				QString s= tr("Time constraint teachers interval max days per week broken for teacher: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalTeachersList[t]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number((nOcDays-this->maxDaysPerWeek)*weightPercentage/100));
				 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->startHour>=r.nHoursPerDay)
		return true;
	if(this->endHour>r.nHoursPerDay)
		return true;
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	assert(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay);

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek(double wp, int maxnd, QString sn, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->students = sn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	if(this->startHour>=this->endHour){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set interval max days per week is wrong because start hour < first hour of the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	/////////
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);

	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set interval max days per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Students set interval max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Abbreviation for students (sets)").arg(this->students);s+=", ";
	s+=tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(tr("End of the day"));
	s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set respects working in an hourly interval a maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= tr("Time constraint students set interval max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number((nOcDays-this->maxDaysPerWeek)*weightPercentage/100));
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->startHour>=r.nHoursPerDay)
		return true;
	if(this->endHour>r.nHoursPerDay)
		return true;
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	assert(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay);

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsIntervalMaxDaysPerWeek::ConstraintStudentsIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsIntervalMaxDaysPerWeek::ConstraintStudentsIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	if(this->startHour>=this->endHour){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students interval max days per week is wrong because start hour < first hour of the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	return true;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Students interval max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour").arg(tr("End of the day"));
	s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students respect working in an hourly interval a maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= tr("Time constraint students interval max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number((nOcDays-this->maxDaysPerWeek)*weightPercentage/100));
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return true;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->startHour>=r.nHoursPerDay)
		return true;
	if(this->endHour>r.nHoursPerDay)
		return true;
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	assert(this->startHour<r.nHoursPerDay && this->endHour<=r.nHoursPerDay);

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesEndStudentsDay::ConstraintActivitiesEndStudentsDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY;
}

ConstraintActivitiesEndStudentsDay::ConstraintActivitiesEndStudentsDay(double wp, QString te,
	QString st, QString su, QString sut)
	: TimeConstraint(wp)
{
	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->type=CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY;
}

bool ConstraintActivitiesEndStudentsDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->nActivities=0;
	this->activitiesIndices.clear();

	int it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.indexOf(this->teacherName);
			if(it==-1)
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.augmentedSetsShareStudentsFaster(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && !act->activityTagsNames.contains(this->activityTagName)){
				continue;
		}
	
		assert(this->nActivities < MAX_ACTIVITIES);	
		this->nActivities++;
		this->activitiesIndices.append(i);
	}
	
	assert(this->activitiesIndices.count()==this->nActivities);

	if(this->nActivities>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesEndStudentsDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintActivitiesEndStudentsDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintActivitiesEndStudentsDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesEndStudentsDay>\n";
	return s;
}

QString ConstraintActivitiesEndStudentsDay::getDescription(Rules& r)
{
	Q_UNUSED(r);
	
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString tc, st, su, at;
	
	if(this->teacherName!="")
		tc=tr("teacher=%1").arg(this->teacherName);
	else
		tc=tr("all teachers");
		
	if(this->studentsName!="")
		st=tr("students=%1").arg(this->studentsName);
	else
		st=tr("all students");
		
	if(this->subjectName!="")
		su=tr("subject=%1").arg(this->subjectName);
	else
		su=tr("all subjects");
		
	if(this->activityTagName!="")
		at=tr("activity tag=%1").arg(this->activityTagName);
	else
		at=tr("all activity tags");
	
	QString s;
	s+=tr("Activities with %1, %2, %3, %4, must end students' day", "%1...%4 are conditions for the activities").arg(tc).arg(st).arg(su).arg(at);

	s+=", ";

	s+=tr("WP:%1%", "Abbreviation for Weight Percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintActivitiesEndStudentsDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("Activities with:");s+="\n";

	if(this->teacherName!="")
		s+=tr("Teacher=%1").arg(this->teacherName);
	else
		s+=tr("All teachers");
	s+="\n";
		
	if(this->studentsName!="")
		s+=tr("Students=%1").arg(this->studentsName);
	else
		s+=tr("All students");
	s+="\n";
		
	if(this->subjectName!="")
		s+=tr("Subject=%1").arg(this->subjectName);
	else
		s+=tr("All subjects");
	s+="\n";
		
	if(this->activityTagName!="")
		s+=tr("Activity tag=%1").arg(this->activityTagName);
	else
		s+=tr("All activity tags");
	s+="\n";

	s+=tr("must end students' day");
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesEndStudentsDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	assert(r.internalStructureComputed);

	for(int kk=0; kk<this->nActivities; kk++){
		int tmp=0;
		int ai=this->activitiesIndices[kk];
	
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
		
			for(int j=0; j<r.internalActivitiesList[ai].iSubgroupsList.count(); j++){
				int sb=r.internalActivitiesList[ai].iSubgroupsList.at(j);
				for(int hh=h+r.internalActivitiesList[ai].duration; hh<r.nHoursPerDay; hh++)
					if(subgroupsMatrix[sb][d][hh]>0){
						nbroken++;
						tmp=1;
						break;
					}
				if(tmp>0)
					break;
			}

			if(conflictsString!=NULL && tmp>0){
				QString s=tr("Time constraint activities end students' day broken for activity with id=%1 (%2), increases conflicts total by %3",
				 "%1 is the id, %2 is the detailed description of the activity")
				 .arg(r.internalActivitiesList[ai].id)
				 .arg(getActivityDetailedDescription(r, r.internalActivitiesList[ai].id))
				 .arg(CustomFETString::number(weightPercentage/100*tmp));

				dl.append(s);
				cl.append(weightPercentage/100*tmp);
	
				*conflictsString+= s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToActivity(Rules& r, Activity* a)
{
	int it;

	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.indexOf(this->teacherName);
		if(it==-1)
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.setsShareStudents(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && !a->activityTagsNames.contains(this->activityTagName))
		return false;

	return true;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
		
	return false;
}

bool ConstraintActivitiesEndStudentsDay::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesEndStudentsDay::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);
	
	return true;
}

bool ConstraintActivitiesEndStudentsDay::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersActivityTagMaxHoursDaily::ConstraintTeachersActivityTagMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

ConstraintTeachersActivityTagMaxHoursDaily::ConstraintTeachersActivityTagMaxHoursDaily(double wp, int maxhours, const QString& activityTag)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;
	this->activityTagName=activityTag;

	this->type=CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);
	
	this->canonicalTeachersList.clear();
	for(int i=0; i<r.nInternalTeachers; i++){
		bool found=false;
	
		Teacher* tch=r.internalTeachersList[i];
		foreach(int actIndex, tch->activitiesForTeacher){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalTeachersList.append(i);
	}

	return true;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersActivityTagMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersActivityTagMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeachersActivityTagMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersActivityTagMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Teachers for activity tag %1 have max %2 hours daily").arg(this->activityTagName).arg(this->maxHoursDaily);s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTeachersActivityTagMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("All teachers, for an activity tag, must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName); s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersActivityTagMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->canonicalTeachersList){
		Teacher* tch=r.internalTeachersList[i];
		int crtTeacherTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtTeacherTimetableActivityTag[d][h]=-1;
				
		foreach(int ai, tch->activitiesForTeacher)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtTeacherTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtTeacherTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}
	
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nd=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(crtTeacherTimetableActivityTag[d][h]==this->activityTagIndex)
					nd++;

			if(nd>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr("Time constraint teachers activity tag %1 max %2 hours daily broken for teacher %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nd)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100.0)));
					
					dl.append(s);
					cl.append(weightPercentage/100.0);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100.0 * nbroken;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	return s->name==this->activityTagName;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeachersActivityTagMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherActivityTagMaxHoursDaily::ConstraintTeacherActivityTagMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

ConstraintTeacherActivityTagMaxHoursDaily::ConstraintTeacherActivityTagMaxHoursDaily(double wp, int maxhours, const QString& teacher, const QString& activityTag)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;
	this->teacherName=teacher;
	this->activityTagName=activityTag;

	this->type=CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	assert(this->teacher_ID>=0);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);

	this->canonicalTeachersList.clear();
	int i=this->teacher_ID;
	bool found=false;
	
	Teacher* tch=r.internalTeachersList[i];
	foreach(int actIndex, tch->activitiesForTeacher){
		if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
			found=true;
			break;
		}
	}
		
	if(found)
		this->canonicalTeachersList.append(i);

	return true;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherActivityTagMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherActivityTagMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintTeacherActivityTagMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeacherActivityTagMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Teacher %1 for activity tag %2 has max %3 hours daily").arg(this->teacherName).arg(this->activityTagName).arg(this->maxHoursDaily);s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintTeacherActivityTagMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("A teacher for an activity tag must respect the maximum number of hours daily");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherActivityTagMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->canonicalTeachersList){
		Teacher* tch=r.internalTeachersList[i];
		int crtTeacherTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtTeacherTimetableActivityTag[d][h]=-1;
				
		foreach(int ai, tch->activitiesForTeacher)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtTeacherTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtTeacherTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}
	
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nd=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(crtTeacherTimetableActivityTag[d][h]==this->activityTagIndex)
					nd++;

			if(nd>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr("Time constraint teacher activity tag %1 max %2 hours daily broken for teacher %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nd)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100.0)));
					
					dl.append(s);
					cl.append(weightPercentage/100.0);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100.0 * nbroken;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	return this->activityTagName==s->name;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintTeacherActivityTagMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsActivityTagMaxHoursDaily::ConstraintStudentsActivityTagMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsActivityTagMaxHoursDaily::ConstraintStudentsActivityTagMaxHoursDaily(double wp, int maxnh, const QString& activityTag)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->activityTagName=activityTag;
	this->type = CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);
	
	this->canonicalSubgroupsList.clear();
	for(int i=0; i<r.nInternalSubgroups; i++){
		bool found=false;
	
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		foreach(int actIndex, sbg->activitiesForSubgroup){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalSubgroupsList.append(i);
	}

	return true;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsActivityTagMaxHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsActivityTagMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	
	if(this->maxHoursDaily>=0)
		s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	else
		assert(0);
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsActivityTagMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsActivityTagMaxHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Students for activity tag %1 have max %2 hours daily")
		.arg(this->activityTagName).arg(this->maxHoursDaily); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintStudentsActivityTagMaxHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("All students, for an activity tag, must respect the maximum number of hours daily"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsActivityTagMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int nbroken;

	nbroken=0;
	
	foreach(int i, this->canonicalSubgroupsList){
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		int crtSubgroupTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtSubgroupTimetableActivityTag[d][h]=-1;
		foreach(int ai, sbg->activitiesForSubgroup)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtSubgroupTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtSubgroupTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}

		for(int d=0; d<r.nDaysPerWeek; d++){
			int nd=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(crtSubgroupTimetableActivityTag[d][h]==this->activityTagIndex)
					nd++;
				
			if(nd>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students, activity tag %1, max %2 hours daily, broken for subgroup %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursDaily))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nd)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100.0)));
					
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}
	
	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100.0 * nbroken;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	return s->name==this->activityTagName;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsActivityTagMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetActivityTagMaxHoursDaily::ConstraintStudentsSetActivityTagMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsSetActivityTagMaxHoursDaily::ConstraintStudentsSetActivityTagMaxHoursDaily(double wp, int maxnh, const QString& s, const QString& activityTag)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->students = s;
	this->activityTagName=activityTag;
	this->type = CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetActivityTagMaxHoursDaily::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetActivityTagMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Maximum_Hours_Daily>"+CustomFETString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetActivityTagMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetActivityTagMaxHoursDaily::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Students set %1 for activity tag %2 has max %3 hours daily").arg(this->students).arg(this->activityTagName).arg(this->maxHoursDaily);
	s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));

	return begin+s+end;
}

QString ConstraintStudentsSetActivityTagMaxHoursDaily::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("A students set, for an activity tag, must respect the maximum number of hours daily"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Maximum hours daily=%1").arg(this->maxHoursDaily);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->activityTagIndex=r.searchActivityTag(this->activityTagName);
	activityTagIndex=r.activityTagsHash.value(activityTagName, -1);
	assert(this->activityTagIndex>=0);

	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);
	
	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max hours daily is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	/////////////
	this->canonicalSubgroupsList.clear();
	foreach(int i, this->iSubgroupsList){
		bool found=false;
	
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		foreach(int actIndex, sbg->activitiesForSubgroup){
			if(r.internalActivitiesList[actIndex].iActivityTagsSet.contains(this->activityTagIndex)){
				found=true;
				break;
			}
		}
		
		if(found)
			this->canonicalSubgroupsList.append(i);
	}

		
	return true;
}

double ConstraintStudentsSetActivityTagMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	
	foreach(int i, this->canonicalSubgroupsList){
		StudentsSubgroup* sbg=r.internalSubgroupsList[i];
		int crtSubgroupTimetableActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				crtSubgroupTimetableActivityTag[d][h]=-1;
		foreach(int ai, sbg->activitiesForSubgroup)if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				assert(crtSubgroupTimetableActivityTag[d][h+dur]==-1);
				if(r.internalActivitiesList[ai].iActivityTagsSet.contains(this->activityTagIndex))
					crtSubgroupTimetableActivityTag[d][h+dur]=this->activityTagIndex;
			}
		}

		for(int d=0; d<r.nDaysPerWeek; d++){
			int nd=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(crtSubgroupTimetableActivityTag[d][h]==this->activityTagIndex)
					nd++;
				
			if(nd>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(tr(
					 "Time constraint students set, activity tag %1, max %2 hours daily, broken for subgroup %3, on day %4, length=%5.")
					 .arg(this->activityTagName)
					 .arg(CustomFETString::number(this->maxHoursDaily))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nd)
					 )
					 +
					 " "
					 +
					 (tr("This increases the conflicts total by %1").arg(CustomFETString::number(weightPercentage/100.0)));
					
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}
	
	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100.0 * nbroken;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::hasWrongDayOrHour(Rules& r)
{
	if(maxHoursDaily>r.nHoursPerDay)
		return true;
		
	return false;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetActivityTagMaxHoursDaily::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxHoursDaily>r.nHoursPerDay)
		maxHoursDaily=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxGapsPerDay::ConstraintStudentsMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY;
}

ConstraintStudentsMaxGapsPerDay::ConstraintStudentsMaxGapsPerDay(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY;
	this->maxGaps=mg;
}

bool ConstraintStudentsMaxGapsPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxGapsPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxGapsPerDay>\n";
	return s;
}

QString ConstraintStudentsMaxGapsPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Students max gaps per day");s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MG:%1", "Max gaps (per day)").arg(this->maxGaps);

	return begin+s+end;
}

QString ConstraintStudentsMaxGapsPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("All students must respect the maximum number of gaps per day");s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per day=%1").arg(this->maxGaps);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//returns a number equal to the number of gaps of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int nGaps;
	int tmp;
	int i;
	
	int tIllegalGaps=0;

	for(i=0; i<r.nInternalSubgroups; i++){
		for(int j=0; j<r.nDaysPerWeek; j++){
			nGaps=0;
	
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					nGaps+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		
			int illegalGaps=nGaps-this->maxGaps;
			if(illegalGaps<0)
				illegalGaps=0;

			if(illegalGaps>0 && conflictsString!=NULL){
				QString s=tr("Time constraint students max gaps per day broken for subgroup: %1, it has %2 extra gaps, on day %3, conflicts increase=%4")
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg(illegalGaps)
				 .arg(r.daysOfTheWeek[j])
				 .arg(CustomFETString::number(illegalGaps*weightPercentage/100));
							 
				dl.append(s);
				cl.append(illegalGaps*weightPercentage/100);
					
				*conflictsString+= s+"\n";
			}
		
			tIllegalGaps+=illegalGaps;
		}
	}
		
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //for partial solutions it might be broken
			assert(tIllegalGaps==0);
	return weightPercentage/100 * tIllegalGaps;
}

bool ConstraintStudentsMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxGapsPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxGapsPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMaxGapsPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nHoursPerDay)
		maxGaps=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxGapsPerDay::ConstraintStudentsSetMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY;
}

ConstraintStudentsSetMaxGapsPerDay::ConstraintStudentsSetMaxGapsPerDay(double wp, int mg, const QString& st )
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY;
	this->maxGaps=mg;
	this->students = st;
}

bool ConstraintStudentsSetMaxGapsPerDay::computeInternalStructure(QWidget* parent, Rules& r){
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);

	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max gaps per day is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxGapsPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+CustomFETString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Students>"; s+=protect(this->students); s+="</Students>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxGapsPerDay>\n";
	return s;
}

QString ConstraintStudentsSetMaxGapsPerDay::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s;
	s+="! ";
	s+=tr("Students set max gaps per day"); s+=", ";
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage)); s+=", ";
	s+=tr("MG:%1", "Max gaps (per day)").arg(this->maxGaps);s+=", ";
	s+=tr("St:%1", "Students").arg(this->students);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxGapsPerDay::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("(not perfect)", "It refers to a not perfect constraint"); s+="\n";
	s+=tr("A students set must respect the maximum number of gaps per day");s+="\n";
	s+=tr("(breaks and students set not available not counted)");s+="\n";
	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum gaps per day=%1").arg(this->maxGaps);s+="\n";
	s+=tr("Students=%1").arg(this->students); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintStudentsSetMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//OLD COMMENT
	//returns a number equal to the number of gaps of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}
	
	int nGaps;
	int tmp;
	
	int tIllegalGaps=0;
	
	for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
		int i=this->iSubgroupsList.at(sg);
		for(int j=0; j<r.nDaysPerWeek; j++){
			nGaps=0;
	
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					nGaps+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		
			int illegalGaps=nGaps-this->maxGaps;
			if(illegalGaps<0)
				illegalGaps=0;

			if(illegalGaps>0 && conflictsString!=NULL){
				QString s=tr("Time constraint students set max gaps per day broken for subgroup: %1, extra gaps=%2, on day %3, conflicts increase=%4")
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg(illegalGaps)
				 .arg(r.daysOfTheWeek[j])
				 .arg(CustomFETString::number(weightPercentage/100*illegalGaps));
							 
				dl.append(s);
				cl.append(weightPercentage/100*illegalGaps);
					
				*conflictsString+= s+"\n";
			}
		
			tIllegalGaps+=illegalGaps;
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)     //for partial solutions it might be broken
			assert(tIllegalGaps==0);
	return weightPercentage/100 * tIllegalGaps;
}

bool ConstraintStudentsSetMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMaxGapsPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxGaps>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxGapsPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsSetMaxGapsPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxGaps>r.nHoursPerDay)
		maxGaps=r.nHoursPerDay;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::ConstraintActivitiesOccupyMaxTimeSlotsFromSelection()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION;
}

ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::ConstraintActivitiesOccupyMaxTimeSlotsFromSelection(double wp,
	QList<int> a_L, QList<int> d_L, QList<int> h_L, int max_slots)
	: TimeConstraint(wp)
{
	assert(d_L.count()==h_L.count());

	this->activitiesIds=a_L;
	this->selectedDays=d_L;
	this->selectedHours=h_L;
	this->maxOccupiedTimeSlots=max_slots;
	
	this->type=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	_activitiesIndices.clear();
	foreach(int id, activitiesIds){
		int i=r.activitiesHash.value(id, -1);
		if(i>=0)
			_activitiesIndices.append(i);
	}

	/*this->_activitiesIndices.clear();
	
	QSet<int> req=this->activitiesIds.toSet();
	assert(req.count()==this->activitiesIds.count());
	
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	int i;
	for(i=0; i<r.nInternalActivities; i++)
		if(req.contains(r.internalActivitiesList[i].id))
			this->_activitiesIndices.append(i);*/
			
	//////////////////////
	assert(this->selectedDays.count()==this->selectedHours.count());
	
	for(int k=0; k<this->selectedDays.count(); k++){
		if(this->selectedDays.at(k) >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities occupy max time slots from selection is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedHours.at(k) == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities occupy max time slots from selection is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedHours.at(k) > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities occupy max time slots from selection is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedDays.at(k)<0 || this->selectedHours.at(k)<0){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities occupy max time slots from selection is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->_activitiesIndices.count()>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::hasInactiveActivities(Rules& r)
{
	//returns true if all activities are inactive
	
	foreach(int aid, this->activitiesIds)
		if(!r.inactiveActivities.contains(aid))
			return false;

	return true;
}

QString ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::getXmlDescription(Rules& r)
{
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString s="<ConstraintActivitiesOccupyMaxTimeSlotsFromSelection>\n";
	
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Number_of_Activities>"+CustomFETString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	foreach(int aid, this->activitiesIds)
		s+="	<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+="	<Number_of_Selected_Time_Slots>"+CustomFETString::number(this->selectedDays.count())+"</Number_of_Selected_Time_Slots>\n";
	for(int i=0; i<this->selectedDays.count(); i++){
		s+="	<Selected_Time_Slot>\n";
		s+="		<Selected_Day>"+protect(r.daysOfTheWeek[this->selectedDays.at(i)])+"</Selected_Day>\n";
		s+="		<Selected_Hour>"+protect(r.hoursOfTheDay[this->selectedHours.at(i)])+"</Selected_Hour>\n";
		s+="	</Selected_Time_Slot>\n";
	}
	s+="	<Max_Number_of_Occupied_Time_Slots>"+CustomFETString::number(this->maxOccupiedTimeSlots)+"</Max_Number_of_Occupied_Time_Slots>\n";
	
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesOccupyMaxTimeSlotsFromSelection>\n";
	return s;
}

QString ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString timeslots=QString("");
	for(int i=0; i<this->selectedDays.count(); i++)
		timeslots+=r.daysOfTheWeek[selectedDays.at(i)]+QString(" ")+r.hoursOfTheDay[selectedHours.at(i)]+QString(", ");
	timeslots.chop(2);
	
	QString s=tr("Activities occupy max time slots from selection, WP:%1, NA:%2, A: %3, STS: %4, MTS:%5", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list, STS means selected time slots, MTS means max time slots")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(CustomFETString::number(this->activitiesIds.count()))
	 .arg(actids)
	 .arg(timeslots)
	 .arg(CustomFETString::number(this->maxOccupiedTimeSlots));
	
	return begin+s+end;
}

QString ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::getDetailedDescription(Rules& r)
{
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString timeslots=QString("");
	for(int i=0; i<this->selectedDays.count(); i++)
		timeslots+=r.daysOfTheWeek[selectedDays.at(i)]+QString(" ")+r.hoursOfTheDay[selectedHours.at(i)]+QString(", ");
	timeslots.chop(2);
	
	QString s=tr("Time constraint"); s+="\n";
	s+=tr("Activities occupy max time slots from selection"); s+="\n";
	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(CustomFETString::number(this->activitiesIds.count())); s+="\n";
	foreach(int id, this->activitiesIds){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	s+=tr("Selected time slots: %1").arg(timeslots); s+="\n";
	s+=tr("Maximum number of occupied slots from selection=%1").arg(CustomFETString::number(this->maxOccupiedTimeSlots)); s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	///////////////////
	Matrix2D<bool> used;
	used.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			used[d][h]=false;
	
	foreach(int ai, this->_activitiesIndices){
		if(c.times[ai]!=UNALLOCATED_TIME){
			Activity* act=&r.internalActivitiesList[ai];
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<act->duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				used[d][h+dur]=true;
			}
		}
	}

	int cnt=0;
	assert(this->selectedDays.count()==this->selectedHours.count());
	for(int t=0; t<this->selectedDays.count(); t++){
		int d=this->selectedDays.at(t);
		int h=this->selectedHours.at(t);
		
		if(used[d][h])
			cnt++;
	}

	nbroken=0;
	
	if(cnt > this->maxOccupiedTimeSlots){
		nbroken=1;
	
		if(conflictsString!=NULL){
			QString s=tr("Time constraint %1 broken - this should not happen, as this kind of constraint should "
			 "have only 100.0% weight. Please report error!").arg(this->getDescription(r));
			
			dl.append(s);
			cl.append(weightPercentage/100.0);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

void ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::removeUseless(Rules& r)
{
	/*QSet<int> validActs;
	
	foreach(Activity* act, r.activitiesList)
		validActs.insert(act->id);*/
		
	QList<int> newActs;
	
	foreach(int aid, activitiesIds){
		Activity* act=r.activitiesPointerHash.value(aid, NULL);
		if(act!=NULL)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
			
	activitiesIds=newActs;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::hasWrongDayOrHour(Rules& r)
{
	assert(selectedDays.count()==selectedHours.count());
	
	for(int i=0; i<selectedDays.count(); i++)
		if(selectedDays.at(i)<0 || selectedDays.at(i)>=r.nDaysPerWeek
		 || selectedHours.at(i)<0 || selectedHours.at(i)>=r.nHoursPerDay)
			return true;
			
	if(maxOccupiedTimeSlots>r.nDaysPerWeek*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivitiesOccupyMaxTimeSlotsFromSelection::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(selectedDays.count()==selectedHours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<selectedDays.count(); i++)
		if(selectedDays.at(i)>=0 && selectedDays.at(i)<r.nDaysPerWeek
		 && selectedHours.at(i)>=0 && selectedHours.at(i)<r.nHoursPerDay){
			newDays.append(selectedDays.at(i));
			newHours.append(selectedHours.at(i));
		}
	
	selectedDays=newDays;
	selectedHours=newHours;
	
	if(maxOccupiedTimeSlots>r.nDaysPerWeek*r.nHoursPerDay)
		maxOccupiedTimeSlots=r.nDaysPerWeek*r.nHoursPerDay;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS;
}

ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots(double wp,
	QList<int> a_L, QList<int> d_L, QList<int> h_L, int max_simultaneous)
	: TimeConstraint(wp)
{
	assert(d_L.count()==h_L.count());

	this->activitiesIds=a_L;
	this->selectedDays=d_L;
	this->selectedHours=h_L;
	this->maxSimultaneous=max_simultaneous;
	
	this->type=CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	_activitiesIndices.clear();
	foreach(int id, activitiesIds){
		int i=r.activitiesHash.value(id, -1);
		if(i>=0)
			_activitiesIndices.append(i);
	}

	/*this->_activitiesIndices.clear();
	
	QSet<int> req=this->activitiesIds.toSet();
	assert(req.count()==this->activitiesIds.count());
	
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	int i;
	for(i=0; i<r.nInternalActivities; i++)
		if(req.contains(r.internalActivitiesList[i].id))
			this->_activitiesIndices.append(i);*/
			
	//////////////////////
	assert(this->selectedDays.count()==this->selectedHours.count());
	
	for(int k=0; k<this->selectedDays.count(); k++){
		if(this->selectedDays.at(k) >= r.nDaysPerWeek){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities max simultaneous in selected time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedHours.at(k) == r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities max simultaneous in selected time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedHours.at(k) > r.nHoursPerDay){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities max simultaneous in selected time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->selectedDays.at(k)<0 || this->selectedHours.at(k)<0){
			TimeConstraintIrreconcilableMessage::information(parent, tr("FET information"),
			 tr("Constraint activities max simultaneous in selected time slots is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->_activitiesIndices.count()>0)
		return true;
	else{
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"), 
			tr("Following constraint is wrong (refers to no activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::hasInactiveActivities(Rules& r)
{
	//returns true if all activities are inactive
	
	foreach(int aid, this->activitiesIds)
		if(!r.inactiveActivities.contains(aid))
			return false;

	return true;
}

QString ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::getXmlDescription(Rules& r)
{
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString s="<ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots>\n";
	
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+="	<Number_of_Activities>"+CustomFETString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	foreach(int aid, this->activitiesIds)
		s+="	<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+="	<Number_of_Selected_Time_Slots>"+CustomFETString::number(this->selectedDays.count())+"</Number_of_Selected_Time_Slots>\n";
	for(int i=0; i<this->selectedDays.count(); i++){
		s+="	<Selected_Time_Slot>\n";
		s+="		<Selected_Day>"+protect(r.daysOfTheWeek[this->selectedDays.at(i)])+"</Selected_Day>\n";
		s+="		<Selected_Hour>"+protect(r.hoursOfTheDay[this->selectedHours.at(i)])+"</Selected_Hour>\n";
		s+="	</Selected_Time_Slot>\n";
	}
	s+="	<Max_Number_of_Simultaneous_Activities>"+CustomFETString::number(this->maxSimultaneous)+"</Max_Number_of_Simultaneous_Activities>\n";
	
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots>\n";
	return s;
}

QString ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString timeslots=QString("");
	for(int i=0; i<this->selectedDays.count(); i++)
		timeslots+=r.daysOfTheWeek[selectedDays.at(i)]+QString(" ")+r.hoursOfTheDay[selectedHours.at(i)]+QString(", ");
	timeslots.chop(2);
	
	QString s=tr("Activities max simultaneous in selected time slots, WP:%1, NA:%2, A: %3, STS: %4, MS:%5", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list, STS means selected time slots, MS means max simultaneous (number of activities in each selected time slot)")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(CustomFETString::number(this->activitiesIds.count()))
	 .arg(actids)
	 .arg(timeslots)
	 .arg(CustomFETString::number(this->maxSimultaneous));
	
	return begin+s+end;
}

QString ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::getDetailedDescription(Rules& r)
{
	assert(this->selectedDays.count()==this->selectedHours.count());

	QString actids=QString("");
	foreach(int aid, this->activitiesIds)
		actids+=CustomFETString::number(aid)+QString(", ");
	actids.chop(2);
		
	QString timeslots=QString("");
	for(int i=0; i<this->selectedDays.count(); i++)
		timeslots+=r.daysOfTheWeek[selectedDays.at(i)]+QString(" ")+r.hoursOfTheDay[selectedHours.at(i)]+QString(", ");
	timeslots.chop(2);
	
	QString s=tr("Time constraint"); s+="\n";
	s+=tr("Activities max simultaneous in selected time slots"); s+="\n";
	s+=tr("Weight (percentage)=%1").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(CustomFETString::number(this->activitiesIds.count())); s+="\n";
	foreach(int id, this->activitiesIds){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	s+=tr("Selected time slots: %1").arg(timeslots); s+="\n";
	s+=tr("Maximum number of simultaneous activities in each selected time slot=%1").arg(CustomFETString::number(this->maxSimultaneous)); s+="\n";
	
	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

///////////////////

	Matrix2D<int> count;
	count.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			count[d][h]=0;
	
	foreach(int ai, this->_activitiesIndices){
		if(c.times[ai]!=UNALLOCATED_TIME){
			Activity* act=&r.internalActivitiesList[ai];
			int d=c.times[ai]%r.nDaysPerWeek;
			int h=c.times[ai]/r.nDaysPerWeek;
			for(int dur=0; dur<act->duration; dur++){
				assert(h+dur<r.nHoursPerDay);
				count[d][h+dur]++;
			}
		}
	}

	nbroken=0;

	assert(this->selectedDays.count()==this->selectedHours.count());
	for(int t=0; t<this->selectedDays.count(); t++){
		int d=this->selectedDays.at(t);
		int h=this->selectedHours.at(t);
		
		if(count[d][h] > this->maxSimultaneous)
			nbroken++;
	}

	if(nbroken>0){
		if(conflictsString!=NULL){
			QString s=tr("Time constraint %1 broken - this should not happen, as this kind of constraint should "
			 "have only 100.0% weight. Please report error!").arg(this->getDescription(r));
			
			dl.append(s);
			cl.append(weightPercentage/100.0);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return nbroken * weightPercentage / 100.0;
}

void ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::removeUseless(Rules& r)
{
	/*QSet<int> validActs;
	
	foreach(Activity* act, r.activitiesList)
		validActs.insert(act->id);*/
		
	QList<int> newActs;
	
	foreach(int aid, activitiesIds){
		Activity* act=r.activitiesPointerHash.value(aid, NULL);
		if(act!=NULL)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
			
	activitiesIds=newActs;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::hasWrongDayOrHour(Rules& r)
{
	assert(selectedDays.count()==selectedHours.count());
	
	for(int i=0; i<selectedDays.count(); i++)
		if(selectedDays.at(i)<0 || selectedDays.at(i)>=r.nDaysPerWeek
		 || selectedHours.at(i)<0 || selectedHours.at(i)>=r.nHoursPerDay)
			return true;

	//Do not care about maxSimultaneous, which can be as high as MAX_ACTIVITIES

	return false;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(selectedDays.count()==selectedHours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<selectedDays.count(); i++)
		if(selectedDays.at(i)>=0 && selectedDays.at(i)<r.nDaysPerWeek
		 && selectedHours.at(i)>=0 && selectedHours.at(i)<r.nHoursPerDay){
			newDays.append(selectedDays.at(i));
			newHours.append(selectedHours.at(i));
		}
	
	selectedDays=newDays;
	selectedHours=newHours;

	//Do not modify maxSimultaneous, which can be as high as MAX_ACTIVITIES
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxDaysPerWeek::ConstraintStudentsSetMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsSetMaxDaysPerWeek::ConstraintStudentsSetMaxDaysPerWeek(double wp, int maxnd, QString sn)
	 : TimeConstraint(wp)
{
	this->students = sn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK;
}

bool ConstraintStudentsSetMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	StudentsSet* ss=r.studentsHash.value(students, NULL);

	if(ss==NULL){
		TimeConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max days per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			if(!this->iSubgroupsList.contains(tmp))
				this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsSetMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Students set max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("St:%1", "Abbreviation for students (sets)").arg(this->students);s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("A students set must respect the maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Students set=%1").arg(this->students);s+="\n";

	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= tr("Time constraint students set max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number((nOcDays-this->maxDaysPerWeek)*weightPercentage/100));
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsSetMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(this->students, s->name);
}

bool ConstraintStudentsSetMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxDaysPerWeek::ConstraintStudentsMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsMaxDaysPerWeek::ConstraintStudentsMaxDaysPerWeek(double wp, int maxnd)
	 : TimeConstraint(wp)
{
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK;
}

bool ConstraintStudentsMaxDaysPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Days_Per_Week>"+CustomFETString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="	<Active>"+trueFalse(active)+"</Active>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</ConstraintStudentsMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
		
	QString s=tr("Students max days per week");s+=", ";
	s+=tr("WP:%1%", "Abbreviation for weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=", ";
	s+=tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Time constraint");s+="\n";
	s+=tr("All students must respect the maximum number of days per week");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Maximum days per week=%1").arg(this->maxDaysPerWeek);s+="\n";

	if(!active){
		s+=tr("Active=%1", "Refers to a constraint").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= tr("Time constraint students max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::number((nOcDays-this->maxDaysPerWeek)*weightPercentage/100));
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return true;
}

bool ConstraintStudentsMaxDaysPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxDaysPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	return true;
}

bool ConstraintStudentsMaxDaysPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(this->maxDaysPerWeek>r.nDaysPerWeek)
		this->maxDaysPerWeek=r.nDaysPerWeek;

	return true;
}
