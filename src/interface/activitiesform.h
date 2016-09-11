/***************************************************************************
                          activitiesform.h  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef ACTIVITIESFORM_H
#define ACTIVITIESFORM_H

#include "ui_activitiesform_template.h"

#include "activity.h"

#include <QSet>
#include <QString>

class ActivitiesForm : public QDialog, Ui::ActivitiesForm_template{
	Q_OBJECT
	
private:
	QSet<QString> showedStudents;
	
public:
	ActivitiesList visibleActivitiesList;

	ActivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName);
	~ActivitiesForm();

	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void removeActivity();
	void modifyActivity();
	void activityChanged();
	void filterChanged();
	
	void studentsFilterChanged();
	
	void help();
	
	void activityComments();
};

#endif
