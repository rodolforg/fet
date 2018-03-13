/*
File timetable_defs.h
*/

/***************************************************************************
                          timetable_defs.h  -  description
                             -------------------
    begin                : Sat Mar 15 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLE_DEFS_H
#define TIMETABLE_DEFS_H

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

#include <QString>

/**
The version number
*/
extern const QString FET_VERSION;

/**
The language
*/
extern QString FET_LANGUAGE;

extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

extern QString LANGUAGE_FOR_HTML;

extern bool SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS;

extern bool SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES;

extern bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS;

extern bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER;

extern bool VERBOSE;

/**
The maximum total number of different subgroups of students
*/
const int MAX_TOTAL_SUBGROUPS=30000;//MAX_YEARS*MAX_GROUPS_PER_YEAR*MAX_SUBGROUPS_PER_GROUP;

const int MAX_ROOM_CAPACITY=30000;

/**
The maximum number of different teachers
*/
const int MAX_TEACHERS=6000;

/**
The maximum number of activities
DEPRECATED COMMENT BELOW
IMPORTANT: must be qint16 (max 32767), because we are using qint16 for each activity index and for
unallocated activity = max_activities
*/
const int MAX_ACTIVITIES=30000;

//if you want to increase this, you also need to modify the add/modify activity dialogs, to permit larger values
//(add more pages in the subactivities tab).
const int MAX_SPLIT_OF_AN_ACTIVITY=35;

/**
The maximum number of rooms
DEPRECATED COMMENT BELOW
IMPORTANT: max_rooms+1 must be qint16 (max 32766 for max_rooms), because we are using qint16 for each room index and
for unallocated space = max_rooms and for unspecified room = max_rooms+1
*/
const int MAX_ROOMS=6000;

/**
The maximum number of buildings
*/
const int MAX_BUILDINGS=6000;

/**
This constant represents an unallocated activity
*/
const int UNALLOCATED_ACTIVITY = MAX_ACTIVITIES;

/**
The maximum number of working hours per day.
DEPRECATED COMMENT BELOW
IMPORTANT: max hours per day * max days per week = max hours per week must be qint16 (max 32767),
because each time is qint16 and unallocated time is qint16
*/
const int MAX_HOURS_PER_DAY=96;

/**
The maximum number of working days per week.
DEPRECATED COMMENT BELOW
IMPORTANT: max hours per day * max days per week = max hours per week must be qint16 (max 32767)
because each time is qint16 and unallocated time is qint16
*/
const int MAX_DAYS_PER_WEEK=35;

/**
The predefined names of the days of the week
*/
extern const QString PREDEFINED_DAYS_OF_THE_WEEK[];

/**
The maximum number of working hours in a week.

Hours in a week are arranged like this:
         Mo Tu We Th Fr
1        0  1  2  3  4
2        5  6  7  8  9
3        10 11 12 13 14
4        15 16 17 18 19
5        20 21 22 23 24
6        25 26 27 28 29 etc.

DEPRECATED COMMENT BELOW
IMPORTANT: MAX_HOURS_PER_DAY * MAX_DAYS_PER_WEEK == MAX_HOURS_PER_WEEK must be qint16 (max 32767)
because each time is qint16 and unallocated time is qint16
*/
const int MAX_HOURS_PER_WEEK = MAX_HOURS_PER_DAY * MAX_DAYS_PER_WEEK;

/**
This constant represents unallocated time for an activity
*/
const int UNALLOCATED_TIME = MAX_HOURS_PER_WEEK;

/**
This constant represents unallocated space for an activity
*/
const int UNALLOCATED_SPACE = MAX_ROOMS;

const int UNSPECIFIED_ROOM = MAX_ROOMS+1;

/**
File and directory separator
*/
extern const QString FILE_SEP;

/**
The timetable's rules input file name
*/
extern QString INPUT_FILENAME_XML;

/**
The working directory
*/
extern QString WORKING_DIRECTORY;

/**
The import directory
*/
extern QString IMPORT_DIRECTORY;

//OUTPUT FILES

/**
The output directory. Please be careful when editing it,
because the functions add a FILE_SEP sign at the end of it
and then the name of a file. If you make OUTPUT_DIR="",
there might be problems.
*/
extern QString OUTPUT_DIR;

/**
A log file explaining how the xml input file was parsed
*/
extern const QString XML_PARSING_LOG_FILENAME;

/**
A function used in xml saving
*/
QString protect(const QString& str);

/**
A function used in html saving
*/
QString protect2(const QString& str);

/**
A function used in html saving
*/
QString protect2vert(const QString& str);

/**
This constants represents free periods of a teacher in the teachers free periods timetable
*/
const int TEACHER_HAS_SINGLE_GAP = 0;
const int TEACHER_HAS_BORDER_GAP = 1;
const int TEACHER_HAS_BIG_GAP = 2;

const int TEACHER_MUST_COME_EARLIER = 4;
const int TEACHER_MUST_COME_MUCH_EARLIER = 6;

const int TEACHER_MUST_STAY_LONGER = 3;
const int TEACHER_MUST_STAY_MUCH_LONGER = 5;		// BE CAREFUL, I just print into LESS_DETAILED timetable, if it's smaller than TEACHER_MUST_STAY_MUCH_LONGER

const int TEACHER_HAS_A_FREE_DAY = 7;

const int TEACHER_IS_NOT_AVAILABLE = 8;

const int TEACHERS_FREE_PERIODS_N_CATEGORIES = 9;

///////tricks to save work to reconvert old code
const int CUSTOM_DOUBLE_PRECISION=6; //number of digits after the decimal dot for the weights

void weight_sscanf(const QString& str, const char* fmt, double* result);

class CustomFETString{
public:
	static QString number(int n);
	static QString number(double x);
};

double customFETStrToDouble(const QString& str, bool* ok=0);
////////////////////////////////////////////////

//Trick found on http://www.qtcentre.org/threads/53066-QMap-sorting-according-to-QLocale
//to create a QMap<QString, ...> ordered by locale order.
//Third post, by ChrisW67.
class LocaleString: public QString{
public:
	LocaleString(const QString& s): QString(s){}
};

inline bool operator<(const LocaleString& lhs, const LocaleString& rhs)
{
	return QString::localeAwareCompare(lhs, rhs)<0;
}
/////////////////////////////////////////////////////

#endif
