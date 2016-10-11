/*
File timetableexport.cpp
*/

/***************************************************************************
                          timetableexport.cpp  -  description
                          -------------------
    begin                : Tue Apr 22 2003
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

//**********************************************************************************************************************/
//August 2007
//XHTML generation code by Volker Dirr (timetabling.de)
//Features:   - XHTML 1.0 strict valid
//            - using colspan and rowspan
//            - table of contents with hyperlinks
//            - CSS and JavaScript support
//            - index HTML file
//            - TIMETABLE_HTML_LEVEL
//            - days/time horizontal/vertical
//            - subgroups, groups, years, teachers, rooms, subjects, activities, activity tags timetable
//            - teachers free periods
//            - daily timetable
//            - activities with same starting time
//            - reorganized functions. now they can be also used for printing
//            - split times tables after X names (TIMETABLE_HTML_SPLIT?) and choose if activity tags should be printed (TIMETABLE_HTML_PRINT_ACTIVITY_TAGS)
//            - teachers and students statistics (gaps, free days, hours)

//TODO: all must be internal here. so maybe also do daysOfTheWeek and hoursPerDay also internal
//maybe TODO: use back_odd and back_even (or back0 and back1, because easier to code!) like in printing. so don't use the table_odd and table_even anymore
//maybe TODO: make TIMETABLE_HTML_SPLIT? (similar to TIMETABLE_HTML_LEVEL)
//maybe TODO: rename augmentedYearsList into internalYearsList to have it similar to others?
//maybe TODO: some "stg" stuff can be replaced by gt.rules.internalGroupsList. I don't want to do that now, because id-s will change. That is not critical, but I want to diff tables with old release.

#include "timetable_defs.h"
#include "timetable.h"
#include "timetableexport.h"
#include "solution.h"

#include "matrix.h"

#include <iostream>
using namespace std;

#include <QString>
#include <QTextStream>
#include <QFile>

#include <QList>

#include <QHash>

//#include <QDesktopWidget>

#include "messageboxes.h"

#include <QLocale>
#include <QTime>
#include <QDate>

#include <QDir>

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;
extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;
extern QString LANGUAGE_FOR_HTML;

extern Timetable gt;
/*extern qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
extern Matrix3D<int> teachers_timetable_weekly;
extern Matrix3D<int> students_timetable_weekly;
extern Matrix3D<int> rooms_timetable_weekly;

//extern QList<qint16> teachers_free_periods_timetable_weekly[TEACHERS_FREE_PERIODS_N_CATEGORIES][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix3D<QList<int> > teachers_free_periods_timetable_weekly;

//extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix2D<bool> breakDayHour;
/*extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK];
extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];*/
extern Matrix3D<bool> teacherNotAvailableDayHour;
extern Matrix2D<double> notAllowedRoomTimePercentages;
extern Matrix3D<bool> subgroupNotAvailableDayHour;

static QList<int> activitiesForCurrentSubject[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static QList<int> activitiesForCurrentActivityTag[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static QList<int> activitiesAtTime[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern Rules rules2;

const QString STRING_EMPTY_SLOT="---";

const QString STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES="???";

const QString STRING_NOT_AVAILABLE_TIME_SLOT="-x-";

const QString STRING_BREAK_SLOT="-X-";

//these hashes are needed to get the IDs for html and css in timetableexport and statistics
static QHash<QString, QString> hashSubjectIDsTimetable;
static QHash<QString, QString> hashActivityTagIDsTimetable;
static QHash<QString, QString> hashStudentIDsTimetable;
static QHash<QString, QString> hashTeacherIDsTimetable;
static QHash<QString, QString> hashRoomIDsTimetable;
static QHash<QString, QString> hashDayIDsTimetable;

//static QHash<QString, QString> hashColorStringIDsTimetable;
QHash<int, int> hashActivityColorBySubject;
QList<int> activeHashActivityColorBySubject;
QHash<int, int> hashActivityColorBySubjectAndStudents;
QList<int> activeHashActivityColorBySubjectAndStudents;
const int COLOR_BY_SUBJECT=1;
const int COLOR_BY_SUBJECT_STUDENTS=2;

//this hash is needed to care about sctivities with same starting time
static QHash<int, QList<int> >activitiesWithSameStartingTime;

//Now the filenames of the output files are following (for xml and all html tables)
const QString SUBGROUPS_TIMETABLE_FILENAME_XML="subgroups.xml";
const QString TEACHERS_TIMETABLE_FILENAME_XML="teachers.xml";
const QString ACTIVITIES_TIMETABLE_FILENAME_XML="activities.xml";
const QString ROOMS_TIMETABLE_FILENAME_XML="rooms.xml";

const QString CONFLICTS_FILENAME="soft_conflicts.txt";
const QString INDEX_HTML="index.html";
const QString STYLESHEET_CSS="stylesheet.css";

const QString SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subgroups_days_horizontal.html";
const QString SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subgroups_days_vertical.html";
const QString SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subgroups_time_horizontal.html";
const QString SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subgroups_time_vertical.html";

const QString GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="groups_days_horizontal.html";
const QString GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="groups_days_vertical.html";
const QString GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="groups_time_horizontal.html";
const QString GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="groups_time_vertical.html";

const QString YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="years_days_horizontal.html";
const QString YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="years_days_vertical.html";
const QString YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="years_time_horizontal.html";
const QString YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="years_time_vertical.html";

const QString TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_days_horizontal.html";
const QString TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_days_vertical.html";
const QString TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="teachers_time_horizontal.html";
const QString TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="teachers_time_vertical.html";

const QString ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="rooms_days_horizontal.html";
const QString ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="rooms_days_vertical.html";
const QString ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="rooms_time_horizontal.html";
const QString ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="rooms_time_vertical.html";

const QString SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subjects_days_horizontal.html";
const QString SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subjects_days_vertical.html";
const QString SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subjects_time_horizontal.html";
const QString SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subjects_time_vertical.html";

const QString ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="activity_tags_days_horizontal.html";
const QString ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="activity_tags_days_vertical.html";
const QString ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="activity_tags_time_horizontal.html";
const QString ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="activity_tags_time_vertical.html";

const QString ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="activities_days_horizontal.html";
const QString ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="activities_days_vertical.html";
const QString ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="activities_time_horizontal.html";
const QString ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="activities_time_vertical.html";

const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_free_periods_days_horizontal.html";
const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_free_periods_days_vertical.html";

const QString TEACHERS_STATISTICS_FILENAME_HTML="teachers_statistics.html";
const QString STUDENTS_STATISTICS_FILENAME_HTML="students_statistics.html";

const QString MULTIPLE_TIMETABLE_DATA_RESULTS_FILE="data_and_timetable.fet";

//now the XML tags used for identification of the output file (is that comment correct? it's the old comment)
const QString STUDENTS_TIMETABLE_TAG="Students_Timetable";
const QString TEACHERS_TIMETABLE_TAG="Teachers_Timetable";
const QString ACTIVITIES_TIMETABLE_TAG="Activities_Timetable";
const QString ROOMS_TIMETABLE_TAG="Rooms_Timetable";

const QString RANDOM_SEED_FILENAME_BEFORE="random_seed_before.txt";
const QString RANDOM_SEED_FILENAME_AFTER="random_seed_after.txt";

extern int XX;
extern int YY;

QString generationLocalizedTime=QString(""); //to be used in timetableprintform.cpp

//similar to code from Marco Vassura, modified by Volker Dirr to get rid of QColor and QBrush, since they need QtGui.
//The command-line version does not have access to QtGui.
void TimetableExport::stringToColor(QString s, int *r, int *g, int *b)
{
	// CRC-24 Based on RFC 2440 Section 6.1
	unsigned long crc = 0xB704CEL;
	int i;
	QChar *data = s.data();
	while (!data->isNull()) {
		crc ^= (data->unicode() & 0xFF) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000)
				crc ^= 0x1864CFBL;
		}
		data++;
	}
	*r=(crc>>16);
	*g=((crc>>8) & 0xFF);
	*b=(crc & 0xFF);
}
//similar to code from Marco Vassura, modified by Volker Dirr

bool writeAtLeastATimetable()
{
	bool t = WRITE_TIMETABLE_CONFLICTS ||
	
	 (WRITE_TIMETABLES_STATISTICS &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_GROUPS ||
	 WRITE_TIMETABLES_YEARS ||
	 WRITE_TIMETABLES_TEACHERS)) ||
	
	 (WRITE_TIMETABLES_XML &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_TEACHERS ||
	 WRITE_TIMETABLES_ACTIVITIES)) ||
	
	 ((WRITE_TIMETABLES_DAYS_HORIZONTAL ||
	 WRITE_TIMETABLES_DAYS_VERTICAL ||
	 WRITE_TIMETABLES_TIME_HORIZONTAL ||
	 WRITE_TIMETABLES_TIME_VERTICAL) &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_GROUPS ||
	 WRITE_TIMETABLES_YEARS ||
	 WRITE_TIMETABLES_TEACHERS ||
	 WRITE_TIMETABLES_ROOMS ||
	 WRITE_TIMETABLES_SUBJECTS ||
	 WRITE_TIMETABLES_ACTIVITY_TAGS ||
	 WRITE_TIMETABLES_ACTIVITIES)) ||
	
	 ((WRITE_TIMETABLES_DAYS_HORIZONTAL ||
	 WRITE_TIMETABLES_DAYS_VERTICAL) &&
	 WRITE_TIMETABLES_TEACHERS_FREE_PERIODS);
	
	 return t;
}

TimetableExport::TimetableExport()
{
}

TimetableExport::~TimetableExport()
{
}

void TimetableExport::getStudentsTimetable(Solution &c){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	c.getSubgroupsTimetable(gt.rules, students_timetable_weekly);
	best_solution.copy(gt.rules, c);
	students_schedule_ready=true;
}

void TimetableExport::getTeachersTimetable(Solution &c){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	c.getTeachersTimetable(gt.rules, teachers_timetable_weekly, teachers_free_periods_timetable_weekly);
	best_solution.copy(gt.rules, c);
	teachers_schedule_ready=true;
}

void TimetableExport::getRoomsTimetable(Solution &c){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	c.getRoomsTimetable(gt.rules, rooms_timetable_weekly);
	best_solution.copy(gt.rules, c);
	rooms_schedule_ready=true;
}

void TimetableExport::getNumberOfPlacedActivities(int& number1, int& number2)
{
	number1=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			number1++;

	number2=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.rooms[i]!=UNALLOCATED_SPACE)
			number2++;
}

void TimetableExport::writeSimulationResults(QWidget* parent){
	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-single");
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	//now write the solution in xml files
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s);
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;
	
	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since simulation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	
	//indexHtml
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//groups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activty_tags
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);

	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();

	if(VERBOSE){
		cout<<"Writing simulation results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeHighestStageResults(QWidget* parent){
	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-highest");
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	//now write the solution in xml files
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s);
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;
	
	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since simulation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	
	//indexHtml
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//groups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);

	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();

	if(VERBOSE){
		cout<<"Writing highest stage results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeRandomSeed(QWidget* parent, bool before)
{
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-single");
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);

	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+RANDOM_SEED_FILENAME;
	
	writeRandomSeedFile(parent, s, before);
}

void TimetableExport::writeRandomSeedFile(QWidget* parent, const QString& filename, bool before)
{
	QString s=filename;

	QFile file(s);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(s));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	
	if(before){
		tos<<tr("Generation started on: %1", "%1 is the time").arg(sTime);
		tos<<endl<<endl;
		tos<<tr("Random seed at the start of generation is: X=%1, Y=%2", "The random seed has two components, X and Y").arg(XX).arg(YY);
		tos<<endl<<endl;
		tos<<tr("This file was automatically generated by FET %1.").arg(FET_VERSION);
		tos<<endl;
	}
	else{
		tos<<tr("Generation ended on: %1", "%1 is the time").arg(sTime);
		tos<<endl<<endl;
		tos<<tr("Random seed at the end of generation is: X=%1, Y=%2", "The random seed has two components, X and Y").arg(XX).arg(YY);
		tos<<endl<<endl;
		tos<<tr("This file was automatically generated by FET %1.").arg(FET_VERSION);
		tos<<endl;
	}
	
	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(s).arg(file.error()));
	}
	file.close();
}

void TimetableExport::writeTimetableDataFile(QWidget* parent, const QString& filename){
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Timetable not generated - cannot save it - this should not happen (please report bug)"));
		return;
	}

	Solution* tc=&best_solution;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time==UNALLOCATED_TIME){
			IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;
		}
		
		int ri=tc->rooms[ai];
		if(ri==UNALLOCATED_SPACE){
			IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;
		}
	}
	
	rules2.initialized=true;
	
	rules2.institutionName=gt.rules.institutionName;
	rules2.comments=gt.rules.comments;
	
	rules2.nHoursPerDay=gt.rules.nHoursPerDay;
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		rules2.hoursOfTheDay[i]=gt.rules.hoursOfTheDay[i];

	rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		rules2.daysOfTheWeek[i]=gt.rules.daysOfTheWeek[i];
		
	rules2.yearsList=gt.rules.yearsList;
	
	rules2.teachersList=gt.rules.teachersList;
	
	rules2.subjectsList=gt.rules.subjectsList;
	
	rules2.activityTagsList=gt.rules.activityTagsList;

	rules2.activitiesList=gt.rules.activitiesList;

	rules2.buildingsList=gt.rules.buildingsList;

	rules2.roomsList=gt.rules.roomsList;

	rules2.timeConstraintsList=gt.rules.timeConstraintsList;
	
	rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;

	rules2.apstHash=gt.rules.apstHash;
	rules2.aprHash=gt.rules.aprHash;
	
	rules2.groupActivitiesInInitialOrderList=gt.rules.groupActivitiesInInitialOrderList;

	//add locking constraints
	TimeConstraintsList lockTimeConstraintsList;
	SpaceConstraintsList lockSpaceConstraintsList;


	//bool report=false;
	
	int addedTime=0, duplicatesTime=0;
	int addedSpace=0, duplicatesSpace=0;

	//lock selected activities
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
			int hour=time/gt.rules.nDaysPerWeek;
			int day=time%gt.rules.nDaysPerWeek;

			ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false); //permanently locked is false
			bool t=rules2.addTimeConstraint(ctr);
						
			if(t){
				addedTime++;
				lockTimeConstraintsList.append(ctr);
			}
			else
				duplicatesTime++;

			QString s;
						
			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			/*if(report){
				int k;
				if(t)
					k=TimetableExportMessage::information(parent, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=TimetableExportMessage::warning(parent, tr("FET warning"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
		 		if(k==0)
					report=false;
			}*/
		}
					
		int ri=tc->rooms[ai];
		if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
			ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, false); //permanently locked is false
			bool t=rules2.addSpaceConstraint(ctr);

			QString s;
						
			if(t){
				addedSpace++;
				lockSpaceConstraintsList.append(ctr);
			}
			else
				duplicatesSpace++;

			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			/*if(report){
				int k;
				if(t)
					k=TimetableExportMessage::information(parent, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=TimetableExportMessage::warning(parent, tr("FET warning"), s,
					 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				if(k==0)
					report=false;
			}*/
		}
	}

	//QMessageBox::information(parent, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
	// " ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace));
		
	bool result=rules2.write(parent, filename);
	
	while(!lockTimeConstraintsList.isEmpty())
		delete lockTimeConstraintsList.takeFirst();
	while(!lockSpaceConstraintsList.isEmpty())
		delete lockSpaceConstraintsList.takeFirst();

	//if(result)
	//	QMessageBox::information(parent, tr("FET information"),
	//		tr("File saved successfully. You can see it on the hard disk. Current data file remained untouched (of locking constraints),"
	//		" so you can save it also, or generate different timetables."));

	rules2.nHoursPerDay=0;
	rules2.nDaysPerWeek=0;

	rules2.yearsList.clear();
	
	rules2.teachersList.clear();
	
	rules2.subjectsList.clear();
	
	rules2.activityTagsList.clear();

	rules2.activitiesList.clear();

	rules2.buildingsList.clear();

	rules2.roomsList.clear();

	rules2.timeConstraintsList.clear();
	
	rules2.spaceConstraintsList.clear();

	rules2.apstHash.clear();
	rules2.aprHash.clear();
	
	rules2.groupActivitiesInInitialOrderList.clear();

	if(!result){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"), tr("Could not save the data + timetable file on the hard disk - maybe hard disk is full"));
	}
}

void TimetableExport::writeSimulationResults(QWidget* parent, int n){
	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
		
	QString finalDestDir=destDir+FILE_SEP+CustomFETString::number(n);

	if(!dir.exists(finalDestDir))
		dir.mkpath(finalDestDir);
		
	finalDestDir+=FILE_SEP;


	QString s3=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s3.right(4)==".fet")
		s3=s3.left(s3.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	finalDestDir+=s3+"_";
	
	//write data+timetable in .fet format
	writeTimetableDataFile(parent, finalDestDir+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE);

	//now write the solution in xml files
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s);
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=finalDestDir+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;

	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	//write the conflicts in txt mode
	s=finalDestDir+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=finalDestDir+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	//indexHtml
	s=finalDestDir+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//groups
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=finalDestDir+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=finalDestDir+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=finalDestDir+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);
	
	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();

	if(VERBOSE){
		cout<<"Writing multiple simulation results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeRandomSeed(QWidget* parent, int n, bool before){
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
		
	QString finalDestDir=destDir+FILE_SEP+CustomFETString::number(n);

	if(!dir.exists(finalDestDir))
		dir.mkpath(finalDestDir);
		
	finalDestDir+=FILE_SEP;

	QString s3=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s3.right(4)==".fet")
		s3=s3.left(s3.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	finalDestDir+=s3+"_";
	
	s=finalDestDir+RANDOM_SEED_FILENAME;

	writeRandomSeedFile(parent, s, before);
}

void TimetableExport::writeReportForMultiple(QWidget* parent, const QString& description, bool begin)
{
	QDir dir;
	
	QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
		
	QString filename=destDir+FILE_SEP+QString("report.txt");

	QFile file(filename);
	if(!file.open(QIODevice::Append)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(filename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	if(begin){
		tos.setGenerateByteOrderMark(true);
	}
	
	tos<<description<<endl;
	
	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(filename).arg(file.error()));
	}
	file.close();
}

void TimetableExport::writeSimulationResultsCommandLine(QWidget* parent, const QString& outputDirectory){ //outputDirectory contains trailing FILE_SEP
	QString add=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(add.right(4)==".fet")
		add=add.left(add.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	if(add!="")
		add.append("_");

	/////////

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	TimetableExport::writeSubgroupsTimetableXml(parent, outputDirectory+add+SUBGROUPS_TIMETABLE_FILENAME_XML);
	TimetableExport::writeTeachersTimetableXml(parent, outputDirectory+add+TEACHERS_TIMETABLE_FILENAME_XML);
	TimetableExport::writeActivitiesTimetableXml(parent, outputDirectory+add+ACTIVITIES_TIMETABLE_FILENAME_XML);
	
	//get the time
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime; //really unneeded, but just to be similar to the other parts
	
	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		QString s=outputDirectory+add+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since simulation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}

	//write the conflicts in txt mode
	QString s=add+CONFLICTS_FILENAME;
	s.prepend(outputDirectory);
	TimetableExport::writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=add+STYLESHEET_CSS;
		s.prepend(outputDirectory);
		TimetableExport::writeStylesheetCss(parent, s, sTime, na);
	}
	//indexHtml
	s=add+INDEX_HTML;
	s.prepend(outputDirectory);
	writeIndexHtml(parent, s, sTime, na);
	//subgroups
	s=add+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//groups
	s=add+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=add+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=add+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=add+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=add+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=add+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=add+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=add+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=add+TEACHERS_STATISTICS_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=add+STUDENTS_STATISTICS_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeStudentsStatisticsHtml(parent, s, sTime, na);

	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
}

void TimetableExport::writeRandomSeedCommandLine(QWidget* parent, const QString& outputDirectory, bool before){ //outputDirectory contains trailing FILE_SEP
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QString add=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(add.right(4)==".fet")
		add=add.left(add.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	if(add!="")
		add.append("_");

	QString s=add+RANDOM_SEED_FILENAME;
	s.prepend(outputDirectory);
	
	writeRandomSeedFile(parent, s, before);
}

//by Volker Dirr (timetabling.de)
void TimetableExport::writeConflictsTxt(QWidget* parent, const QString& filename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLE_CONFLICTS){
		if(QFile::exists(filename))
			QFile::remove(filename);
	
		return;
	}

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(filename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	
	if(placedActivities==gt.rules.nInternalActivities){
		QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		if(INPUT_FILENAME_XML=="")
			tt=tr("unnamed");
		tos<<TimetableExport::tr("Soft conflicts of %1", "%1 is the file name").arg(tt);
		tos<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Number of broken soft constraints: %1").arg(best_solution.conflictsWeightList.count())<<endl;
		tos<<TimetableExport::tr("Total soft conflicts: %1").arg(CustomFETString::number(best_solution.conflictsTotal))<<endl<<endl;
		tos<<TimetableExport::tr("Soft conflicts list (in decreasing order):")<<endl<<endl;
		foreach(QString t, best_solution.conflictsDescriptionList)
			tos<<t<<endl;
		tos<<endl<<TimetableExport::tr("End of file.")<<"\n";
	}
	else{
		QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		if(INPUT_FILENAME_XML=="")
			tt=tr("unnamed");
		tos<<TimetableExport::tr("Conflicts of %1").arg(tt);
		tos<<"\n";
		tos<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Number of broken constraints: %1").arg(best_solution.conflictsWeightList.count())<<endl;
		tos<<TimetableExport::tr("Total conflicts: %1").arg(CustomFETString::number(best_solution.conflictsTotal))<<endl<<endl;
		tos<<TimetableExport::tr("Conflicts list (in decreasing order):")<<endl<<endl;
		foreach(QString t, best_solution.conflictsDescriptionList)
			tos<<t<<endl;
		tos<<endl<<TimetableExport::tr("End of file.")<<"\n";
	}
	
	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(filename).arg(file.error()));
	}
	file.close();
}

void TimetableExport::writeSubgroupsTimetableXml(QWidget* parent, const QString& xmlfilename){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(xmlfilename))
			QFile::remove(xmlfilename);
	
		return;
	}

	//Now we print the results to an XML file
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos<<"<"<<protect(STUDENTS_TIMETABLE_TAG)<<">\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<< "  <Subgroup name=\"";
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<< protect(subgroup_name) << "\">\n";

		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			tos<<"   <Day name=\""<<protect(gt.rules.daysOfTheWeek[day])<<"\">\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[hour]) << "\">\n";
				tos<<"     ";
				int ai=students_timetable_weekly[subgroup][day][hour]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<" <Teacher name=\""<<protect(*it)<<"\"></Teacher>";
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					foreach(QString atn, act->activityTagsNames)
						tos<<"<Activity_Tag name=\""<<protect(atn)<<"\"></Activity_Tag>";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"<Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>";
					}
				}
				tos<<"\n";
				tos << "    </Hour>\n";
			}
			tos<<"   </Day>\n";
		}
		tos<<"  </Subgroup>\n";
	}

	tos << "</" << protect(STUDENTS_TIMETABLE_TAG) << ">\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(xmlfilename).arg(file.error()));
	}
	file.close();
}

void TimetableExport::writeTeachersTimetableXml(QWidget* parent, const QString& xmlfilename){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(xmlfilename))
			QFile::remove(xmlfilename);
	
		return;
	}

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos << "<" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos << "  <Teacher name=\"" << protect(gt.rules.internalTeachersList[i]->name) << "\">\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			tos << "   <Day name=\"" << protect(gt.rules.daysOfTheWeek[day]) << "\">\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[hour]) << "\">\n";

				tos<<"     ";
				int ai=teachers_timetable_weekly[i][day][hour]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					foreach(QString atn, act->activityTagsNames)
						tos<<"<Activity_Tag name=\""<<protect(atn)<<"\"></Activity_Tag>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"<Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>";
					}
				}
				tos<<"\n";
				tos << "    </Hour>\n";
			}
			tos << "   </Day>\n";
		}
		tos<<"  </Teacher>\n";
	}

	tos << "</" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(xmlfilename).arg(file.error()));
	}
	file.close();
}

void TimetableExport::writeActivitiesTimetableXml(QWidget* parent, const QString& xmlfilename){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(xmlfilename))
			QFile::remove(xmlfilename);

		return;
	}

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos << "<" << protect(ACTIVITIES_TIMETABLE_TAG) << ">\n";
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		tos<<"<Activity>"<<endl;
		
		tos<<"	<Id>"<<gt.rules.internalActivitiesList[i].id<<"</Id>"<<endl;
		
		QString day="";
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			day=gt.rules.daysOfTheWeek[d];
		}
		tos<<"	<Day>"<<protect(day)<<"</Day>"<<endl;
		
		QString hour="";
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			hour=gt.rules.hoursOfTheDay[h];
		}
		tos<<"	<Hour>"<<protect(hour)<<"</Hour>"<<endl;
		
		QString room="";
		if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
			int r=best_solution.rooms[i];
			room=gt.rules.internalRoomsList[r]->name;
		}
		tos<<"	<Room>"<<protect(room)<<"</Room>"<<endl;
		
		tos<<"</Activity>"<<endl;
	}

	tos << "</" << protect(ACTIVITIES_TIMETABLE_TAG) << ">\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(xmlfilename).arg(file.error()));
	}
	file.close();
}

// writing the index html file by Volker Dirr.
void TimetableExport::writeIndexHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	bool _writeAtLeastATimetable = writeAtLeastATimetable();

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(false, placedActivities, true);

	QString bar;
	QString s2="";
	if(INPUT_FILENAME_XML=="")
		bar="";
	else{
		bar="_";
		s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

		if(s2.right(4)==".fet")
			s2=s2.left(s2.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	tos<<"    <p>\n";
	
	if(!_writeAtLeastATimetable){
		tos<<"      "<<TimetableExport::tr("No timetable was written, because from the settings you disabled writing any timetable.")<<"\n";
		tos<<"      "<<TimetableExport::tr("The exception is that after each successful (complete) timetable generation the %1 file"
		 " will be written.").arg("data_and_timetable.fet")<<"\n";
		tos<<"    </p>\n";
		tos<<"    <p>\n";
		tos<<"      "<<TimetableExport::tr("File generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"\n";
		tos<<"    </p>\n";
	}
	else{
		if(WRITE_TIMETABLE_CONFLICTS)
			tos<<"      <a href=\""<<s2+bar+CONFLICTS_FILENAME<<"\">"<<tr("View the soft conflicts list.")<<"</a><br />\n";
		else
			tos<<"      "<<TimetableExport::tr("Soft conflicts list - disabled.")<<"<br />\n";

		///////////////////////////

		QString tmps1, tmps2;
		if(WRITE_TIMETABLES_STATISTICS && (WRITE_TIMETABLES_YEARS || WRITE_TIMETABLES_GROUPS || WRITE_TIMETABLES_SUBGROUPS) )
			tmps1="      <a href=\""+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML+"\">"+tr("students")+"</a>";
		else
			tmps1=tr("students - disabled");
		
		if(WRITE_TIMETABLES_STATISTICS && WRITE_TIMETABLES_TEACHERS)
			tmps2="<a href=\""+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML+"\">"+tr("teachers")+"</a>";
		else
			tmps2=tr("teachers - disabled");
			
		QString tmps3=tr("View statistics: %1, %2.", "%1 and %2 are two files in HTML format, to show statistics for students and teachers. The user can click on one file to view it")
		 .arg(tmps1).arg(tmps2);

		tos<<"      "<<tmps3<<"<br />\n";
		
		///////////////////////////
		
		QString tmp1, tmp2, tmp3;
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_SUBGROUPS)
			tmp1="<a href=\""+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML+"\">"+tr("subgroups")+"</a>";
		else
			tmp1=tr("subgroups - disabled", "It means the subgroups XML timetables are disabled");
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_TEACHERS)
			tmp2="<a href=\""+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML+"\">"+tr("teachers")+"</a>";
		else
			tmp2=tr("teachers - disabled", "It means the teachers XML timetables are disabled");
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_ACTIVITIES)
			tmp3="<a href=\""+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML+"\">"+tr("activities")+"</a>";
		else
			tmp3=tr("activities - disabled", "It means the activities XML timetables are disabled");
		QString tmp4=TimetableExport::tr("View XML: %1, %2, %3.", "%1, %2 and %3 are three files in XML format, subgroups, teachers and activities timetables. The user can click on one file to view it").arg(tmp1).arg(tmp2).arg(tmp3);
		tos<<"      "<<tmp4<<"\n";

		tos<<"    </p>\n\n";

		tos<<"    <table border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\"4\">"+tr("Timetables")+"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		tos<<"          <th>"+tr("Days Horizontal")+"</th><th>"+tr("Days Vertical")+"</th><th>"+tr("Time Horizontal")+"</th><th>"+tr("Time Vertical")+"</th>";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		tos<<"      <tbody>\n";
		
		/* workaround
		tos<<"      <tfoot><tr><td></td><td colspan=\"4\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/

		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Subgroups")+"</th>\n";
		if(WRITE_TIMETABLES_SUBGROUPS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Groups")+"</th>\n";
		if(WRITE_TIMETABLES_GROUPS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Years")+"</th>\n";
		if(WRITE_TIMETABLES_YEARS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Teachers")+"</th>\n";
		if(WRITE_TIMETABLES_TEACHERS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Teachers Free Periods")+"</th>\n";
		if(WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
		tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Rooms")+"</th>\n";
		if(WRITE_TIMETABLES_ROOMS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Subjects")+"</th>\n";
		if(WRITE_TIMETABLES_SUBJECTS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Activity Tags")+"</th>\n";
		if(WRITE_TIMETABLES_ACTIVITY_TAGS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+tr("Activities")+"</th>\n";
		if(WRITE_TIMETABLES_ACTIVITIES){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+tr("view")+"</a></td>\n";
			else
				tos<<"          <td>"+tr("disabled")+"</td>\n";
		} else {
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
			tos<<"          <td>"+tr("disabled")+"</td>\n";
		}
		tos<<"        </tr>\n";
		//workaround begin.
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\"4\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		tos<<"    </table>\n";
	}

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

// writing the stylesheet in css format to a file by Volker Dirr.
void TimetableExport::writeStylesheetCss(QWidget* parent, const QString& cssfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	
	bool _writeAtLeastATimetable = writeAtLeastATimetable();

	if(!_writeAtLeastATimetable){
		if(QFile::exists(cssfilename))
			QFile::remove(cssfilename);

		return;
	}

	//get used students	//TODO: do it the same way in statistics.cpp
	QSet<QString> usedStudents;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		foreach(QString st, gt.rules.internalActivitiesList[i].studentsNames){
			if(!usedStudents.contains(st))
				usedStudents<<st;
		}
	}

	//Now we print the results to a CSS file
	QFile file(cssfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(cssfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	
	tos<<"@charset \"UTF-8\";"<<"\n\n";

	QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(INPUT_FILENAME_XML=="")
		tt=tr("unnamed");
	tos<<"/* "<<TimetableExport::tr("CSS Stylesheet of %1", "%1 is the file name").arg(tt);
	tos<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"   "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
	tos<<"   "<<TimetableExport::tr("Stylesheet generated with FET %1 on %2", "%1 is FET version, %2 is date and time").arg(FET_VERSION).arg(saveTime)<<" */\n\n";

	tos<<"/* "<<TimetableExport::tr("To do a page-break only after every second timetable, delete \"page-break-before: always;\" in \"table.even_table\".",
		"Please keep fields in quotes as they are, untranslated.")<<" */\n";
	tos<<"/* "<<TimetableExport::tr("Delete \"page-break-before: always;\" in \"table.even_table\" and in \"table.odd_table\" to skip page-breaks.",
		"Please keep fields in quotes as they are, untranslated.")<<" */\n";
	tos<<"/* "<<TimetableExport::tr("To hide an element just write the following phrase into the element: %1 (without quotes).",
		"%1 is a short phrase beginning and ending with quotes, and we want the user to be able to add it, but without quotes").arg("\"display:none;\"")<<" */\n\n";
	tos<<"p.back {\n  margin-top: 4ex;\n  margin-bottom: 5ex;\n}\n\n";
	tos<<"table {\n  text-align: center;\n  page-break-inside: avoid;\n}\n\n";
	tos<<"table.odd_table {\n  page-break-before: always;\n}\n\n";
	tos<<"table.even_table {\n  page-break-before: always;\n}\n\n";
	tos<<"table.detailed {\n  margin-left:auto; margin-right:auto;\n  text-align: center;\n  border: 0px;\n  border-spacing: 0;\n  border-collapse: collapse;\n}\n\n";
	tos<<"caption {\n\n}\n\n";
	tos<<"thead {\n\n}\n\n";
	
	//workaround begin.
	tos<<"/* "<<TimetableExport::tr("Some programs import \"tfoot\" incorrectly. So we use \"tr.foot\" instead of \"tfoot\".",
	 "Please keep tfoot and tr.foot untranslated, as they are in the original English phrase")<<" */\n\n";
	//tos<<"tfoot {\n\n}\n\n";
	tos<<"tr.foot {\n\n}\n\n";
	//workaround end

	tos<<"tbody {\n\n}\n\n";
	tos<<"th {\n\n}\n\n";
	tos<<"td {\n\n}\n\n";
	tos<<"td.detailed {\n  border: 1px dashed silver;\n  border-bottom: 0;\n  border-top: 0;\n}\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"th.xAxis {\n/* width: 8em; */\n}\n\n";
		tos<<"th.yAxis {\n  height: 8ex;\n}\n\n";
	}
	
	//By Liviu, with ideas from Volker
	if(TIMETABLE_HTML_LEVEL==7){ //must be written before LEVEL 3, because LEVEL 3 should have higher priority
		int cnt=0;
		foreach(int i, activeHashActivityColorBySubject){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName;
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			stringToColor(tmpString, &r, &g, &b);
			tos << "td.c_"<<QString::number(cnt+1)<<" { /* Activity id: "<<QString::number(act->id)<<" (subject) */\n ";
			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
			if (brightness<127.5)
				tos<<" color: white;\n";
			else
				tos<<" color: black;\n";
			tos<<"}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
		foreach(int i, activeHashActivityColorBySubjectAndStudents){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName+" "+act->studentsNames.join(", ");
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			stringToColor(tmpString, &r, &g, &b);
			tos << "td.c_"<<QString::number(cnt+1)<<" { /* Activity id: "<<QString::number(act->id)<<" (subject+students) */\n ";
			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
			if (brightness<127.5)
				tos<<" color: white;\n";
			else
				tos<<" color: black;\n";
			tos<<"}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
	}
//	if(TIMETABLE_HTML_LEVEL==7){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
//		QHashIterator<QString, QString> i(hashColorStringIDsTimetable);
//		while(i.hasNext()) {
//			i.next();
//			tos << "td.c_"<<i.value()<<" { /* "<<i.key()<<" */\n ";
//			
//			//similar to the coloring by Marco Vassura (start)
//			int r,g,b;
//			stringToColor(i.key(), &r, &g, &b);
//			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
//			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
//			if (brightness<127.5)
//				tos<<" color: white;\n";
//			else
//				tos<<" color: black;\n";
//			//similar to the coloring by Marco Vassura (end)
//			tos<<"}\n\n";
//		}
//	}
	else if(TIMETABLE_HTML_LEVEL>=4){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
		for(int i=0; i<gt.rules.nInternalSubjects; i++){
			tos << "span.s_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<" { /* subject "<<gt.rules.internalSubjectsList[i]->name<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.nInternalActivityTags; i++){
			tos << "span.at_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<" { /* activity tag "<<gt.rules.internalActivityTagsList[i]->name<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			if(usedStudents.contains(sty->name))
				tos << "span.ss_"<<hashStudentIDsTimetable.value(sty->name)<<" { /* students set "<<sty->name<<" */\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				if(usedStudents.contains(stg->name))
					tos << "span.ss_"<<hashStudentIDsTimetable.value(stg->name)<<" { /* students set "<<stg->name<<" */\n\n}\n\n";
				for(int k=0; k<stg->subgroupsList.size(); k++){
					StudentsSubgroup* sts=stg->subgroupsList[k];
					if(usedStudents.contains(sts->name))
						tos << "span.ss_"<<hashStudentIDsTimetable.value(sts->name)<<" { /* students set "<<sts->name<<" */\n\n}\n\n";
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			tos << "span.t_"<<hashTeacherIDsTimetable.value(gt.rules.internalTeachersList[i]->name)<<" { /* teacher "<<gt.rules.internalTeachersList[i]->name<<" */\n\n}\n\n";
		}
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos << "span.r_"<<hashRoomIDsTimetable.value(gt.rules.internalRoomsList[room]->name)<<" { /* room "<<gt.rules.internalRoomsList[room]->name<<" */\n\n}\n\n";
		}
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"span.subject {\n\n}\n\n";
		tos<<"span.activitytag {\n\n}\n\n";

		tos<<"span.empty {\n  color: gray;\n}\n\n";
		tos<<"td.empty {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";

		tos<<"span.notAvailable {\n  color: gray;\n}\n\n";
		tos<<"td.notAvailable {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";

		tos<<"span.break {\n  color: gray;\n}\n\n";
		tos<<"td.break {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";

		tos<<"tr.studentsset, div.studentsset {\n\n}\n\n";
		tos<<"tr.teacher, div.teacher {\n\n}\n\n";
		tos<<"tr.room, div.room {\n\n}\n\n";
		if(TIMETABLE_HTML_LEVEL!=7){
			tos<<"tr.line0, div.line0 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line1, div.line1 {\n\n}\n\n";
			tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n  color: gray;\n}\n\n";
			tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n  color: silver;\n}\n\n";
		} else {
			tos<<"tr.line0, div.line0 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line1, div.line1 {\n\n}\n\n";
			tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n}\n\n";
		}
	}
	if(TIMETABLE_HTML_LEVEL==6){
		tos<<endl<<"/* "<<TimetableExport::tr("Be careful. You might get mutual and ambiguous styles. CSS means that the last definition will be used.")<<" */\n\n";
		for(int i=0; i<gt.rules.nInternalSubjects; i++){
			tos << "td.s_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<" { /* subject "<<gt.rules.internalSubjectsList[i]->name<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.nInternalActivityTags; i++){
			tos << "td.at_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<" { /* activity tag "<<gt.rules.internalActivityTagsList[i]->name<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			if(usedStudents.contains(sty->name))
				tos << "td.ss_"<<hashStudentIDsTimetable.value(sty->name)<<" { /* students set "<<sty->name<<" */\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				if(usedStudents.contains(stg->name))
					tos << "td.ss_"<<hashStudentIDsTimetable.value(stg->name)<<" { /* students set "<<stg->name<<" */\n\n}\n\n";
				for(int k=0; k<stg->subgroupsList.size(); k++){
					StudentsSubgroup* sts=stg->subgroupsList[k];
					if(usedStudents.contains(sts->name))
						tos << "td.ss_"<<hashStudentIDsTimetable.value(sts->name)<<" { /* students set "<<sts->name<<" */\n\n}\n\n";
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			tos << "td.t_"<<hashTeacherIDsTimetable.value(gt.rules.internalTeachersList[i]->name)<<" { /* teacher "<<gt.rules.internalTeachersList[i]->name<<" */\n\n}\n\n";
		}

		//not included yet
		//for(int room=0; room<gt.rules.nInternalRooms; room++){
		//	tos << "span.r_"<<hashRoomIDsTimetable.value(gt.rules.internalRoomsList[room]->name)<<" { /* room "<<gt.rules.internalRoomsList[room]->name<<" */\n\n}\n\n";
		//}
	}
	tos<<"/* "<<TimetableExport::tr("Style the teachers free periods")<<" */\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"div.DESCRIPTION {\n  text-align: left;\n  font-size: smaller;\n}\n\n";
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"div.TEACHER_HAS_SINGLE_GAP {\n  color: black;\n}\n\n";
		tos<<"div.TEACHER_HAS_BORDER_GAP {\n  color: gray;\n}\n\n";
		tos<<"div.TEACHER_HAS_BIG_GAP {\n  color: silver;\n}\n\n";
		tos<<"div.TEACHER_MUST_COME_EARLIER {\n  color: purple;\n}\n\n";
		tos<<"div.TEACHER_MUST_COME_MUCH_EARLIER {\n  font-size: smaller;\n  color: fuchsia;\n}\n\n";
		tos<<"div.TEACHER_MUST_STAY_LONGER {\n  color: teal;\n}\n\n";
		tos<<"div.TEACHER_MUST_STAY_MUCH_LONGER {\n  font-size: smaller;\n  color: aqua;\n}\n\n";
		tos<<"div.TEACHER_HAS_A_FREE_DAY {\n  font-size: smaller;\n  color: red;\n}\n\n";
		tos<<"div.TEACHER_IS_NOT_AVAILABLE {\n  font-size: smaller;\n  color: olive;\n}\n\n";
	}
	tos<<"/* "<<TimetableExport::tr("End of file.")<<" */\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(cssfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group")<<" "<<protect2(stg->name)<<": \n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sts->name)<<"\">"<<protect2(sts->name)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<<singleSubgroupsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, subgroup, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group")<<" "<<protect2(stg->name)<<": \n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sts->name)<<"\">"<<protect2(sts->name)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<<singleSubgroupsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, subgroup, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos << singleSubgroupsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubgroups, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}	
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos << singleSubgroupsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubgroups, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

// by Volker Dirr
void TimetableExport::writeSubgroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleSubgroupsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubgroups, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}	
	file.close();
}

// by Volker Dirr
void TimetableExport::writeSubgroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();
	
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleSubgroupsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubgroups, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Now print the groups
//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group");
			tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(stg->name)<<"\">"<<protect2(stg->name)<<"</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int group=0; group<gt.rules.internalGroupsList.size(); group++){
		tos << singleGroupsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, group, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group");
			tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(stg->name)<<"\">"<<protect2(stg->name)<<"</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int group=0; group<gt.rules.internalGroupsList.size(); group++){
		tos<<singleGroupsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, group, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleGroupsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.internalGroupsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleGroupsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.internalGroupsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleGroupsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.internalGroupsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();
	
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleGroupsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.internalGroupsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Now print the years

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year");
		tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sty->name)<<"\">"<<protect2(sty->name)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		tos << singleYearsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, year, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year");
		tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sty->name)<<"\">"<<protect2(sty->name)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		tos << singleYearsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, year, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleYearsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.augmentedYearsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleYearsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.augmentedYearsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleYearsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.augmentedYearsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();
	
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleYearsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.augmentedYearsList.size(), tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Print all activities

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<singleAllActivitiesTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<singleAllActivitiesTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	tos<<singleAllActivitiesTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	tos<<singleAllActivitiesTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tos<<singleAllActivitiesTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tos<<singleAllActivitiesTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Print the teachers

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	
	tos<<writeHead(true, placedActivities, true);
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<hashTeacherIDsTimetable.value(teacher_name)<<"\">"<<protect2(teacher_name)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		tos<<singleTeachersTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, teacher, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true); 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<hashTeacherIDsTimetable.value(teacher_name)<<"\">"<<protect2(teacher_name)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		tos<<singleTeachersTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, teacher, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeTeachersTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	QSet<int> tmp;
	tos<<singleTeachersTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalTeachers, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	QSet<int> tmp;
	tos<<singleTeachersTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalTeachers, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeTeachersTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleTeachersTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalTeachers, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeTeachersTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleTeachersTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalTeachers, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);	
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashRoomIDsTimetable.value(room_name)<<"\">"<<protect2(room_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos<<singleRoomsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, room, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;

		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
		
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashRoomIDsTimetable.value(room_name)<<"\">"<<protect2(room_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos<<singleRoomsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, room, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleRoomsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalRooms, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

// writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleRoomsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalRooms, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QSet<int> tmp;
			tos<<singleRoomsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalRooms, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
		}
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QSet<int> tmp;
			tos<<singleRoomsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalRooms, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
		}

	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Print the subjects

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Subject");
		tos<<" <a href=\""<<"#table_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<"\">"<<gt.rules.internalSubjectsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";


	for(int subject=0; subject<gt.rules.nInternalSubjects; subject++){
		tos<<singleSubjectsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, subject, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Subject");
		tos<<" <a href=\""<<"#table_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<"\">"<<gt.rules.internalSubjectsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int subject=0; subject<gt.rules.nInternalSubjects; subject++){
		tos<<singleSubjectsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, subject, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleSubjectsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubjects, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleSubjectsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubjects, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleSubjectsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubjects, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleSubjectsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubjects, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Print the activity tags

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Activity Tag");
		tos<<" <a href=\""<<"#table_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<"\">"<<gt.rules.internalActivityTagsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";


	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags; activityTag++){
		tos<<singleActivityTagsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, activityTag, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of contents")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Activity Tag");
		tos<<" <a href=\""<<"#table_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<"\">"<<gt.rules.internalActivityTagsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags; activityTag++){
		tos<<singleActivityTagsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, activityTag, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleActivityTagsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalActivityTags, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleActivityTagsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalActivityTags, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleActivityTagsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalActivityTags, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}

	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays();

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		QSet<int> tmp;
		tos<<singleActivityTagsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalActivityTags, tmp, saveTime, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Print the teachers free periods. Code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<TimetableExport::tr("Teacher has a single gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<TimetableExport::tr("Teacher has a border gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<TimetableExport::tr("Teacher has a big gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<TimetableExport::tr("Teacher must come earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<TimetableExport::tr("Teacher must come much earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<TimetableExport::tr("Teacher must stay longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<TimetableExport::tr("Teacher must stay much longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<TimetableExport::tr("Teacher has a free day")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<TimetableExport::tr("Teacher is not available")<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n\n";
	
	tos<<singleTeachersFreePeriodsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS, TIMETABLE_HTML_REPEAT_NAMES);
	
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<TimetableExport::tr("Teacher has a single gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<TimetableExport::tr("Teacher has a border gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<TimetableExport::tr("Teacher has a big gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<TimetableExport::tr("Teacher must come earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<TimetableExport::tr("Teacher must come much earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<TimetableExport::tr("Teacher must stay longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<TimetableExport::tr("Teacher must stay much longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<TimetableExport::tr("Teacher has a free day")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<TimetableExport::tr("Teacher is not available")<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n\n";

	tos<<singleTeachersFreePeriodsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Code contributed by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeTeachersStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	
	if(!WRITE_TIMETABLES_STATISTICS || !WRITE_TIMETABLES_TEACHERS){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	bool PRINT_DETAILED=true;
	tos<<singleTeachersStatisticsHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED, TIMETABLE_HTML_REPEAT_NAMES, true);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Code contributed by Volker Dirr (http://timetabling.de/)
void TimetableExport::writeStudentsStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	if(!WRITE_TIMETABLES_STATISTICS || !(WRITE_TIMETABLES_YEARS || WRITE_TIMETABLES_GROUPS || WRITE_TIMETABLES_SUBGROUPS) ){
		if(QFile::exists(htmlfilename))
			QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	bool PRINT_DETAILED=true;
	tos<<singleStudentsStatisticsHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED, TIMETABLE_HTML_REPEAT_NAMES, true);

	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void TimetableExport::computeHashForIDsTimetable(){
// by Volker Dirr

//TODO This is unneeded if you use a relational data base, because we can use the primary key id of the database.
//This is very similar to statistics compute hash. So always check it if you change something here!

/*	QSet<QString> usedStudents;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		foreach(QString st, gt.rules.internalActivitiesList[i].studentsNames){
			if(!usedStudents.contains(st))
				usedStudents<<st;
		}
	}*/
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	hashStudentIDsTimetable.clear();
	int cnt=1;
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		//if(usedStudents.contains(sty->name)){
		if(!hashStudentIDsTimetable.contains(sty->name)){
			hashStudentIDsTimetable.insert(sty->name, CustomFETString::number(cnt));
			cnt++;
		}
		//}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
		//	if(usedStudents.contains(stg->name)){
			if(!hashStudentIDsTimetable.contains(stg->name)){
				hashStudentIDsTimetable.insert(stg->name, CustomFETString::number(cnt));
				cnt++;
			}
		//	}
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
		//		if(usedStudents.contains(sts->name)){
				if(!hashStudentIDsTimetable.contains(sts->name)){
					hashStudentIDsTimetable.insert(sts->name, CustomFETString::number(cnt));
					cnt++;
				}
		//		}
			}
		}
	}

	hashSubjectIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		hashSubjectIDsTimetable.insert(gt.rules.internalSubjectsList[i]->name, CustomFETString::number(i+1));
	}
	hashActivityTagIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
		hashActivityTagIDsTimetable.insert(gt.rules.internalActivityTagsList[i]->name, CustomFETString::number(i+1));
	}
	hashTeacherIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		hashTeacherIDsTimetable.insert(gt.rules.internalTeachersList[i]->name, CustomFETString::number(i+1));
	}
	hashRoomIDsTimetable.clear();
	for(int room=0; room<gt.rules.nInternalRooms; room++){
		hashRoomIDsTimetable.insert(gt.rules.internalRoomsList[room]->name, CustomFETString::number(room+1));
	}
	hashDayIDsTimetable.clear();
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		hashDayIDsTimetable.insert(gt.rules.daysOfTheWeek[day], CustomFETString::number(day+1));
	}
	if(TIMETABLE_HTML_LEVEL==7){
		computeHashActivityColorBySubject();
		computeHashActivityColorBySubjectAndStudents();
	}
}

//By Liviu, with ideas from Volker
void TimetableExport::computeHashActivityColorBySubject(){
	QHash<QString, int> tmpHash;

	hashActivityColorBySubject.clear();
	activeHashActivityColorBySubject.clear();
	
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QSet<QString> alreadyAdded;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			Activity* act=&gt.rules.internalActivitiesList[i];
			QString tmpString=act->subjectName;
			if(!alreadyAdded.contains(tmpString)){
				alreadyAdded.insert(tmpString);
				hashActivityColorBySubject.insert(i, alreadyAdded.count());
				activeHashActivityColorBySubject.append(i);
				tmpHash.insert(tmpString, alreadyAdded.count());
			}
			else{
				assert(tmpHash.contains(tmpString));
				int k=tmpHash.value(tmpString);
				hashActivityColorBySubject.insert(i, k);
			}
		}
	}
	
	//cout<<"hashActivityColorBySubject.count()=="<<hashActivityColorBySubject.count()<<endl;
}

//By Liviu, with ideas from Volker
void TimetableExport::computeHashActivityColorBySubjectAndStudents(){
	QHash<QString, int> tmpHash;

	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
	
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QSet<QString> alreadyAdded;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName+" "+act->studentsNames.join(", ");
			if(!alreadyAdded.contains(tmpString)){
				alreadyAdded.insert(tmpString);
				hashActivityColorBySubjectAndStudents.insert(i, alreadyAdded.count());
				activeHashActivityColorBySubjectAndStudents.append(i);
				tmpHash.insert(tmpString, alreadyAdded.count());
			}
			else{
				assert(tmpHash.contains(tmpString));
				int k=tmpHash.value(tmpString);
				hashActivityColorBySubjectAndStudents.insert(i, k);
			}
		}
	}
	
	//cout<<"hashActivityColorBySubjectAndStudents.count()=="<<hashActivityColorBySubjectAndStudents.count()<<endl;
}

/*void TimetableExport::computeHashForColors(QHash<QString, QString>& hashColorStringIDsTimetable){
// by Volker Dirr
	qWarning("compute hash for colors");
	hashColorStringIDsTimetable.clear();
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	QSet<QString> alreadyAddedString;
	for(int i=0; i<gt.rules.nInternalActivities; i++) {
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			qWarning("add a hash");
			//coloring for students
			QString tmpString=act->subjectName;
			if(!alreadyAddedString.contains(tmpString)){
				alreadyAddedString<<tmpString;
				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
			}
			//coloring for teachers
			tmpString=act->subjectName+" "+act->studentsNames.join(", ");
			if(!alreadyAddedString.contains(tmpString)){
				alreadyAddedString<<tmpString;
				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
			}
			//coloring for rooms
//			it is similar to students
//			tmpString=act->subjectName+" "+act->studentsNames.join(", ");
//			if(!alreadyAddedString.contains(tmpString)){
//				alreadyAddedString<<tmpString;
//				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
//			}
		}
	}
}*/

void TimetableExport::computeActivitiesAtTime(){		// by Liviu Lalescu
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++)
			activitiesAtTime[day][hour].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by students or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
}


void TimetableExport::computeActivitiesWithSameStartingTime(){
// by Volker Dirr
	activitiesWithSameStartingTime.clear();

	if(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME){
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
			if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){ //not needed anymore:  && tc->weightPercentage==100
				ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*) tc;
				for(int a=0; a<c->_n_activities; a++){
					//speed improvement
					QList<int> & tmpList=activitiesWithSameStartingTime[c->_activities[a]];
					for(int b=0; b<c->_n_activities; b++){
						if(a!=b){
							if(best_solution.times[c->_activities[a]]==best_solution.times[c->_activities[b]]){ 	//because constraint is maybe not with 100% weight and failed
								if(!tmpList.contains(c->_activities[b])){
									tmpList<<c->_activities[b];
								}
							}
						}
					}
					/*
					QList<int> tmpList;
					if(activitiesWithSameStartingTime.contains(c->_activities[a]))
						tmpList=activitiesWithSameStartingTime.value(c->_activities[a]);
					for(int b=0; b<c->_n_activities; b++){
						if(a!=b){
							if(best_solution.times[c->_activities[a]]==best_solution.times[c->_activities[b]]){ 	//because constraint is maybe not with 100% weight and failed
								if(!tmpList.contains(c->_activities[b])){
									tmpList<<c->_activities[b];
								}
							}
						}
					}
					activitiesWithSameStartingTime.insert(c->_activities[a], tmpList);
					*/
				}
			}
		}
	}
}

bool TimetableExport::addActivitiesWithSameStartingTime(QList<int>& allActivities, int hour){
// by Volker Dirr
	if(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME){
		bool activitiesWithSameStartingtime=false;
		QList<int> allActivitiesNew;
		foreach(int tmpAct, allActivities){
			allActivitiesNew<<tmpAct;
			if(activitiesWithSameStartingTime.contains(tmpAct)){
				QList<int> sameTimeList=activitiesWithSameStartingTime.value(tmpAct);
				foreach(int sameTimeAct, sameTimeList){
					if(!allActivitiesNew.contains(sameTimeAct) && !allActivities.contains(sameTimeAct)){
						if(best_solution.times[sameTimeAct]!=UNALLOCATED_TIME){
							Activity* act=&gt.rules.internalActivitiesList[sameTimeAct];
							assert(best_solution.times[tmpAct]==best_solution.times[sameTimeAct]);//{
								if((best_solution.times[sameTimeAct]/gt.rules.nDaysPerWeek+(act->duration-1))>=hour){
									allActivitiesNew<<sameTimeAct;
								}
								activitiesWithSameStartingtime=true; //don't add this line in previous if command because of activities with different duration!
							//}
						}
					}
				}
			}
		}
		//allActivities.clear();
		allActivities=allActivitiesNew;
		//allActivitiesNew.clear();
		return activitiesWithSameStartingtime;
	}
	else
		return false;
}

// by Volker Dirr
QString TimetableExport::writeHead(bool java, int placedActivities, bool printInstitution){
	QString tmp;
	tmp+="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tmp+="  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(!LANGUAGE_STYLE_RIGHT_TO_LEFT)
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\">\n";
	else
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\" dir=\"rtl\">\n";
	tmp+="  <head>\n";
	tmp+="    <title>"+protect2(gt.rules.institutionName)+"</title>\n";
	tmp+="    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		
		if(cssfilename.right(4)==".fet")
			cssfilename=cssfilename.left(cssfilename.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
		
		cssfilename+="_"+STYLESHEET_CSS;
		if(INPUT_FILENAME_XML=="")
			cssfilename=STYLESHEET_CSS;
		tmp+="    <link rel=\"stylesheet\" media=\"all\" href=\""+cssfilename+"\" type=\"text/css\" />\n";
	}
	if(java){
		if(TIMETABLE_HTML_LEVEL>=5 && TIMETABLE_HTML_LEVEL!=7){  // the following JavaScript code is pretty similar to an example of Les Richardson
			tmp+="    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
			tmp+="    <script type=\"text/javascript\">\n";
			tmp+="      function highlight(classval) {\n";
			tmp+="        var spans = document.getElementsByTagName('span');\n";
			tmp+="        for(var i=0; spans.length>i; i++) {\n";
			tmp+="          if (spans[i].className == classval) {\n";
			tmp+="            spans[i].style.backgroundColor = 'lime';\n";
			tmp+="          } else {\n";
			tmp+="            spans[i].style.backgroundColor = 'white';\n";
			tmp+="          }\n";
			tmp+="        }\n";
			tmp+="      }\n";
			tmp+="    </script>\n";
		}
	}
	tmp+="  </head>\n\n";
	tmp+="  <body id=\"top\">\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tmp+="    <h1>"+TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)+"</h1>\n";
	if(printInstitution){
		tmp+="    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+TimetableExport::tr("Institution name")+":</th>\n        <td>"+protect2(gt.rules.institutionName)+"</td>\n      </tr>\n    </table>\n";
		tmp+="    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+TimetableExport::tr("Comments")+":</th>\n        <td>"+protect2(gt.rules.comments).replace(QString("\n"), QString("<br />\n"))+"</td>\n      </tr>\n    </table>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeTOCDays(){
	QString tmp;
	tmp+="    <p><strong>"+TimetableExport::tr("Table of contents")+"</strong></p>\n";
	tmp+="    <ul>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmp+="      <li>\n        ";
		tmp+=" <a href=\"#table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</a>\n";
		tmp+="          </li>\n";
	}
	tmp+="    </ul>\n    <p>&nbsp;</p>\n";
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeStartTagTDofActivities(int htmlLevel, const Activity* act, bool detailed, bool colspan, bool rowspan, int colorBy){
	QString tmp;
	assert(!(colspan && rowspan));
	if(detailed)
		assert(!colspan && !rowspan);
	else
		tmp+="          ";
	tmp+="<td";
	if(rowspan && act->duration>1)
		tmp+=" rowspan=\""+CustomFETString::number(act->duration)+"\"";
	if(colspan && act->duration>1)
		tmp+=" colspan=\""+CustomFETString::number(act->duration)+"\"";
	if(htmlLevel==6){
		tmp+=" class=\"";
		if(act->subjectName.size()>0){
			tmp+="s_"+hashSubjectIDsTimetable.value(act->subjectName);
		}
		if(act->activityTagsNames.size()>0){
			foreach(QString atn, act->activityTagsNames)
				 tmp+=" at_"+hashActivityTagIDsTimetable.value(atn);
		}
		if(act->studentsNames.size()>0){
			foreach(QString st, act->studentsNames)
				tmp+=" ss_"+hashStudentIDsTimetable.value(st);
		}
		if(act->teachersNames.size()>0){
			foreach(QString t, act->teachersNames)
				tmp+=" t_"+hashTeacherIDsTimetable.value(t);
		}
		//i need ai for this!!! so i need a parameter ai?! //TODO
		/*int r=best_solution.rooms[ai];
		if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
			tmp+=" room_"+protect2id(gt.rules.internalRoomsList[r]->name);
		}*/
		if(detailed)
			tmp+=" detailed";
		tmp+="\"";
	}
	if(htmlLevel==7){
		assert(gt.rules.activitiesHash.contains(act->id));
		int index=gt.rules.activitiesHash.value(act->id);
		switch(colorBy){
			case COLOR_BY_SUBJECT_STUDENTS: tmp+=" class=\"c_"+QString::number(activeHashActivityColorBySubject.count()+hashActivityColorBySubjectAndStudents.value(index)); break;
			case COLOR_BY_SUBJECT: tmp+=" class=\"c_"+QString::number(hashActivityColorBySubject.value(index)); break;
			default: assert(0==1);
		}
		
		if(detailed)
			tmp+=" detailed";
		tmp+="\"";
	}
	if(detailed && htmlLevel>=1 && htmlLevel<=5)
		tmp+=" class=\"detailed\"";
	tmp+=">";
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeSubjectAndActivityTags(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute, bool activityTagsOnly, bool printActivityTags){
	QString tmp;
	if(act->subjectName.size()>0||act->activityTagsNames.size()>0){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		if(act->subjectName.size()>0 && !activityTagsOnly){
			switch(htmlLevel){
				case 3 : tmp+="<span class=\"subject\">"+protect2(act->subjectName)+"</span>"; break;
				case 4 : tmp+="<span class=\"subject\"><span class=\"s_"+hashSubjectIDsTimetable.value(act->subjectName)+"\">"+protect2(act->subjectName)+"</span></span>"; break;
				case 5 : ;
				case 6 : tmp+="<span class=\"subject\"><span class=\"s_"+hashSubjectIDsTimetable.value(act->subjectName)+"\" onmouseover=\"highlight('s_"+hashSubjectIDsTimetable.value(act->subjectName)+"')\">"+protect2(act->subjectName)+"</span></span>"; break;
				case 7 : tmp+="<span class=\"subject\">"+protect2(act->subjectName)+"</span>"; break;
				default: tmp+=protect2(act->subjectName); break;
			}
			if(act->activityTagsNames.size()>0 && printActivityTags){
				tmp+=" ";
			}
		}
		if(act->activityTagsNames.size()>0 && printActivityTags){
			if(!activityTagsOnly){
				if(htmlLevel>=3){
					tmp+="<span class=\"activitytag\">";
				}
			}
			foreach(QString atn, act->activityTagsNames){
				switch(htmlLevel){
					case 3 : tmp+=protect2(atn); break;
					case 4 : tmp+="<span class=\"at_"+hashActivityTagIDsTimetable.value(atn)+"\">"+protect2(atn)+"</span>"; break;
					case 5 : ;
					case 6 : tmp+="<span class=\"at_"+hashActivityTagIDsTimetable.value(atn)+"\" onmouseover=\"highlight('at_"+hashActivityTagIDsTimetable.value(atn)+"')\">"+protect2(atn)+"</span>"; break;
					default: tmp+=protect2(atn); break;
				}
				tmp+=", ";
			}
			tmp.remove(tmp.size()-2, 2);
			if(!activityTagsOnly){
				if(htmlLevel>=3){
					tmp+="</span>";
				}
			}
		}
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeStudents(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute){
	QString tmp;
	if(act->studentsNames.size()>0){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		foreach(QString st, act->studentsNames){
			switch(htmlLevel){
				case 4 : tmp+="<span class=\"ss_"+hashStudentIDsTimetable.value(st)+"\">"+protect2(st)+"</span>"; break;
				case 5 : ;
				case 6 : tmp+="<span class=\"ss_"+hashStudentIDsTimetable.value(st)+"\" onmouseover=\"highlight('ss_"+hashStudentIDsTimetable.value(st)+"')\">"+protect2(st)+"</span>"; break;
				default: tmp+=protect2(st); break;
			}
			tmp+=", ";
		}
		tmp.remove(tmp.size()-2, 2);
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeTeachers(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute){
	QString tmp;
	if(act->teachersNames.size()>0){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		foreach(QString t, act->teachersNames){
			switch(htmlLevel){
				case 4 : tmp+="<span class=\"t_"+hashTeacherIDsTimetable.value(t)+"\">"+protect2(t)+"</span>"; break;
				case 5 : ;
				case 6 : tmp+="<span class=\"t_"+hashTeacherIDsTimetable.value(t)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(t)+"')\">"+protect2(t)+"</span>"; break;
				default: tmp+=protect2(t); break;
			}
			tmp+=", ";
		}
		tmp.remove(tmp.size()-2, 2);
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeRoom(int htmlLevel, int ai, const QString& startTag, const QString& startTagAttribute){
	QString tmp;
	int r=best_solution.rooms[ai];
	if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		switch(htmlLevel){
			case 4 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\">"+protect2(gt.rules.internalRoomsList[r]->name)+"</span>"; break;
			case 5 : ;
			case 6 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\" onmouseover=\"highlight('r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"')\">"+protect2(gt.rules.internalRoomsList[r]->name)+"</span>"; break;
			default: tmp+=protect2(gt.rules.internalRoomsList[r]->name); break;
		}
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeNotAvailableSlot(int htmlLevel, const QString& weight){
	QString tmp;
	//weight=" "+weight;
	switch(htmlLevel){
		case 3 : ;
		case 4 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		case 5 : ;
		case 6 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</td>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeBreakSlot(int htmlLevel, const QString& weight){
	QString tmp;
	//weight=" "+weight;
	switch(htmlLevel){
		case 3 : ;
		case 4 : tmp="          <td class=\"break\"><span class=\"break\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		case 5 : ;
		case 6 : tmp="          <td class=\"break\"><span class=\"break\" onmouseover=\"highlight('break')\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"break\"><span class=\"break\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_BREAK_SLOT)+weight+"</td>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeEmpty(int htmlLevel){
	QString tmp;
	switch(htmlLevel){
		case 3 : ;
		case 4 : tmp="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		case 5 : ;
		case 6 : tmp="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_EMPTY_SLOT)+"</td>\n";
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityStudents(int htmlLevel, int ai, int day, int hour, bool notAvailable, bool colspan, bool rowspan, bool printActivityTags, QString skipStudentsSet){
	QString tmp;
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT);
			//TODO line0
			bool skipLine0=false;
			if(act->studentsNames.size()==1){
				if(act->studentsNames.at(0)==skipStudentsSet){
					skipLine0=true;
				}
			}
			if(!skipLine0){
				tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line0\"");
			}
			tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line1\"", false, printActivityTags);
			tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line2\"");
			tmp+=writeRoom(htmlLevel, ai, "div", " class=\"room line3\"");
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(notAvailable && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesStudents(int htmlLevel, const QList<int>& allActivities, bool printActivityTags){
	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"studentsset line0\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT)+writeStudents(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"line1\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT)+writeSubjectAndActivityTags(htmlLevel, act, "", "", false, printActivityTags)+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"teacher line2\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT)+writeTeachers(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"room line3\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT)+writeRoom(htmlLevel, ai, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityTeacher(int htmlLevel, int teacher, int day, int hour, bool colspan, bool rowspan, bool printActivityTags, QString skipTeacher){
	QString tmp;
	int ai=teachers_timetable_weekly[teacher][day][hour];
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT_STUDENTS);
			//TODO line0
			bool skipLine0=false;
			if(act->teachersNames.size()==1){
				if(act->teachersNames.at(0)==skipTeacher){
					skipLine0=true;
				}
			}
			if(!skipLine0){
				tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line0\"");
			}	
			tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line1\"");
			tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line2\"", false, printActivityTags);
			tmp+=writeRoom(htmlLevel, ai, "div", " class=\"room line3\"");
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(teacherNotAvailableDayHour[teacher][day][hour] && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesTeachers(int htmlLevel, const QList<int>& allActivities, bool printActivityTags){
	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"teacher line0\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeTeachers(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"studentsset line1\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeStudents(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"line2\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeSubjectAndActivityTags(htmlLevel, act, "", "", false, printActivityTags)+"</td>";
		}
	}
	tmp+="</tr>";

	if(htmlLevel>=3)
		tmp+="<tr class=\"room line3\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeRoom(htmlLevel, ai, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityRoom(int htmlLevel, int room, int day, int hour, bool colspan, bool rowspan, bool printActivityTags){
	QString tmp;
	int ai=rooms_timetable_weekly[room][day][hour];
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT_STUDENTS);
			//Each activity has only a single room. So there is no need for line0. Modify this as soon as FET supports multiple rooms per activity.
			tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line1\"");
			tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line2\"");
			tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line3\"", false, printActivityTags);
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(notAllowedRoomTimePercentages[room][day+hour*gt.rules.nDaysPerWeek]>=0 && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesRooms(int htmlLevel, const QList<int>& allActivities, bool printActivityTags){
	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"room line0\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeRoom(htmlLevel, ai, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"studentsset line1\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeStudents(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"teacher line2\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeTeachers(htmlLevel, act, "", "")+"</td>";
		}
	}
	tmp+="</tr>";
	if(htmlLevel>=3)
		tmp+="<tr class=\"line3\">";
	else	tmp+="<tr>";
	for(int a=0; a<allActivities.size(); a++){
		int ai=allActivities[a];
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeSubjectAndActivityTags(htmlLevel, act, "", "", false, printActivityTags)+"</td>";
		}
	}
	tmp+="</tr>";

	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesSubjects(int htmlLevel, const QList<int>& allActivities, bool printActivityTags){
	QString tmp;
	if(allActivities.isEmpty()){
		tmp+=writeEmpty(htmlLevel);
	} else {
		if(htmlLevel>=1)
			tmp+="          <td><table class=\"detailed\">";
		else
			tmp+="          <td><table>";
		//Each activity has only a single subject. So there is no need for subjects in line0. Modify this as soon as FET supports multiple subjects per activity.
		if(printActivityTags){
			if(htmlLevel>=3)
				tmp+="<tr class=\"line0 activitytag\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeSubjectAndActivityTags(htmlLevel, act, "", "", true, printActivityTags)+"</td>";
			}
			tmp+="</tr>";
		}
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line1\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeStudents(htmlLevel, act, "", "")+"</td>";	
		}
		tmp+="</tr>";
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line2\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeTeachers(htmlLevel, act, "", "")+"</td>";
		}
		tmp+="</tr>";
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line3\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeRoom(htmlLevel, ai, "", "")+"</td>";
		}
		tmp+="</tr>";
		tmp+="</table></td>\n";
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesActivityTags(int htmlLevel, const QList<int>& allActivities, bool printActivityTags){
	QString tmp;
	if(allActivities.isEmpty()){
		tmp+=writeEmpty(htmlLevel);
	} else {
		if(htmlLevel>=1)
			tmp+="          <td><table class=\"detailed\">";
		else
			tmp+="          <td><table>";
		
		if(htmlLevel>=3)
			tmp+="<tr class=\"line0\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeSubjectAndActivityTags(htmlLevel, act, "", "", false, printActivityTags)+"</td>";
			}
		}
		tmp+="</tr>";
		
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line1\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeStudents(htmlLevel, act, "", "")+"</td>";	
		}
		tmp+="</tr>";
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line2\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeTeachers(htmlLevel, act, "", "")+"</td>";
		}
		tmp+="</tr>";
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line3\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS)+writeRoom(htmlLevel, ai, "", "")+"</td>";
		}
		tmp+="</tr>";
		tmp+="</table></td>\n";
	}
	return tmp;
}


//the following functions return a single html table (needed for html file export and printing)

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableDaysHorizontalHtml(int htmlLevel, int subgroup, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(subgroup>=0);
	assert(subgroup<gt.rules.nInternalSubgroups);
	QString tmpString;
	QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	if(subgroup%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
		
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(subgroup_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<students_timetable_weekly[subgroup][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], false, true, printActivityTags, subgroup_name);
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableDaysVerticalHtml(int htmlLevel, int subgroup, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(subgroup>=0);
	assert(subgroup<gt.rules.nInternalSubgroups);
	QString tmpString;
	QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	if(subgroup%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(subgroup_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<students_timetable_weekly[subgroup][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], true, false, printActivityTags, subgroup_name);
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeVerticalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		if(!excludedNames.contains(subgroup)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalSubgroupsList[subgroup]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			currentCount=0;
			for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
				if(!excludedNames.contains(subgroup)){
					currentCount++;
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<subgroup;
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<students_timetable_weekly[subgroup][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], false, true, printActivityTags, gt.rules.internalSubgroupsList[subgroup]->name);
					} else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				
				if(hour==0)
					tmpString+="        <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="<th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	}
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="        </tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		if(!excludedNames.contains(subgroup)){
			currentCount++;
			excludedNames<<subgroup;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalSubgroupsList[subgroup]->name)+"</th>\n";
			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<students_timetable_weekly[subgroup][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], true, false, printActivityTags, gt.rules.internalSubgroupsList[subgroup]->name);
					} else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalSubgroupsList[subgroup]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		if(!excludedNames.contains(subgroup)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalSubgroupsList[subgroup]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		currentCount=0;
		for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
			if(!excludedNames.contains(subgroup)){
				currentCount++;
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<subgroup;
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<students_timetable_weekly[subgroup][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], false, true, printActivityTags, gt.rules.internalSubgroupsList[subgroup]->name);
				} else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="        </tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		if(!excludedNames.contains(subgroup)){
			currentCount++;
			excludedNames<<subgroup;

			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalSubgroupsList[subgroup]->name+"</th>\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<students_timetable_weekly[subgroup][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[subgroup][day][hour], day, hour, subgroupNotAvailableDayHour[subgroup][day][hour], true, false, printActivityTags, gt.rules.internalSubgroupsList[subgroup]->name);
				} else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=gt.rules.internalSubgroupsList[subgroup]->name+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleGroupsTimetableDaysHorizontalHtml(int htmlLevel, int group, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(group>=0);
	assert(group<gt.rules.internalGroupsList.size());
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList[group]->name);
	tmpString+="\" border=\"1\"";
	if(group%2) tmpString+=" class=\"even_table\"";
	else tmpString+=" class=\"odd_table\"";
	tmpString+=">\n";
			
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(gt.rules.internalGroupsList[group]->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList[group]->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList[group]->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
					allActivities<<students_timetable_weekly[subgroup][day][hour];
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
				}
			assert(!allActivities.isEmpty());
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.internalGroupsList[group]->name);
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleGroupsTimetableDaysVerticalHtml(int htmlLevel, int group, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(group>=0);
	assert(group<gt.rules.internalGroupsList.size());
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList.at(group)->name);
	tmpString+="\" border=\"1\"";
	if(group%2) tmpString+=" class=\"even_table\"";
	else tmpString+=" class=\"odd_table\"";
	tmpString+=">\n";
			
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;
			allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
					allActivities<<students_timetable_weekly[subgroup][day][hour];
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
			}
			assert(!allActivities.isEmpty());
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.internalGroupsList[group]->name);
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeVerticalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table";
	tmpString+="\" border=\"1\">\n";
		tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
		tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			currentCount=0;
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
				if(!excludedNames.contains(group)){
					currentCount++;
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<group;
					QList<int> allActivities;
					allActivities.clear();
					bool isNotAvailable=true;
					for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
							allActivities<<students_timetable_weekly[subgroup][day][hour];
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
					assert(!allActivities.isEmpty());
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.internalGroupsList[group]->name);
					} else{
						if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
						else{
							tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
						}
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table";
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			excludedNames<<group;

			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>\n";
			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;
					allActivities.clear();
					bool isNotAvailable=true;
					for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
							allActivities<<students_timetable_weekly[subgroup][day][hour];
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
					assert(!allActivities.isEmpty());
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.internalGroupsList[group]->name);
					} else{
						if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
						else{
							tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
						}
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day]);
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalGroupsList.at(group)->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		currentCount=0;
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">" + protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
			if(!excludedNames.contains(group)){
				currentCount++;
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<group;
				QList<int> allActivities;
				allActivities.clear();
				bool isNotAvailable=true;
				for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
						allActivities<<students_timetable_weekly[subgroup][day][hour];
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
				assert(!allActivities.isEmpty());
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.internalGroupsList[group]->name);
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">" + protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day]);
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			excludedNames<<group;

			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;
				allActivities.clear();
				bool isNotAvailable=true;
				for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
						allActivities<<students_timetable_weekly[subgroup][day][hour];
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
				assert(!allActivities.isEmpty());
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.internalGroupsList[group]->name);
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalGroupsList.at(group)->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleYearsTimetableDaysHorizontalHtml(int htmlLevel, int year, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(year>=0);
	assert(year<gt.rules.augmentedYearsList.size());
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	tmpString+="\" border=\"1\"";
	if(year%2)  tmpString+=" class=\"even_table\"";
	else tmpString+=" class=\"odd_table\"";
	tmpString+=">\n";
				
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(gt.rules.augmentedYearsList.at(year)->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<gt.rules.augmentedYearsList.at(year)->groupsList.size(); g++){
				StudentsGroup* stg=gt.rules.augmentedYearsList.at(year)->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
						allActivities<<students_timetable_weekly[subgroup][day][hour];
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivities.isEmpty());
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleYearsTimetableDaysVerticalHtml(int htmlLevel, int year, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(year>=0);
	assert(year<gt.rules.augmentedYearsList.size());
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	tmpString+="\" border=\"1\"";
	if(year%2)  tmpString+=" class=\"even_table\"";
	else tmpString+=" class=\"odd_table\"";
	tmpString+=">\n";
			
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.augmentedYearsList.at(year)->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";

		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;
			allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<gt.rules.augmentedYearsList.at(year)->groupsList.size(); g++){
				StudentsGroup* stg=gt.rules.augmentedYearsList.at(year)->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
						allActivities<<students_timetable_weekly[subgroup][day][hour];
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivities.isEmpty());
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeVerticalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table";
	tmpString+="\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.augmentedYearsList.at(year)->name)+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour]) + "</th>\n";
			currentCount=0;
			for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
				if(!excludedNames.contains(year)){
					currentCount++;
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<year;
					QList<int> allActivities;
					allActivities.clear();
					bool isNotAvailable=true;
					StudentsYear* sty=gt.rules.augmentedYearsList[year];
					for(int g=0; g<sty->groupsList.size(); g++){
						StudentsGroup* stg=sty->groupsList[g];
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							int subgroup=sts->indexInInternalSubgroupsList;
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
								allActivities<<students_timetable_weekly[subgroup][day][hour];
							if(!subgroupNotAvailableDayHour[subgroup][day][hour])
								isNotAvailable=false;
						}
					}
					assert(!allActivities.isEmpty());
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
					} else{
						if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
						else{
							tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
						}
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour]) + "</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeHorizontalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table";
	tmpString+="\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		if(!excludedNames.contains(year)){
			currentCount++;
			excludedNames<<year;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(sty->name)+"</th>\n";
			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;
					allActivities.clear();
					bool isNotAvailable=true;
					for(int g=0; g<sty->groupsList.size(); g++){
						StudentsGroup* stg=sty->groupsList[g];
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							int subgroup=sts->indexInInternalSubgroupsList;
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
								allActivities<<students_timetable_weekly[subgroup][day][hour];
							if(!subgroupNotAvailableDayHour[subgroup][day][hour])
								isNotAvailable=false;
						}
					}
					assert(!allActivities.isEmpty());
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
					} else{
						if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
						else{
							tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
						}
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(sty->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day]);
	tmpString+="\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;

			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.augmentedYearsList.at(year)->name)+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		currentCount=0;
		for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
			StudentsYear* sty=gt.rules.augmentedYearsList[year];
			if(!excludedNames.contains(year)){
				currentCount++;
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<year;
				QList<int> allActivities;
				allActivities.clear();
				bool isNotAvailable=true;
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
							allActivities<<students_timetable_weekly[subgroup][day][hour];
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
				}
				assert(!allActivities.isEmpty());
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, false, true, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day]);
	tmpString+="\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";

	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			excludedNames<<year;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			StudentsYear* sty=gt.rules.augmentedYearsList[year];
			tmpString+=protect2(sty->name)+"</th>\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;
				allActivities.clear();
				bool isNotAvailable=true;
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour])))
							allActivities<<students_timetable_weekly[subgroup][day][hour];
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
				}
				assert(!allActivities.isEmpty());
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivities[0], day, hour, isNotAvailable, true, false, printActivityTags, gt.rules.augmentedYearsList.at(year)->name);
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(sty->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+tr("All Activities")+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			if(activitiesAtTime[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+tr("All Activities")+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(activitiesAtTime[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

	
//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeVerticalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames){
QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	if(htmlLevel>=2)
		tmpString+="          <th class=\"xAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=tr("All Activities");
	tmpString+="</th>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td>"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(activitiesAtTime[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td colspan=\"2\"></td><td>"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeHorizontalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames){

	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay) +"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	}
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
		
	tmpString+="        <tr>\n";
	if(htmlLevel>=2)
		tmpString+="          <th class=\"yAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=tr("All Activities")+"</th>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(activitiesAtTime[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
			}
		}
	}
	if(repeatNames){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=tr("All Activities")+"</th>\n";
	}
	tmpString+="        </tr>\n";
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeVerticalDailyHtml(int htmlLevel, int day, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	if(htmlLevel>=2)
		tmpString+="          <th class=\"xAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=tr("All Activities");
	tmpString+="</th>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td>"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		if(activitiesAtTime[day][hour].isEmpty()){
			if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
				tmpString+=writeBreakSlot(htmlLevel, "");
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		} else {
			tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td>"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
		
	tmpString+="        <tr>\n";
	if(htmlLevel>=2)
		tmpString+="          <th class=\"yAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=tr("All Activities")+"</th>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(activitiesAtTime[day][hour].isEmpty()){
			if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
				tmpString+=writeBreakSlot(htmlLevel, "");
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		} else {
			tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTime[day][hour], printActivityTags);
		}
	}
	if(repeatNames){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=tr("All Activities")+"</th>\n";
	}
	tmpString+="        </tr>\n";
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleTeachersTimetableDaysHorizontalHtml(int htmlLevel, int teacher, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(teacher>=0);
	assert(teacher<gt.rules.nInternalTeachers);
	QString tmpString;
	QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
	tmpString+="    <table id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	if(teacher%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(teacher_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<teachers_timetable_weekly[teacher][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true, printActivityTags, teacher_name);
			} else{
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}


//by Volker Dirr
QString TimetableExport::singleTeachersTimetableDaysVerticalHtml(int htmlLevel, int teacher, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(teacher>=0);
	assert(teacher<gt.rules.nInternalTeachers);
	QString tmpString;
	QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
	tmpString+="    <table id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	if(teacher%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n";
	tmpString+="        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(teacher_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<teachers_timetable_weekly[teacher][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false, printActivityTags, teacher_name);
			} else{
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

	
//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeVerticalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalTeachersList[teacher]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			currentCount=0;
			for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
				if(!excludedNames.contains(teacher)){
					currentCount++;
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<teacher;
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<teachers_timetable_weekly[teacher][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true, printActivityTags, gt.rules.internalTeachersList[teacher]->name);
					} else {
						tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeHorizontalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			excludedNames<<teacher;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalTeachersList[teacher]->name)+"</th>\n";
			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<teachers_timetable_weekly[teacher][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false, printActivityTags, gt.rules.internalTeachersList[teacher]->name);
					} else {
						tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalTeachersList[teacher]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalTeachersList[teacher]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		currentCount=0;
		for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
			if(!excludedNames.contains(teacher)){
				currentCount++;
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<teacher;
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<teachers_timetable_weekly[teacher][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true, printActivityTags, gt.rules.internalTeachersList[teacher]->name);
				} else {
					tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	tmpString+="<th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			excludedNames<<teacher;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalTeachersList[teacher]->name)+"</th>\n";
		
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<teachers_timetable_weekly[teacher][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false, printActivityTags, gt.rules.internalTeachersList[teacher]->name);
				} else {
					tmpString+=writeActivitiesTeachers(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalTeachersList[teacher]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleRoomsTimetableDaysHorizontalHtml(int htmlLevel, int room, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(room>=0);
	assert(room<gt.rules.nInternalRooms);
	QString tmpString;
	QString room_name = gt.rules.internalRoomsList[room]->name;
	tmpString+="    <table id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	if(room%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(room_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<rooms_timetable_weekly[room][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true, printActivityTags);
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleRoomsTimetableDaysVerticalHtml(int htmlLevel, int room, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(room>=0);
	assert(room<gt.rules.nInternalRooms);
	QString tmpString;
	QString room_name = gt.rules.internalRoomsList[room]->name;
	tmpString+="    <table id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	if(room%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n";
	tmpString+="        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(room_name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;
			allActivities.clear();
			allActivities<<rooms_timetable_weekly[room][day][hour];
			bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
			if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false, printActivityTags);
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

	
//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeVerticalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalRoomsList[room]->name+"</th>";	
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			currentCount=0;
			for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
				if(!excludedNames.contains(room)){
					currentCount++;
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<room;
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<rooms_timetable_weekly[room][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true, printActivityTags);
					} else {
						tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeHorizontalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour]) + "</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			excludedNames<<room;

			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalRoomsList[room]->name)+"</th>\n";
			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;
					allActivities.clear();
					allActivities<<rooms_timetable_weekly[room][day][hour];
					bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
					if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
						tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false, printActivityTags);
					} else {
						tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
					}
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalRoomsList[room]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}


//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalRoomsList[room]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		currentCount=0;
		for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
			if(!excludedNames.contains(room)){
				currentCount++;
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<room;
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<rooms_timetable_weekly[room][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true, printActivityTags);
				} else {
					tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
				}
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";
	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			excludedNames<<room;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalRoomsList[room]->name)+"</th>\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;
				allActivities.clear();
				allActivities<<rooms_timetable_weekly[room][day][hour];
				bool activitiesWithSameStartingtime=addActivitiesWithSameStartingTime(allActivities, hour);
				if(allActivities.size()==1 && !activitiesWithSameStartingtime){  // because i am using colspan or rowspan!!!
					tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false, printActivityTags);
				} else {
					tmpString+=writeActivitiesRooms(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.internalRoomsList[room]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableDaysHorizontalHtml(int htmlLevel, int subject, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(subject>=0);
	assert(subject<gt.rules.nInternalSubjects);
	QString tmpString;
	///////by Liviu Lalescu
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentSubject[d][h].clear();
	foreach(int ai, gt.rules.activitiesForSubject[subject])
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentSubject[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu
	tmpString+="    <table id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	tmpString+="\" border=\"1\"";
	if(subject%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			
			allActivities=activitiesForCurrentSubject[day][hour];
			
			/*
			allActivities.clear();
			//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
			for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
				if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivities+students_timetable_weekly[subgroup][day][hour];
						}
				}
			}
			//Now run through the teachers timetable, because activities without a students set are still missing.
			for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
				if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
							assert(act->studentsNames.isEmpty());
							allActivities+teachers_timetable_weekly[teacher][day][hour];
						}
				}
			}*/
			addActivitiesWithSameStartingTime(allActivities, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr	
QString TimetableExport::singleSubjectsTimetableDaysVerticalHtml(int htmlLevel, int subject, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(subject>=0);
	assert(subject<gt.rules.nInternalSubjects);
	QString tmpString;
	///////by Liviu Lalescu
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentSubject[d][h].clear();
	foreach(int ai, gt.rules.activitiesForSubject[subject])
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentSubject[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu
	tmpString+="    <table id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	tmpString+="\" border=\"1\"";
	if(subject%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;

			allActivities=activitiesForCurrentSubject[day][hour];

			/*
			allActivities.clear();
			//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
			for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
				if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivities+students_timetable_weekly[subgroup][day][hour];
						}
				}
			}
			//Now run through the teachers timetable, because activities without a students set are still missing.
			for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
				if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
							assert(act->studentsNames.isEmpty());
							allActivities+teachers_timetable_weekly[teacher][day][hour];
						}
				}
			}
			*/
			addActivitiesWithSameStartingTime(allActivities, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

	
//by Volker Dirr	
QString TimetableExport::singleSubjectsTimetableTimeVerticalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){	
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalSubjectsList[subject]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";

			currentCount=0;
			for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
				currentCount++;
				if(!excludedNames.contains(subject)){	
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<subject;	
					QList<int> allActivities;
					allActivities.clear();
					
					foreach(int ai, gt.rules.activitiesForSubject[subject])
						if(activitiesAtTime[day][hour].contains(ai)){
							assert(!allActivities.contains(ai));
							allActivities.append(ai);
						}
					
					/* //Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
					for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
						if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
							Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
							if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
								if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
									allActivities+students_timetable_weekly[subgroup][day][hour];
								}
						}
					}
					//Now run through the teachers timetable, because activities without a students set are still missing.
					for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
						if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
							Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
							if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
								if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
									assert(act->studentsNames.isEmpty());
									allActivities+teachers_timetable_weekly[teacher][day][hour];
								}
						}
					}*/
					addActivitiesWithSameStartingTime(allActivities, hour);
					tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr	
QString TimetableExport::singleSubjectsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){	
			currentCount++;
			excludedNames<<subject;	
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
			else
				tmpString+="        <th>"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";

			///////by Liviu Lalescu
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					activitiesForCurrentSubject[d][h].clear();
			foreach(int ai, gt.rules.activitiesForSubject[subject])
				if(best_solution.times[ai]!=UNALLOCATED_TIME){
					int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
					int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
						activitiesForCurrentSubject[d][h+dd].append(ai);
				}
			///////end Liviu Lalescu

			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;

					allActivities=activitiesForCurrentSubject[day][hour];


					/*allActivities.clear();
					//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
					for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
						if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
							Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
							if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
								if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
									allActivities+students_timetable_weekly[subgroup][day][hour];
								}
						}
					}
					//Now run through the teachers timetable, because activities without a students set are still missing.
					for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
						if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
							Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
							if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
								if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
									assert(act->studentsNames.isEmpty());
									allActivities+teachers_timetable_weekly[teacher][day][hour];
								}
						}
					}*/
					addActivitiesWithSameStartingTime(allActivities, hour);
					tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
				else
					tmpString+="        <th>"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){	
			currentCount++;

			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalSubjectsList[subject]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";

		currentCount=0;
		for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
			currentCount++;
			if(!excludedNames.contains(subject)){	
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<subject;	
				QList<int> allActivities;
				allActivities.clear();
				
				foreach(int ai, gt.rules.activitiesForSubject[subject])
					if(activitiesAtTime[day][hour].contains(ai)){
						assert(!allActivities.contains(ai));
						allActivities.append(ai);
					}
				
				/*//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
								allActivities+students_timetable_weekly[subgroup][day][hour];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
								assert(act->studentsNames.isEmpty());
								allActivities+teachers_timetable_weekly[teacher][day][hour];
							}
					}
				}*/
				addActivitiesWithSameStartingTime(allActivities, hour);
				tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";

	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		currentCount++;
		if(!excludedNames.contains(subject)){
			excludedNames<<subject;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
			else
				tmpString+="        <th>"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";

			///////by Liviu Lalescu
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					activitiesForCurrentSubject[d][h].clear();
			foreach(int ai, gt.rules.activitiesForSubject[subject])
				if(best_solution.times[ai]!=UNALLOCATED_TIME){
					int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
					int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
						activitiesForCurrentSubject[d][h+dd].append(ai);
				}
			///////end Liviu Lalescu

			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;

				allActivities=activitiesForCurrentSubject[day][hour];


				/*allActivities.clear();
				//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
								allActivities+students_timetable_weekly[subgroup][day][hour];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
								assert(act->studentsNames.isEmpty());
								allActivities+teachers_timetable_weekly[teacher][day][hour];
							}
					}
				}*/
				addActivitiesWithSameStartingTime(allActivities, hour);
				tmpString+=writeActivitiesSubjects(htmlLevel, allActivities, printActivityTags);
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
				else
					tmpString+="        <th>"+protect2(gt.rules.internalSubjectsList[subject]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}


//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableDaysHorizontalHtml(int htmlLevel, int activityTag, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(activityTag>=0);
	assert(activityTag<gt.rules.nInternalActivityTags);
	QString tmpString;
	///////by Liviu Lalescu
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentActivityTag[d][h].clear();
	foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentActivityTag[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu
	tmpString+="    <table id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	tmpString+="\" border=\"1\"";
	if(activityTag%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QList<int> allActivities;
			
			allActivities=activitiesForCurrentActivityTag[day][hour];
			
			addActivitiesWithSameStartingTime(allActivities, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr	
QString TimetableExport::singleActivityTagsTimetableDaysVerticalHtml(int htmlLevel, int activityTag, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(activityTag>=0);
	assert(activityTag<gt.rules.nInternalActivityTags);
	QString tmpString;
	///////by Liviu Lalescu
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentActivityTag[d][h].clear();
	foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentActivityTag[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu
	tmpString+="    <table id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	tmpString+="\" border=\"1\"";
	if(activityTag%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			QList<int> allActivities;

			allActivities=activitiesForCurrentActivityTag[day][hour];

			addActivitiesWithSameStartingTime(allActivities, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

	
//by Volker Dirr	
QString TimetableExport::singleActivityTagsTimetableTimeVerticalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(!excludedNames.contains(activityTag)){	
			currentCount++;
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalActivityTagsList[activityTag]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";

			currentCount=0;
			for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
				currentCount++;
				if(!excludedNames.contains(activityTag)){	
					if(day+1==gt.rules.nDaysPerWeek && hour+1==gt.rules.nHoursPerDay)
						excludedNames<<activityTag;	
					QList<int> allActivities;
					allActivities.clear();
					
					foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
						if(activitiesAtTime[day][hour].contains(ai)){
							assert(!allActivities.contains(ai));
							allActivities.append(ai);
						}
					
					addActivitiesWithSameStartingTime(allActivities, hour);
					tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
				if(hour==0)
					tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr	
QString TimetableExport::singleActivityTagsTimetableTimeHorizontalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table\" border=\"1\">\n";

	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	}
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++)
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(!excludedNames.contains(activityTag)){	
			currentCount++;
			excludedNames<<activityTag;	
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
			else
				tmpString+="        <th>"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";

			///////by Liviu Lalescu
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					activitiesForCurrentActivityTag[d][h].clear();
			foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
				if(best_solution.times[ai]!=UNALLOCATED_TIME){
					int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
					int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
						activitiesForCurrentActivityTag[d][h+dd].append(ai);
				}
			///////end Liviu Lalescu

			for(int day=0; day<gt.rules.nDaysPerWeek; day++){
				for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
					QList<int> allActivities;

					allActivities=activitiesForCurrentActivityTag[day][hour];

					addActivitiesWithSameStartingTime(allActivities, hour);
					tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
				}
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
				else
					tmpString+="        <th>"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td colspan=\"2\"></td>";
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(!excludedNames.contains(activityTag)){	
			currentCount++;

			if(htmlLevel>=2)
				tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=gt.rules.internalActivityTagsList[activityTag]->name+"</th>";
		}
	}
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(hour==0)
			tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
		else tmpString+="          <!-- span -->\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";

		currentCount=0;
		for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
			currentCount++;
			if(!excludedNames.contains(activityTag)){	
				if(hour+1==gt.rules.nHoursPerDay)
					excludedNames<<activityTag;	
				QList<int> allActivities;
				allActivities.clear();
				
				foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
					if(activitiesAtTime[day][hour].contains(ai)){
						assert(!allActivities.contains(ai));
						allActivities.append(ai);
					}
				
				addActivitiesWithSameStartingTime(allActivities, hour);
				tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
			if(hour==0)
				tmpString+="        <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+ "\">"+protect2vert(gt.rules.daysOfTheWeek[day])+"</th>\n";
			else tmpString+="          <!-- span -->\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames){
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);
	QString tmpString;
	tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[day])+"\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td>";

	tmpString+="<th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(gt.rules.daysOfTheWeek[day])+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		currentCount++;
		if(!excludedNames.contains(activityTag)){
			excludedNames<<activityTag;
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
			else
				tmpString+="        <th>"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";

			///////by Liviu Lalescu
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					activitiesForCurrentActivityTag[d][h].clear();
			foreach(int ai, gt.rules.activitiesForActivityTagList[activityTag])
				if(best_solution.times[ai]!=UNALLOCATED_TIME){
					int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
					int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
						activitiesForCurrentActivityTag[d][h+dd].append(ai);
				}
			///////end Liviu Lalescu

			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				QList<int> allActivities;

				allActivities=activitiesForCurrentActivityTag[day][hour];

				addActivitiesWithSameStartingTime(allActivities, hour);
				tmpString+=writeActivitiesActivityTags(htmlLevel, allActivities, printActivityTags);
			}
			if(repeatNames){
				if(htmlLevel>=2)
					tmpString+="        <th class=\"yAxis\">"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
				else
					tmpString+="        <th>"+protect2(gt.rules.internalActivityTagsList[activityTag]->name)+"</th>\n";
			}
			tmpString+="        </tr>\n";
		}
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersFreePeriodsTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames){
	QString tmpString;
	tmpString+="    <table id=\"table\" border=\"1\">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Teachers' Free Periods")+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			bool empty_slot;
			empty_slot=true;
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					empty_slot=false;
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot) tmpString+="          <td>";
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					if(htmlLevel>=2)
						tmpString+="<div class=\"DESCRIPTION\">";
					switch(tfp){
						case TEACHER_HAS_SINGLE_GAP		: tmpString+=TimetableExport::tr("Single gap"); break;
						case TEACHER_HAS_BORDER_GAP		: tmpString+=TimetableExport::tr("Border gap"); break;
						case TEACHER_HAS_BIG_GAP		: tmpString+=TimetableExport::tr("Big gap"); break;
						case TEACHER_MUST_COME_EARLIER		: tmpString+=TimetableExport::tr("Must come earlier"); break;
						case TEACHER_MUST_STAY_LONGER		: tmpString+=TimetableExport::tr("Must stay longer"); break;
						case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+=TimetableExport::tr("Must come much earlier"); break;
						case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+=TimetableExport::tr("Must stay much longer"); break;
						case TEACHER_HAS_A_FREE_DAY		: tmpString+=TimetableExport::tr("Free day"); break;
						case TEACHER_IS_NOT_AVAILABLE		: tmpString+=TimetableExport::tr("Not available", "It refers to a teacher"); break;
						default: assert(0==1); break;
					}
					if(htmlLevel>=2)
						tmpString+=":</div>";
					else tmpString+=":<br />";
					if(htmlLevel>=3)
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tmpString+="<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
							case TEACHER_HAS_BORDER_GAP		: tmpString+="<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
							case TEACHER_HAS_BIG_GAP		: tmpString+="<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
							case TEACHER_MUST_COME_EARLIER		: tmpString+="<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
							case TEACHER_MUST_STAY_LONGER		: tmpString+="<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+="<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+="<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
							case TEACHER_HAS_A_FREE_DAY		: tmpString+="<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
							case TEACHER_IS_NOT_AVAILABLE		: tmpString+="<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
							default: assert(0==1); break;
						}
					for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][day][hour].size(); t++){
						QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][day][hour].at(t)]->name;
							switch(htmlLevel){
								case 4 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\">"+protect2(teacher_name)+"</span>"; break;
								case 5 : ;
								case 6 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(teacher_name)+"')\">"+protect2(teacher_name)+"</span>"; break;
								default: tmpString+=protect2(teacher_name); break;
							}
						tmpString+="<br />";
					}
					if(htmlLevel>=3)
						tmpString+="</div>";
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot){
				tmpString+="</td>\n";
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}
	
//by Volker Dirr
QString TimetableExport::singleTeachersFreePeriodsTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames){
	QString tmpString;
	
	tmpString+="    <table id=\"table\" border=\"1\">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Teachers' Free Periods")+"</th>";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			bool empty_slot;
			empty_slot=true;
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					empty_slot=false;
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot) tmpString+="          <td>";
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					if(htmlLevel>=2)
						tmpString+="<div class=\"DESCRIPTION\">";
					switch(tfp){
						case TEACHER_HAS_SINGLE_GAP		: tmpString+=TimetableExport::tr("Single gap"); break;
						case TEACHER_HAS_BORDER_GAP		: tmpString+=TimetableExport::tr("Border gap"); break;
						case TEACHER_HAS_BIG_GAP		: tmpString+=TimetableExport::tr("Big gap"); break;
						case TEACHER_MUST_COME_EARLIER		: tmpString+=TimetableExport::tr("Must come earlier"); break;
						case TEACHER_MUST_STAY_LONGER		: tmpString+=TimetableExport::tr("Must stay longer"); break;
						case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+=TimetableExport::tr("Must come much earlier"); break;
						case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+=TimetableExport::tr("Must stay much longer"); break;
						case TEACHER_HAS_A_FREE_DAY		: tmpString+=TimetableExport::tr("Free day"); break;
						case TEACHER_IS_NOT_AVAILABLE		: tmpString+=TimetableExport::tr("Not available"); break;
						default: assert(0==1); break;
					}
					if(htmlLevel>=2)
						tmpString+=":</div>";
					else tmpString+=":<br />";
					if(htmlLevel>=3)
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tmpString+="<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
							case TEACHER_HAS_BORDER_GAP		: tmpString+="<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
							case TEACHER_HAS_BIG_GAP		: tmpString+="<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
							case TEACHER_MUST_COME_EARLIER		: tmpString+="<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
							case TEACHER_MUST_STAY_LONGER		: tmpString+="<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+="<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+="<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
							case TEACHER_HAS_A_FREE_DAY		: tmpString+="<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
							case TEACHER_IS_NOT_AVAILABLE		: tmpString+="<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
							default: assert(0==1); break;
						}
					for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][day][hour].size(); t++){
						QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][day][hour].at(t)]->name;
							switch(htmlLevel){
								case 4 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\">"+protect2(teacher_name)+"</span>"; break;
								case 5 : ;
								case 6 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(teacher_name)+"')\">"+protect2(teacher_name)+"</span>"; break;
								default: tmpString+=protect2(teacher_name); break;
							}
						tmpString+="<br />";
					}
					if(htmlLevel>=3)
						tmpString+="</div>";
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot){
				tmpString+="</td>\n";
			} else
				tmpString+=writeEmpty(htmlLevel);
		}
		if(repeatNames){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
	if(repeatNames){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll){
	Q_UNUSED(htmlLevel);
	QString tmpString;
	if(!printAll){
		tmpString+="    <p>\n";
		tmpString+="      <strong>"+tr("This is a teaser only. Values are not correct!")+"</strong>\n";
		tmpString+="    </p>\n";
	}
	tmpString+="    <p>\n";
	tmpString+="      "+tr("This file doesn't list limits that are set by constraints. It contains statistics about the min and max values of the currently calculated solution.")+"\n";
	tmpString+="    </p>\n";
	
	QString teachersString="";
	int freeDaysAllTeachers=0;
	int minFreeDaysAllTeachers=gt.rules.nDaysPerWeek;
	int maxFreeDaysAllTeachers=0;
	int gapsAllTeachers=0;
	int minGapsPerDayAllTeachers=gt.rules.nHoursPerDay;
	int maxGapsPerDayAllTeachers=0;
	int minGapsPerWeekAllTeachers=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxGapsPerWeekAllTeachers=0;
	int minHoursPerDayAllTeachers=gt.rules.nHoursPerDay;
	int maxHoursPerDayAllTeachers=0;
	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		int freeDaysSingleTeacher=0;
		int gapsSingleTeacher=0;
		int minGapsPerDaySingleTeacher=gt.rules.nHoursPerDay;
		int maxGapsPerDaySingleTeacher=0;
		int minHoursPerDaySingleTeacher=gt.rules.nHoursPerDay;
		int maxHoursPerDaySingleTeacher=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			int firstPeriod=-1;
			int lastPeriod=-1;
			int gapsPerDaySingleTeacher=0;
			int hoursPerDaySingleTeacher=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(teachers_timetable_weekly[tch][d][h]!=UNALLOCATED_ACTIVITY){
					if(firstPeriod==-1)
						firstPeriod=h;
					lastPeriod=h;
					hoursPerDaySingleTeacher++;
				}
			}
			if(firstPeriod==-1){
				freeDaysSingleTeacher++;
			} else {
				for(int h=firstPeriod; h<lastPeriod; h++){
					if(teachers_timetable_weekly[tch][d][h]==UNALLOCATED_ACTIVITY && teacherNotAvailableDayHour[tch][d][h]==false && breakDayHour[d][h]==false){
						gapsPerDaySingleTeacher++;
					}
				}
			}
			gapsSingleTeacher+=gapsPerDaySingleTeacher;
			if(minGapsPerDaySingleTeacher>gapsPerDaySingleTeacher)
				minGapsPerDaySingleTeacher=gapsPerDaySingleTeacher;
			if(maxGapsPerDaySingleTeacher<gapsPerDaySingleTeacher)
				maxGapsPerDaySingleTeacher=gapsPerDaySingleTeacher;
			if(hoursPerDaySingleTeacher>0){
				if(minHoursPerDaySingleTeacher>hoursPerDaySingleTeacher)
					minHoursPerDaySingleTeacher=hoursPerDaySingleTeacher;
				if(maxHoursPerDaySingleTeacher<hoursPerDaySingleTeacher)
					maxHoursPerDaySingleTeacher=hoursPerDaySingleTeacher;
			}
		}
		if(minFreeDaysAllTeachers>freeDaysSingleTeacher)
			minFreeDaysAllTeachers=freeDaysSingleTeacher;
		if(maxFreeDaysAllTeachers<freeDaysSingleTeacher)
			maxFreeDaysAllTeachers=freeDaysSingleTeacher;
		
		if(minGapsPerDayAllTeachers>minGapsPerDaySingleTeacher)
			minGapsPerDayAllTeachers=minGapsPerDaySingleTeacher;
		if(maxGapsPerDayAllTeachers<maxGapsPerDaySingleTeacher)
			maxGapsPerDayAllTeachers=maxGapsPerDaySingleTeacher;
			
		if(minGapsPerWeekAllTeachers>gapsSingleTeacher)
			minGapsPerWeekAllTeachers=gapsSingleTeacher;
		if(maxGapsPerWeekAllTeachers<gapsSingleTeacher)
			maxGapsPerWeekAllTeachers=gapsSingleTeacher;
		
		if(minHoursPerDayAllTeachers>minHoursPerDaySingleTeacher)
			minHoursPerDayAllTeachers=minHoursPerDaySingleTeacher;
		if(maxHoursPerDayAllTeachers<maxHoursPerDaySingleTeacher)
			maxHoursPerDayAllTeachers=maxHoursPerDaySingleTeacher;

		gapsAllTeachers+=gapsSingleTeacher;
		freeDaysAllTeachers+=freeDaysSingleTeacher;
		
		if(detailed){
			if(freeDaysSingleTeacher==gt.rules.nDaysPerWeek)
				minHoursPerDaySingleTeacher=0;
			teachersString+="      <tr><th>"+protect2(gt.rules.internalTeachersList[tch]->name)
					+"</th><td>"+QString::number(freeDaysSingleTeacher)
								+"</td><td>"+QString::number(gapsSingleTeacher)
								+"</td><td>"+QString::number(minGapsPerDaySingleTeacher)
								+"</td><td>"+QString::number(maxGapsPerDaySingleTeacher)
								+"</td><td>"+QString::number(minHoursPerDaySingleTeacher)
								+"</td><td>"+QString::number(maxHoursPerDaySingleTeacher)
								+"</td>";
			
			if(repeatNames){
				teachersString+="<th>"+protect2(gt.rules.internalTeachersList[tch]->name)+"</th>";
			}
			teachersString+="</tr>\n";
		}
		if(!printAll && tch>10){
			break;
		}
	}
	
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n";
	tmpString+="        <tr><th>"+tr("All teachers")
		  +"</th><th>"+tr("Free days")
		  +"</th><th>"+tr("Gaps")
		  +"</th><th>"+tr("Gaps per day")
		  +"</th><th>"+tr("Hours per day")
		  +"</th></tr>\n";
	tmpString+="      </thead>\n";
	tmpString+="      <tr><th>"+tr("Sum")+"</th>";
	tmpString+="<td>"+QString::number(freeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(gapsAllTeachers)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Average")+"</th>";
	tmpString+="<td>"+QString::number(double(freeDaysAllTeachers)/gt.rules.nInternalTeachers,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(gapsAllTeachers)/gt.rules.nInternalTeachers,'f',2)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Min")+"</th>";
	tmpString+="<td>"+QString::number(minFreeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerDayAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minHoursPerDayAllTeachers)+"</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Max")+"</th>";
	tmpString+="<td>"+QString::number(maxFreeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerDayAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxHoursPerDayAllTeachers)+"</td>";
	tmpString+="</tr>\n";
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(4)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
        tmpString+="    </table>\n";
	
	if(detailed){
		tmpString+="    <p class=\"back0\"><br /></p>\n\n";
		
		tmpString+="    <table border=\"1\">\n";
		tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
		tmpString+="      <thead>\n";
		tmpString+="        <tr><th>"+tr("Teacher")
			+"</th><th>"+tr("Free days")
			+"</th><th>"+tr("Total gaps")
			+"</th><th>"+tr("Min gaps per day")
			+"</th><th>"+tr("Max gaps per day")
			+"</th><th>"+tr("Min hours per day")
			+"</th><th>"+tr("Max hours per day")
			+"</th>";
		if(repeatNames){
			tmpString+="<th>"+tr("Teacher")+"</th>";
		}
		tmpString+="</tr>\n";
		tmpString+="      </thead>\n";
		tmpString+=teachersString;
		//workaround begin.
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(6)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
		if(repeatNames){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
		//workaround end.
		tmpString+="    </table>\n";
	}
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleStudentsStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll){
	Q_UNUSED(htmlLevel);
	QString tmpString;
	if(!printAll){
		tmpString+="    <p>\n";
		tmpString+="      <strong>"+tr("This is a teaser only. Values are not correct!")+"</strong>\n";
		tmpString+="    </p>\n";
	}
	tmpString+="    <p>\n";
	tmpString+="      "+tr("This file doesn't list limits that are set by constraints. It contains statistics about the min and max values of the currently calculated solution.")+"\n";
	tmpString+="    </p>\n";
	
	//subgroups statistics (start)
	QString subgroupsString="";
	int freeDaysAllSubgroups=0;
	int minFreeDaysAllSubgroups=gt.rules.nDaysPerWeek;
	int maxFreeDaysAllSubgroups=0;
	int gapsAllSubgroups=0;
	int minGapsPerDayAllSubgroups=gt.rules.nHoursPerDay;
	int maxGapsPerDayAllSubgroups=0;
	int minGapsPerWeekAllSubgroups=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxGapsPerWeekAllSubgroups=0;
	int minHoursPerDayAllSubgroups=gt.rules.nHoursPerDay;
	int maxHoursPerDayAllSubgroups=0;
	QList<int> freeDaysPerWeekSubgroupList;
	QList<int> gapsPerWeekSubgroupList;
	QList<int> minGapsPerDaySubgroupList;
	QList<int> maxGapsPerDaySubgroupList;
	QList<int> minHoursPerDaySubgroupList;
	QList<int> maxHoursPerDaySubgroupList;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		int freeDaysSingleSubgroup=0;
		int gapsSingleSubgroup=0;
		int minGapsPerDaySingleSubgroup=gt.rules.nHoursPerDay;
		int maxGapsPerDaySingleSubgroup=0;
		int minHoursPerDaySingleSubgroup=gt.rules.nHoursPerDay;
		int maxHoursPerDaySingleSubgroup=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			int firstPeriod=-1;
			int lastPeriod=-1;
			int gapsPerDaySingleSubgroup=0;
			int hoursPerDaySingleSubgroup=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(students_timetable_weekly[subgroup][d][h]!=UNALLOCATED_ACTIVITY){
					if(firstPeriod==-1)
						firstPeriod=h;
					lastPeriod=h;
					hoursPerDaySingleSubgroup++;
				}
			}
			if(firstPeriod==-1){
				freeDaysSingleSubgroup++;
			} else {
				for(int h=firstPeriod; h<lastPeriod; h++){
					if(students_timetable_weekly[subgroup][d][h]==UNALLOCATED_ACTIVITY && subgroupNotAvailableDayHour[subgroup][d][h]==false && breakDayHour[d][h]==false){
						gapsPerDaySingleSubgroup++;
					}
				}
			}
			gapsSingleSubgroup+=gapsPerDaySingleSubgroup;
			if(minGapsPerDaySingleSubgroup>gapsPerDaySingleSubgroup)
				minGapsPerDaySingleSubgroup=gapsPerDaySingleSubgroup;
			if(maxGapsPerDaySingleSubgroup<gapsPerDaySingleSubgroup)
				maxGapsPerDaySingleSubgroup=gapsPerDaySingleSubgroup;
			if(hoursPerDaySingleSubgroup>0){
				if(minHoursPerDaySingleSubgroup>hoursPerDaySingleSubgroup)
					minHoursPerDaySingleSubgroup=hoursPerDaySingleSubgroup;
				if(maxHoursPerDaySingleSubgroup<hoursPerDaySingleSubgroup)
					maxHoursPerDaySingleSubgroup=hoursPerDaySingleSubgroup;
			}
		}
		if(minFreeDaysAllSubgroups>freeDaysSingleSubgroup)
			minFreeDaysAllSubgroups=freeDaysSingleSubgroup;
		if(maxFreeDaysAllSubgroups<freeDaysSingleSubgroup)
			maxFreeDaysAllSubgroups=freeDaysSingleSubgroup;
		
		if(minGapsPerDayAllSubgroups>minGapsPerDaySingleSubgroup)
			minGapsPerDayAllSubgroups=minGapsPerDaySingleSubgroup;
		if(maxGapsPerDayAllSubgroups<maxGapsPerDaySingleSubgroup)
			maxGapsPerDayAllSubgroups=maxGapsPerDaySingleSubgroup;
			
		if(minGapsPerWeekAllSubgroups>gapsSingleSubgroup)
			minGapsPerWeekAllSubgroups=gapsSingleSubgroup;
		if(maxGapsPerWeekAllSubgroups<gapsSingleSubgroup)
			maxGapsPerWeekAllSubgroups=gapsSingleSubgroup;
		
		if(minHoursPerDayAllSubgroups>minHoursPerDaySingleSubgroup)
			minHoursPerDayAllSubgroups=minHoursPerDaySingleSubgroup;
		if(maxHoursPerDayAllSubgroups<maxHoursPerDaySingleSubgroup)
			maxHoursPerDayAllSubgroups=maxHoursPerDaySingleSubgroup;

		gapsAllSubgroups+=gapsSingleSubgroup;
		freeDaysAllSubgroups+=freeDaysSingleSubgroup;
		
		if(freeDaysSingleSubgroup==gt.rules.nDaysPerWeek)
			minHoursPerDaySingleSubgroup=0;
		if(detailed){
			subgroupsString+="      <tr><th>"+protect2(gt.rules.internalSubgroupsList[subgroup]->name)
						+"</th><td>"+QString::number(freeDaysSingleSubgroup)
							 +"</td><td>"+QString::number(gapsSingleSubgroup)
							 +"</td><td>"+QString::number(minGapsPerDaySingleSubgroup)
							 +"</td><td>"+QString::number(maxGapsPerDaySingleSubgroup)
							 +"</td><td>"+QString::number(minHoursPerDaySingleSubgroup)
							 +"</td><td>"+QString::number(maxHoursPerDaySingleSubgroup)
							 +"</td>";
			if(repeatNames){
				subgroupsString+="<th>"+protect2(gt.rules.internalSubgroupsList[subgroup]->name)+"</th>";
			}
			subgroupsString+="</tr>\n";
			freeDaysPerWeekSubgroupList<<freeDaysSingleSubgroup;
			gapsPerWeekSubgroupList<<gapsSingleSubgroup;
			minGapsPerDaySubgroupList<<minGapsPerDaySingleSubgroup;
			maxGapsPerDaySubgroupList<<maxGapsPerDaySingleSubgroup;
			minHoursPerDaySubgroupList<<minHoursPerDaySingleSubgroup;
			maxHoursPerDaySubgroupList<<maxHoursPerDaySingleSubgroup;
		}
		if(!printAll && subgroup>10){
			break;
		}
	}

	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n";
	tmpString+="        <tr><th>"+tr("All students")
		  +"</th><th>"+tr("Free days")
		  +"</th><th>"+tr("Gaps")
		  +"</th><th>"+tr("Gaps per day")
		  +"</th><th>"+tr("Hours per day")
		  +"</th></tr>\n";
	tmpString+="      </thead>\n";
	tmpString+="      <tr><th>"+tr("Sum")+"</th>";
	tmpString+="<td>"+QString::number(freeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(gapsAllSubgroups)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Average")+"</th>";
	tmpString+="<td>"+QString::number(double(freeDaysAllSubgroups)/gt.rules.nInternalSubgroups,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(gapsAllSubgroups)/gt.rules.nInternalSubgroups,'f',2)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Min")+"</th>";
	tmpString+="<td>"+QString::number(minFreeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerDayAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minHoursPerDayAllSubgroups)+"</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+tr("Max")+"</th>";
	tmpString+="<td>"+QString::number(maxFreeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerDayAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxHoursPerDayAllSubgroups)+"</td>";
	tmpString+="</tr>\n";
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(4)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
        tmpString+="    </table>\n";
	
	tmpString+="    <p class=\"back0\"><br /></p>\n\n";
	
	//subgroups statistics (end)
	
	if(detailed){
		if(!printAll){
			//similar to source in else part (start)
			tmpString+="    <p></p>\n";
			tmpString+="    <table border=\"1\">\n";
			tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			tmpString+="      <thead>\n";
			tmpString+="      <tr><th>"+tr("Subgroup")
					+"</th><th>"+tr("Free days")
					+"</th><th>"+tr("Total gaps")
					+"</th><th>"+tr("Min gaps per day")
					+"</th><th>"+tr("Max gaps per day")
					+"</th><th>"+tr("Min hours per day")
					+"</th><th>"+tr("Max hours per day")
					+"</th>";
			if(repeatNames){
				tmpString+="<td>"+tr("Subgroup")+"</td>";
			}
			tmpString+="</tr>\n";
			tmpString+="      </thead>\n";
			tmpString+=subgroupsString;
			//workaround begin.
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(6)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
			if(repeatNames){
				tmpString+="<td></td>";
			}
			tmpString+="</tr>\n";
			//workaround end.
			tmpString+="    </table>\n";
			//similar to source in else part (end)
		} else {
			//groups and years statistics (start)
			QString yearsString="    <table border=\"1\">\n";
			yearsString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			yearsString+="      <thead>\n";
			yearsString+="      <tr><th>"+tr("Year")
							+"</th><th>"+tr("Min free days")
							+"</th><th>"+tr("Max free days")
							+"</th><th>"+tr("Min hours per day")
							+"</th><th>"+tr("Max hours per day")
							+"</th><th>"+tr("Min gaps per week")
							+"</th><th>"+tr("Max gaps per week")
							+"</th><th>"+tr("Min gaps per day")
							+"</th><th>"+tr("Max gaps per day")
							+"</th>";
			if(repeatNames){
					yearsString+="<th>"+tr("Year")+"</th>";
			}
			yearsString+="</tr>\n";
			yearsString+="      </thead>\n";
			QString groupsString="    <table border=\"1\">\n";
			groupsString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			groupsString+="      <thead>\n";
			groupsString+="      <tr><th>"+tr("Group")
							+"</th><th>"+tr("Min free days")
							+"</th><th>"+tr("Max free days")
							+"</th><th>"+tr("Min hours per day")
							+"</th><th>"+tr("Max hours per day")
							+"</th><th>"+tr("Min gaps per week")
							+"</th><th>"+tr("Max gaps per week")
							+"</th><th>"+tr("Min gaps per day")
							+"</th><th>"+tr("Max gaps per day")
							+"</th>";
			if(repeatNames){
				groupsString+="<th>"+tr("Group")+"</th>";
			}
			groupsString+="</tr>\n";
			groupsString+="      </thead>\n";
			for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
				StudentsYear* sty=gt.rules.augmentedYearsList[i];
				int minFreeDaysPerWeekYear=gt.rules.nDaysPerWeek;
				int maxFreeDaysPerWeekYear=0;
				int minGapsPerDayYear=gt.rules.nHoursPerDay;
				int maxGapsPerDayYear=0;
				int minGapsPerWeekYear=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
				int maxGapsPerWeekYear=0;
				int minHoursPerDayYear=gt.rules.nHoursPerDay;
				int maxHoursPerDayYear=0;
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					int minFreeDaysPerWeekGroup=gt.rules.nDaysPerWeek;
					int maxFreeDaysPerWeekGroup=0;
					int minGapsPerDayGroup=gt.rules.nHoursPerDay;
					int maxGapsPerDayGroup=0;
					int minGapsPerWeekGroup=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
					int maxGapsPerWeekGroup=0;
					int minHoursPerDayGroup=gt.rules.nHoursPerDay;
					int maxHoursPerDayGroup=0;
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;

						if(minFreeDaysPerWeekGroup>freeDaysPerWeekSubgroupList.at(subgroup))
							minFreeDaysPerWeekGroup=freeDaysPerWeekSubgroupList.at(subgroup);
						if(maxFreeDaysPerWeekGroup<freeDaysPerWeekSubgroupList.at(subgroup))
							maxFreeDaysPerWeekGroup=freeDaysPerWeekSubgroupList.at(subgroup);
						
						if(minHoursPerDayGroup>minHoursPerDaySubgroupList.at(subgroup))
							minHoursPerDayGroup=minHoursPerDaySubgroupList.at(subgroup);
						if(maxHoursPerDayGroup<maxHoursPerDaySubgroupList.at(subgroup))
							maxHoursPerDayGroup=maxHoursPerDaySubgroupList.at(subgroup);
						
						if(minGapsPerWeekGroup>gapsPerWeekSubgroupList.at(subgroup))
							minGapsPerWeekGroup=gapsPerWeekSubgroupList.at(subgroup);
						if(maxGapsPerWeekGroup<gapsPerWeekSubgroupList.at(subgroup))
							maxGapsPerWeekGroup=gapsPerWeekSubgroupList.at(subgroup);
						
						if(minGapsPerDayGroup>minGapsPerDaySubgroupList.at(subgroup))
							minGapsPerDayGroup=minGapsPerDaySubgroupList.at(subgroup);
						if(maxGapsPerDayGroup<maxGapsPerDaySubgroupList.at(subgroup))
							maxGapsPerDayGroup=maxGapsPerDaySubgroupList.at(subgroup);
					}
					//print groups
					groupsString+="      <tr><th>"+protect2(stg->name)+"</th><td>"
					+QString::number(minFreeDaysPerWeekGroup)+"</td><td>"+QString::number(maxFreeDaysPerWeekGroup)+"</td><td>"
					+QString::number(minHoursPerDayGroup)+"</td><td>"+QString::number(maxHoursPerDayGroup)+"</td><td>"
					+QString::number(minGapsPerWeekGroup)+"</td><td>"+QString::number(maxGapsPerWeekGroup)+"</td><td>"
					+QString::number(minGapsPerDayGroup)+"</td><td>"+QString::number(maxGapsPerDayGroup)+"</td>";
					if(repeatNames){
						groupsString+="<th>"+protect2(stg->name)+"</th>";
					}
					groupsString+="</tr>\n";

					//check years
					if(minFreeDaysPerWeekYear>minFreeDaysPerWeekGroup)
						minFreeDaysPerWeekYear=minFreeDaysPerWeekGroup;
					if(maxFreeDaysPerWeekYear<maxFreeDaysPerWeekGroup)
						maxFreeDaysPerWeekYear=maxFreeDaysPerWeekGroup;
					
					if(minHoursPerDayYear>minHoursPerDayGroup)
						minHoursPerDayYear=minHoursPerDayGroup;
					if(maxHoursPerDayYear<maxHoursPerDayGroup)
						maxHoursPerDayYear=maxHoursPerDayGroup;
						
					if(minGapsPerWeekYear>minGapsPerWeekGroup)
						minGapsPerWeekYear=minGapsPerWeekGroup;
					if(maxGapsPerWeekYear<maxGapsPerWeekGroup)
						maxGapsPerWeekYear=maxGapsPerWeekGroup;
						
					if(minGapsPerDayYear>minGapsPerDayGroup)
						minGapsPerDayYear=minGapsPerDayGroup;
					if(maxGapsPerDayYear<maxGapsPerDayGroup)
						maxGapsPerDayYear=maxGapsPerDayGroup;
				}
				//print years
					yearsString+="      <tr><th>"+protect2(sty->name)+"</th><td>"
					+QString::number(minFreeDaysPerWeekYear)+"</td><td>"+QString::number(maxFreeDaysPerWeekYear)+"</td><td>"
					+QString::number(minHoursPerDayYear)+"</td><td>"+QString::number(maxHoursPerDayYear)+"</td><td>"
					+QString::number(minGapsPerWeekYear)+"</td><td>"+QString::number(maxGapsPerWeekYear)+"</td><td>"
					+QString::number(minGapsPerDayYear)+"</td><td>"+QString::number(maxGapsPerDayYear)+"</td>";
					if(repeatNames){
						yearsString+="<th>"+protect2(sty->name)+"</th>";
					}
					yearsString+="</tr>\n";
			}
			//workaround begin.
			groupsString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(8)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
			if(repeatNames){
				groupsString+="<td></td>";
			}
			groupsString+="</tr>\n";
			//workaround end.
			groupsString+="    </table>\n";
			groupsString+="    <p class=\"back0\"><br /></p>\n\n";
			//workaround begin.
			yearsString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(8)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
			if(repeatNames){
				yearsString+="<td></td>";
			}
			yearsString+="</tr>\n";
			//workaround end.
			yearsString+="    </table>\n";
			yearsString+="    <p class=\"back0\"><br /></p>\n\n";
			tmpString+=yearsString;
			tmpString+=groupsString;
			//similar to source in if part (start)
			tmpString+="    <p></p>\n";
			tmpString+="    <table border=\"1\">\n";
			tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			tmpString+="      <thead>\n";
			tmpString+="      <tr><th>"+tr("Subgroup")
					+"</th><th>"+tr("Free days")
					+"</th><th>"+tr("Total gaps")
					+"</th><th>"+tr("Min gaps per day")
					+"</th><th>"+tr("Max gaps per day")
					+"</th><th>"+tr("Min hours per day")
					+"</th><th>"+tr("Max hours per day")
					+"</th>";
			if(repeatNames){
				tmpString+="<td>"+tr("Subgroup")+"</td>";
			}
			tmpString+="</tr>\n";
			tmpString+="      </thead>\n";
			tmpString+=subgroupsString;
			//workaround begin.
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(6)+"\">"+TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td>";
			if(repeatNames){
				tmpString+="<td></td>";
			}
			tmpString+="</tr>\n";
			//workaround end.
			tmpString+="    </table>\n";
			//similar to source in if part (end)
		}
	}
	//groups and years statistics (end)
	return tmpString;
}
