/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          advancedlockunlockform.h  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (http://lalescu.ro/liviu/) and Volker Dirr (http://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADVANCEDLOCKUNLOCKFORM_H
#define ADVANCEDLOCKUNLOCKFORM_H

#include <QObject>

class AdvancedLockUnlockForm: public QObject{
	Q_OBJECT

public:
	static void lockAll(QWidget* parent);
	static void unlockAll(QWidget* parent);

	static void unlockAllWithoutTimetable(QWidget* parent);
	
	static void lockDay(QWidget* parent);
	static void unlockDay(QWidget* parent);

	static void unlockDayWithoutTimetable(QWidget* parent);
	
	static void lockEndStudentsDay(QWidget* parent);
	static void unlockEndStudentsDay(QWidget* parent);
};

#endif
