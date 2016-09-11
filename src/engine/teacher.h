//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TEACHER_H
#define TEACHER_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QLinkedList>
#include <QHash>

class Teacher;
class Rules;

typedef QList<Teacher*> TeachersList;

/**
@author Liviu Lalescu
*/
class Teacher
{
	Q_DECLARE_TR_FUNCTIONS(Teacher)
	
public:
	QList<int> activitiesForTeacher;

	QString name;
	
	QString comments;
	
	int targetNumberOfHours;
	
	QLinkedList<QString> qualifiedSubjectsList;
	QHash<QString, QLinkedList<QString>::Iterator> qualifiedSubjectsHash; //index in the above list, useful when removing/renaming subjects

	Teacher();
	~Teacher();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int teachersAscending(const Teacher* t1, const Teacher* t2);

#endif
