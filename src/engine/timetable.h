/*
File timetable.h
*/

/***************************************************************************
                          timetable.h  -  description
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

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include "timetable_defs.h"
#include "rules.h"

/**
This class represents the main interface for the library.
*/
class Timetable{
public:
	/**
	The set of rules for the timetable (the main input).
	*/
	Rules rules;
};

#endif
