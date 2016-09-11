/*
File fet.h
*/
  
/***************************************************************************
                          fet.h  -  description
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

#ifndef FET_H
#define FET_H

#include "timetable_defs.h"
#include "timetable.h"

#include "matrix.h"

#include <QTextStream>

#ifndef FET_COMMAND_LINE
#include <QApplication>
#endif

class QWidget;

class FetTranslate: public QObject{
	Q_OBJECT
};

/**
The one and only instantiation of the main class.
*/
extern Timetable gt;

/**
The name of the file from where the rules are read.
*/
extern QString INPUT_FILENAME_XML;

/**
The timetable for the teachers
*/
//extern qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix3D<int> teachers_timetable_weekly;

/**
The timetable for the students
*/
//extern qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix3D<int> students_timetable_weekly;

/**
The timetable for the rooms
*/
//extern qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern Matrix3D<int> rooms_timetable_weekly;

void readSimulationParameters();
void writeSimulationParameters();
//void writeDefaultSimulationParameters();

#ifndef FET_COMMAND_LINE
void setLanguage(QApplication& qapplication, QWidget* parent);
#else
void setLanguage(QCoreApplication& qapplication, QWidget* parent);
#endif

void initLanguagesSet();

/**
The main function.
*/
int main(int argc, char **argv);

#endif
