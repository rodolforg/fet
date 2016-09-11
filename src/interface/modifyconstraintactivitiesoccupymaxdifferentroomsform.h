/***************************************************************************
                          modifyconstraintactivitiesoccupymaxdifferentroomsform.h  -  description
                             -------------------
    begin                : Apr 29, 2012
    copyright            : (C) 2012 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H
#define MODIFYCONSTRAINTACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H

#include "ui_modifyconstraintactivitiesoccupymaxdifferentroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm : public QDialog, Ui::ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm_template  {
	Q_OBJECT
	
public:
	ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent, ConstraintActivitiesOccupyMaxDifferentRooms* ctr);
	~ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm();

	void updateActivitiesListWidget();
	
	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();
	
	void filterChanged();
	
	void clear();

	void ok();
	void cancel();

private:
	ConstraintActivitiesOccupyMaxDifferentRooms* _ctr;

	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
