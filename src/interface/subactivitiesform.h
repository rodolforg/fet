/***************************************************************************
                          subactivitiesform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef SUBACTIVITIESFORM_H
#define SUBACTIVITIESFORM_H

#include "ui_subactivitiesform_template.h"

#include "activity.h"

#include <QSet>
#include <QString>

class SubactivitiesForm : public QDialog, Ui::SubactivitiesForm_template  {
	Q_OBJECT
	
private:
	QSet<QString> showedStudents;
	
public:
	ActivitiesList visibleSubactivitiesList;

	SubactivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName);
	~SubactivitiesForm();

	bool filterOk(Activity* act);

public slots:
	void modifySubactivity();
	void subactivityChanged();
	void filterChanged();
	
	void studentsFilterChanged();
	
	void help();
	
	void subactivityComments();
};

#endif
