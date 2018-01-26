/*
File timetableexport.h
*/

/***************************************************************************
                          timetableexport.h  -  description
                             -------------------
    begin                : Tue Apr 22, 2003
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

#ifndef TIMETABLEEXPORT_H
#define TIMETABLEEXPORT_H

#include "timetable.h"
#include "solution.h"
#include "errorcode.h"

#include <QString>
#include <QHash>
#include <QList>

extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;
extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;
class CachedSchedule {
public:
	static void invalidate();
	static bool isValid();
	static void update(const Solution &c);

	/**
	The timetable for the teachers
	*/
	static Matrix3D<int> teachers_timetable_weekly;

	/**
	The timetable for the students
	*/
	static Matrix3D<int> students_timetable_weekly;

	/**
	The timetable for the rooms
	*/
	static Matrix3D<int> rooms_timetable_weekly;

	static Matrix3D<QList<int> > teachers_free_periods_timetable_weekly;

	static const Solution& getCachedSolution();

private:
	static Solution cachedSolution;
	static bool cached_schedule_ready;
};

/**
Timetable html css javaScript Level, added by Volker Dirr
*/
extern int TIMETABLE_HTML_LEVEL;

extern bool TIMETABLE_HTML_PRINT_ACTIVITY_TAGS;

extern bool PRINT_DETAILED_HTML_TIMETABLES;

extern bool PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS;

extern bool PRINT_NOT_AVAILABLE_TIME_SLOTS;

extern bool PRINT_BREAK_TIME_SLOTS;

extern bool DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS;

extern bool TIMETABLE_HTML_REPEAT_NAMES;

extern bool PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME;

extern bool WRITE_TIMETABLE_CONFLICTS;

extern bool WRITE_TIMETABLES_STATISTICS;
extern bool WRITE_TIMETABLES_XML;
extern bool WRITE_TIMETABLES_DAYS_HORIZONTAL;
extern bool WRITE_TIMETABLES_DAYS_VERTICAL;
extern bool WRITE_TIMETABLES_TIME_HORIZONTAL;
extern bool WRITE_TIMETABLES_TIME_VERTICAL;

extern bool WRITE_TIMETABLES_SUBGROUPS;
extern bool WRITE_TIMETABLES_GROUPS;
extern bool WRITE_TIMETABLES_YEARS;
extern bool WRITE_TIMETABLES_TEACHERS;
extern bool WRITE_TIMETABLES_TEACHERS_FREE_PERIODS;
extern bool WRITE_TIMETABLES_ROOMS;
extern bool WRITE_TIMETABLES_SUBJECTS;
extern bool WRITE_TIMETABLES_ACTIVITY_TAGS;
extern bool WRITE_TIMETABLES_ACTIVITIES;

class TimetableExport: public QObject{
	Q_OBJECT

public:
	TimetableExport();
	~TimetableExport();
	
	static void stringToColor(QString s, int *r, int *g, int *b);

	static void getNumberOfPlacedActivities(int& number1, int& number2);

	static ErrorList writeSimulationResults();
	static ErrorList writeHighestStageResults();
	static ErrorList writeSimulationResults(int n); //write in a directory with number n (for multiple generation)
	static ErrorList writeSimulationResultsCommandLine(const QString& outputDirectory);
	
	static ErrorCode writeRandomSeed(bool before);
	static ErrorCode writeRandomSeed(int n, bool before); //write in a directory with number n (for multiple generation)
	static ErrorCode writeRandomSeedCommandLine(const QString& outputDirectory, bool before);
	static ErrorCode writeRandomSeedFile(const QString& filename, bool before);
	
	static ErrorCode writeTimetableDataFile(const QString& filename);
	
	static ErrorCode writeReportForMultiple(const QString& description, bool begin);
	
	//the following functions return a single html table (needed for html file export and printing)
	static QString singleSubgroupsTimetableDaysHorizontalHtml(int htmlLevel, int subgroup, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubgroupsTimetableDaysVerticalHtml(int htmlLevel, int subgroup, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubgroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubgroupsTimetableTimeVerticalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubgroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubgroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleGroupsTimetableDaysHorizontalHtml(int htmlLevel, int group, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleGroupsTimetableDaysVerticalHtml(int htmlLevel, int group, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeVerticalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxGroups, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableDaysHorizontalHtml(int htmlLevel, int year, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableDaysVerticalHtml(int htmlLevel, int year, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeHorizontalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeVerticalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxYears, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool detailed, bool repeatNames);
	static QString singleAllActivitiesTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleAllActivitiesTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleAllActivitiesTimetableTimeHorizontalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleAllActivitiesTimetableTimeVerticalHtml(int htmlLevel, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleAllActivitiesTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleAllActivitiesTimetableTimeVerticalDailyHtml(int htmlLevel, int day, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableDaysHorizontalHtml(int htmlLevel, int teacher, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableDaysVerticalHtml(int htmlLevel, int teacher, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableTimeHorizontalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableTimeVerticalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableDaysHorizontalHtml(int htmlLevel, int room, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableDaysVerticalHtml(int htmlLevel, int room, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableTimeHorizontalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableTimeVerticalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleRoomsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxRooms, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableDaysHorizontalHtml(int htmlLevel, int subject, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableDaysVerticalHtml(int htmlLevel, int subject, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableTimeVerticalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleSubjectsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableDaysHorizontalHtml(int htmlLevel, int activityTag, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableDaysVerticalHtml(int htmlLevel, int activityTag, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableTimeHorizontalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableTimeVerticalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableTimeHorizontalDailyHtml(int htmlLevel, int day, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleActivityTagsTimetableTimeVerticalDailyHtml(int htmlLevel, int day, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime, bool printActivityTags, bool repeatNames);
	static QString singleTeachersFreePeriodsTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames);
	static QString singleTeachersFreePeriodsTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames);
	static QString singleTeachersStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll);
	static QString singleStudentsStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll);
	
	static void computeHashActivityColorBySubject();
	static void computeHashActivityColorBySubjectAndStudents();

private:
	//this function must be called before export html files, because it compute the IDs
	static void computeHashForIDsTimetable();

	//this function must be called before export html files, because it is needed for the allActivities tables
	static void computeActivitiesAtTime();
	
	//this function must be called before export html files, because it is needed to add activities with same starting time (simultanious activities)
	static void computeActivitiesWithSameStartingTime();
	//this function add activities with same starting time into the allActivities list
	static bool addActivitiesWithSameStartingTime(QList<int>& allActivities, int hour);

	//the following functions write the conflicts text and the xml files
	static ErrorCode writeSubgroupsTimetableXml(const QString& xmlfilename);
	static ErrorCode writeTeachersTimetableXml(const QString& xmlfilename);
	static ErrorCode writeActivitiesTimetableXml(const QString& xmlfilename);
	static ErrorCode writeConflictsTxt(const QString& filename, const QString& saveTime, int placedActivities);

	//the following functions write the css and html timetable files
	static ErrorCode writeIndexHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeStylesheetCss(const QString& cssfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubgroupsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeGroupsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeYearsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeAllActivitiesTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeRoomsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeSubjectsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableTimeHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableTimeVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableTimeHorizontalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeActivityTagsTimetableTimeVerticalDailyHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersFreePeriodsTimetableDaysHorizontalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersFreePeriodsTimetableDaysVerticalHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeTeachersStatisticsHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static ErrorCode writeStudentsStatisticsHtml(const QString& htmlfilename, const QString& saveTime, int placedActivities);

	//the following functions return QStrings, because they are 'only' subfunctions to the writeXxxHtml functions
	static QString writeActivityStudents(int htmlLevel, int ai, int day, int hour, bool notAvailable, bool colspan, bool rowspan, bool printActivityTags, QString skipStudentsSet);
	static QString writeActivitiesStudents(int htmlLevel, const QList<int>& allActivities, bool printActivityTags);
	static QString writeActivityTeacher(int htmlLevel, int teacher, int day, int hour, bool colspan, bool rowspan, bool printActivityTags, QString skipTeacher);
	static QString writeActivitiesTeachers(int htmlLevel, const QList<int>& allActivities, bool printActivityTags);
	static QString writeActivityRoom(int htmlLevel, int room, int day, int hour, bool colspan, bool rowspan, bool printActivityTags/*, QString skipRoom*/); //currently unneeded, because FET supports only one room per activity
	static QString writeActivitiesRooms(int htmlLevel, const QList<int>& allActivities, bool printActivityTags);
	static QString writeActivitiesSubjects(int htmlLevel, const QList<int>& allActivities, bool printActivityTags);
	static QString writeActivitiesActivityTags(int htmlLevel, const QList<int>& allActivities, bool printActivityTags);

	//the following functions return QStrings, because they are 'only' subfunctions to the writeActivity-iesXxx functions
	static QString writeHead(bool java, int placedActivities, bool printInstitution);
	static QString writeTOCDays();
	static QString writeStartTagTDofActivities(int htmlLevel, const Activity* act, bool detailed, bool colspan, bool rowspan, int colorBy);
	static QString writeSubjectAndActivityTags(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute, bool activityTagsOnly, bool printActivityTags);
	static QString writeStudents(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute);
	static QString writeTeachers(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute);
	static QString writeRoom(int htmlLevel, int ai, const QString& startTag, const QString& startTagAttribute);
	static QString writeNotAvailableSlot(int htmlLevel, const QString& weight);
	static QString writeBreakSlot(int htmlLevel, const QString& weight);
	static QString writeEmpty(int htmlLevel);
	static ErrorList doWriteResults(QString filenamePrefix);
};

#endif
