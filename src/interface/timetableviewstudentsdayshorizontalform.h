/***************************************************************************
                          timetableviewstudentsdayshorizontalform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef TIMETABLEVIEWSTUDENTSDAYSHORIZONTALFORM_H
#define TIMETABLEVIEWSTUDENTSDAYSHORIZONTALFORM_H

#include <QResizeEvent>

class QColor; //by Marco Vassura

#include "ui_timetableviewstudentsdayshorizontalform_template.h"

class TimetableViewStudentsDaysHorizontalForm : public QDialog, public Ui::TimetableViewStudentsDaysHorizontalForm_template
{
	Q_OBJECT

public:
	TimetableViewStudentsDaysHorizontalForm(QWidget* parent);
	~TimetableViewStudentsDaysHorizontalForm();

	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateStudentsTimetableTable();

	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	void subgroupChanged(const QString& subgroupName);

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();

protected:
	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(QString s); //by Marco Vassura
};

#endif
