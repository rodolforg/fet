/***************************************************************************
                          modifyconstraintactivitiessameroomifconsecutiveform.h  -  description
                             -------------------
    begin                : Sept 14, 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H
#define MODIFYCONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H

#include "ui_modifyconstraintactivitiessameroomifconsecutiveform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesSameRoomIfConsecutiveForm : public QDialog, Ui::ModifyConstraintActivitiesSameRoomIfConsecutiveForm_template  {
	Q_OBJECT
	
public:
	ModifyConstraintActivitiesSameRoomIfConsecutiveForm(QWidget* parent, ConstraintActivitiesSameRoomIfConsecutive* ctr);
	~ModifyConstraintActivitiesSameRoomIfConsecutiveForm();

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
	ConstraintActivitiesSameRoomIfConsecutive* _ctr;

	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
