/***************************************************************************
                          modifyconstraintactivitiesnotoverlappingform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITIESNOTOVERLAPPINGFORM_H
#define MODIFYCONSTRAINTACTIVITIESNOTOVERLAPPINGFORM_H

#include "ui_modifyconstraintactivitiesnotoverlappingform_template.h"
#include "timetable_defs.h"
#include "timetable.h"

#include <QList>

class ModifyConstraintActivitiesNotOverlappingForm : public QDialog, Ui::ModifyConstraintActivitiesNotOverlappingForm_template  {
	Q_OBJECT
public:
	ModifyConstraintActivitiesNotOverlappingForm(QWidget* parent, ConstraintActivitiesNotOverlapping* ctr);
	~ModifyConstraintActivitiesNotOverlappingForm();

	bool filterOk(Activity* ac);
	
	
public slots:
	void filterChanged();

	void addActivity();
	void addAllActivities();
	void removeActivity();

	void clear();

	void ok();
	void cancel();
	
private:
	ConstraintActivitiesNotOverlapping* _ctr;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of non-overlapping activities
	QList<int> notOverlappingActivitiesList;
};

#endif
