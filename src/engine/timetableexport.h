/*
File timetableexport.h
*/

/***************************************************************************
                          timetableexport.h  -  description
                             -------------------
    begin                : Tue Apr 22, 2003
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

#ifndef TIMETABLEEXPORT_H
#define TIMETABLEEXPORT_H

#include "timetable_defs.h"
#include "timetable.h"

#include <QString>
#include <QHash>
#include <QList>

class QWidget;

bool writeAtLeastATimetable();

extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;
extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;

class TimetableExport: public QObject{
	Q_OBJECT

public:
	TimetableExport();
	~TimetableExport();
	
	static void stringToColor(QString s, int *r, int *g, int *b);

	static void getStudentsTimetable(Solution& c);
	static void getTeachersTimetable(Solution& c);
	static void getRoomsTimetable(Solution& c);
	static void getNumberOfPlacedActivities(int& number1, int& number2);

	static void writeSimulationResults(QWidget* parent);
	static void writeHighestStageResults(QWidget* parent);
	static void writeSimulationResults(QWidget* parent, int n); //write in a directory with number n (for multiple generation)
	static void writeSimulationResultsCommandLine(QWidget* parent, const QString& outputDirectory);
	
	static void writeRandomSeed(QWidget* parent, bool before);
	static void writeRandomSeed(QWidget* parent, int n, bool before); //write in a directory with number n (for multiple generation)
	static void writeRandomSeedCommandLine(QWidget* parent, const QString& outputDirectory, bool before);
	static void writeRandomSeedFile(QWidget* parent, const QString& filename, bool before);
	
	static void writeTimetableDataFile(QWidget* parent, const QString& filename);
	
	static void writeReportForMultiple(QWidget* parent, const QString& description, bool begin);
	
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
	static void writeSubgroupsTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeTeachersTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeActivitiesTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeConflictsTxt(QWidget* parent, const QString& filename, const QString& saveTime, int placedActivities);

	//the following functions write the css and html timetable files
	static void writeIndexHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeStylesheetCss(QWidget* parent, const QString& cssfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeStudentsStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);

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
};

#endif
