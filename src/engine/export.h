/*
File export.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
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

#ifndef FET_COMMAND_LINE
#include <QDialog>
#include <QWidget>
#endif

class Export: public QObject{
	Q_OBJECT

public:
#ifndef FET_COMMAND_LINE
	Export(const Timetable &gt, const Solution &solution);
#else
	Export(const Timetable &gt);
#endif
	~Export();

#ifndef FET_COMMAND_LINE
	void exportCSV(QWidget* parent);
#else
	void exportCSV(Solution* bestOrHighest, Solution* current=NULL);
#endif

	QString getTextQuote() const;
	void setTextQuote(const QString &value);

	QString getFieldSeparator() const;
	void setFieldSeparator(const QString &value);

	bool getHeader() const;
	void setHeader(bool value);

	QString getSetSeparator() const;

	QString getDirectoryCSV() const;
	void setDirectoryCSV(const QString &value);

	enum OverwriteOptions {
		OVERWRITE_ALL,
		OVERWRITE_NONE,
		OVERWRITE_PROMPT
	};

	OverwriteOptions getOverwrite() const;
	void setOverwrite(const OverwriteOptions& value);

private:
	const Timetable &gt;
#ifndef FET_COMMAND_LINE
	const Solution &solution;
#endif

	QString directoryCSV;

	QString textquote;
	QString fieldSeparator;
	bool header;
	QString setSeparator;
	OverwriteOptions overwrite;

	QString lastWarnings;

#ifndef FET_COMMAND_LINE
	bool okToWrite(QWidget* parent, const QString& file);
#else
	bool okToWrite(const QString& file);
#endif
	static bool checkSetSeparator(const QString& str, const QString &setSeparator);
	static QString protectCSV(const QString& str);

	bool isActivityNotManualyEdited(int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents, bool& diffCompNStud, bool& diffNStud, bool& diffActive);

#ifndef FET_COMMAND_LINE
	static bool selectSeparatorAndTextquote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head);

	bool exportCSVActivities(QWidget* parent);
	bool exportCSVActivitiesStatistic(QWidget* parent);
	bool exportCSVActivityTags(QWidget* parent);
	bool exportCSVRoomsAndBuildings(QWidget* parent);
	bool exportCSVSubjects(QWidget* parent);
	bool exportCSVTeachers(QWidget* parent);
	bool exportCSVStudents(QWidget* parent);
	bool exportCSVTimetable(QWidget* parent);
#else
	bool exportCSVActivities();
	bool exportCSVActivitiesStatistic();
	bool exportCSVActivityTags();
	bool exportCSVRoomsAndBuildings();
	bool exportCSVSubjects();
	bool exportCSVTeachers();
	bool exportCSVStudents();
	bool exportCSVTimetable(const Solution& solution);
#endif
	QString getFilePath(QString suffix);
};

#ifndef FET_COMMAND_LINE
class LastWarningsDialogE: public QDialog{
	Q_OBJECT

public:				//can I do that private too?
	LastWarningsDialogE(QWidget *parent, const QString& lastWarning);
	~LastWarningsDialogE();
};
#endif

#endif
