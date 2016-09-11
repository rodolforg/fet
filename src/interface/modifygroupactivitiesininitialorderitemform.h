/***************************************************************************
                          modifygroupactivitiesininitialorderitemform.h  -  description
                             -------------------
    begin                : 2014
    copyright            : (C) 2014 by Lalescu Liviu
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

#ifndef MODIFYGROUPACTIVITIESININITIALORDERFORM_H
#define MODIFYGROUPACTIVITIESININITIALORDERFORM_H

#include "ui_modifygroupactivitiesininitialorderitemform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyGroupActivitiesInInitialOrderItemForm : public QDialog, Ui::ModifyGroupActivitiesInInitialOrderItemForm_template  {
	Q_OBJECT	

public:
	ModifyGroupActivitiesInInitialOrderItemForm(QWidget* parent, GroupActivitiesInInitialOrderItem* item);
	~ModifyGroupActivitiesInInitialOrderItemForm();

	bool filterOk(Activity* ac);
	
public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();

	void ok();
	void cancel();
	
	void clear();

	void filterChanged();

private:
	GroupActivitiesInInitialOrderItem* _item;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
