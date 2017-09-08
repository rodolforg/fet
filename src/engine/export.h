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

#include <QDialog>
#include <QWidget>

#include <QMessageBox>

class Export: public QObject{
	Q_OBJECT

public:
	Export(const Timetable &gt, const Solution &solution);
	~Export();

	void exportCSV(QWidget* parent);

	QString getTextquote() const;
	void setTextquote(const QString &value);

	QString getFieldSeparator() const;
	void setFieldSeparator(const QString &value);

	bool getHeader() const;
	void setHeader(bool value);

	QString getSetSeparator() const;

	QString getDirectoryCSV() const;
	void setDirectoryCSV(const QString &value);

private:
	const Timetable &gt;
	const Solution &solution;

	QString directoryCSV;

	QString textquote;
	QString fieldSeparator;
	bool header;
	QString setSeparator;

	QString lastWarnings;

	static bool okToWrite(QWidget* parent, const QString& file, QMessageBox::StandardButton& msgBoxButton);

	static bool checkSetSeparator(const QString& str, const QString &setSeparator);
	static QString protectCSV(const QString& str);

	bool isActivityNotManualyEdited(const int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents, bool& diffCompNStud, bool& diffNStud, bool& diffActive);

	static bool selectSeparatorAndTextquote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head);

	bool exportCSVActivities(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVActivitiesStatistic(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVActivityTags(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVRoomsAndBuildings(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVSubjects(QWidget* parent,QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVTeachers(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVStudents(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	bool exportCSVTimetable(QWidget* parent, QMessageBox::StandardButton& msgBoxButton);
	QString getFilePath(QString suffix);
};

class LastWarningsDialogE: public QDialog{
	Q_OBJECT
	
public:				//can I do that private too?
	LastWarningsDialogE(QWidget *parent, QString lastWarning);
	~LastWarningsDialogE();
};

#endif
