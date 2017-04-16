/***************************************************************************
                          studentsstatisticsform.h  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#ifndef STUDENTSSTATISTICSFORM_H
#define STUDENTSSTATISTICSFORM_H

#include "ui_studentsstatisticsform_template.h"

#include <QDialog>

#include <QString>
#include <QHash>

#include "studentsset.h"

class StudentsStatisticsForm : public QDialog, Ui::StudentsStatisticsForm_template  {
	Q_OBJECT
	
private:
	QHash<QString, int> allHours;
	QHash<QString, int> allActivities;

public:
	StudentsStatisticsForm(QWidget* parent);
	~StudentsStatisticsForm();
	
	void insertStudentsSet(StudentsSet* set, int row);
	
public slots:
	void checkBoxesModified();

	void on_helpPushButton_clicked();
};

#endif
