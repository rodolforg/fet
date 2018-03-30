/*
File fet.h
*/
  
/***************************************************************************
                          fet.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Lalescu Liviu
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

#ifndef FET_H
#define FET_H

#include "timetable.h"

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

void setLanguage(QWidget* parent);

#endif
