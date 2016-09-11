/*
File export.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          export.h  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef EXPORT_H
#define EXPORT_H

#include "timetable.h"
#include "timetable_defs.h"

#include <QDialog>
#include <QWidget>

#include <QMessageBox>

class Export: public QObject{
	Q_OBJECT

public:
	Export();
	~Export();

	static void exportCSV(QWidget* parent);
private:
	static bool okToWrite(QWidget* parent, const QString& file, QMessageBox::StandardButton& msgBoxButton);

	static bool checkSetSeparator(const QString& str, const QString setSeparator);
	static QString protectCSV(const QString& str);

	static bool isActivityNotManualyEdited(const int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents, bool& diffCompNStud, bool& diffNStud, bool& diffActive);

	static bool selectSeparatorAndTextquote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head);

	static bool exportCSVActivities(QWidget* parent, QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVActivitiesStatistic(QWidget* parent, QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVActivityTags(QWidget* parent, QString& lastWarnings, const QString textquote, const bool head, const QString setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVRoomsAndBuildings(QWidget* parent, QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVSubjects(QWidget* parent, QString& lastWarnings, const QString textquote, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVTeachers(QWidget* parent, QString& lastWarnings, const QString textquote, const bool head, const QString setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVStudents(QWidget* parent, QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, const QString setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVTimetable(QWidget* parent, QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
};

class LastWarningsDialogE: public QDialog{
	Q_OBJECT
	
public:				//can i do that privat too?
	LastWarningsDialogE(QWidget *parent, QString lastWarning);
	~LastWarningsDialogE();
};

#endif
