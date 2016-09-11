//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITYTAG_H
#define ACTIVITYTAG_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>

class ActivityTag;
class Rules;

typedef QList<ActivityTag*> ActivityTagsList;

/**
This class represents an activity tag

@author Liviu Lalescu
*/
class ActivityTag{
	Q_DECLARE_TR_FUNCTIONS(ActivityTag)

public:
	QString name;
	
	QString comments;

	ActivityTag();
	~ActivityTag();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int activityTagsAscending(const ActivityTag* st1, const ActivityTag* st2);

#endif
