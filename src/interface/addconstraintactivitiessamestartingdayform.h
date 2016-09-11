/***************************************************************************
                          addconstraintactivitiessamestartingdayform.h  -  description
                             -------------------
    begin                : June 23, 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITIESSAMESTARTINGDAYFORM_H
#define ADDCONSTRAINTACTIVITIESSAMESTARTINGDAYFORM_H

#include "ui_addconstraintactivitiessamestartingdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivitiesSameStartingDayForm : public QDialog, Ui::AddConstraintActivitiesSameStartingDayForm_template  {
	Q_OBJECT

public:
	AddConstraintActivitiesSameStartingDayForm(QWidget* parent);
	~AddConstraintActivitiesSameStartingDayForm();

	void updateActivitiesListWidget();
	
	bool filterOk(Activity* a);

public slots:
	void filterChanged();

	void addActivity();
	void addAllActivities();
	void removeActivity();
	void clear();

	void addConstraint();
	
private:
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of simultaneous activities
	QList<int> simultaneousActivitiesList;
};

#endif
