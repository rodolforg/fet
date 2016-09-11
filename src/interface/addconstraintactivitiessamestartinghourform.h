/***************************************************************************
                          addconstraintactivitiessamestartinghourform.h  -  description
                             -------------------
    begin                : Feb 15, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITIESSAMESTARTINGHOURFORM_H
#define ADDCONSTRAINTACTIVITIESSAMESTARTINGHOURFORM_H

#include "ui_addconstraintactivitiessamestartinghourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivitiesSameStartingHourForm : public QDialog, Ui::AddConstraintActivitiesSameStartingHourForm_template  {
	Q_OBJECT
public:
	AddConstraintActivitiesSameStartingHourForm(QWidget* parent);
	~AddConstraintActivitiesSameStartingHourForm();

	void updateActivitiesListWidget();
	
	bool filterOk(Activity* a);

public slots:
	void clear();

	void filterChanged();

	void addActivity();
	void addAllActivities();
	void removeActivity();

	void addConstraint();
	
private:
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of same starting hour activities
	QList<int> selectedActivitiesList;
};

#endif
