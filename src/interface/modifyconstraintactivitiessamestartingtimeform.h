/***************************************************************************
                          modifyconstraintactivitiessamestartingtimeform.h  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#ifndef MODIFYCONSTRAINTACTIVITIESSAMESTARTINGTIMEFORM_H
#define MODIFYCONSTRAINTACTIVITIESSAMESTARTINGTIMEFORM_H

#include "ui_modifyconstraintactivitiessamestartingtimeform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesSameStartingTimeForm : public QDialog, Ui::ModifyConstraintActivitiesSameStartingTimeForm_template  {
	Q_OBJECT
public:
	ModifyConstraintActivitiesSameStartingTimeForm(QWidget* parent, ConstraintActivitiesSameStartingTime* ctr);
	~ModifyConstraintActivitiesSameStartingTimeForm();

	bool filterOk(Activity* ac);
	
public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();

	void clear();

	void ok();
	void cancel();
	
	void filterChanged();
	
private:
	ConstraintActivitiesSameStartingTime* _ctr;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
