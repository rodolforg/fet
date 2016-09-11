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

#ifndef SUBJECT_H
#define SUBJECT_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QList>
#include <QString>

class Subject;
class Rules;

typedef QList<Subject*> SubjectsList;

/**
This class represents a subject

@author Liviu Lalescu
*/
class Subject{
	Q_DECLARE_TR_FUNCTIONS(Subject)
	
public:
	QString name;
	
	QString comments;

	Subject();
	~Subject();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int subjectsAscending(const Subject* s1, const Subject* s2);

#endif
