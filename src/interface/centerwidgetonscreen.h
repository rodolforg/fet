/*
File centerwidgetonscreen.h
*/

/***************************************************************************
                          centerwidgetonscreen.h  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CENTERWIDGETONSCREEN_H
#define CENTERWIDGETONSCREEN_H

#include <QString>

class QWidget;
class QTableWidget;

void centerWidgetOnScreen(QWidget* widget);
void forceCenterWidgetOnScreen(QWidget* widget);
//void centerWidgetOnParent(QWidget* widget, QWidget* parent);

int maxRecommendedWidth(QWidget* widget);

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());
void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());

void setStretchAvailabilityTableNicely(QTableWidget* notAllowedTimesTable);

#endif
