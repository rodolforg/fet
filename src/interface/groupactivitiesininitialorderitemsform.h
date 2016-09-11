/***************************************************************************
                          groupactivitiesininitialorderitemsform.h  -  description
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

#ifndef GROUPACTIVITIESININITIALORDERITEMFORM_H
#define GROUPACTIVITIESININITIALORDERITEMFORM_H

#include "ui_groupactivitiesininitialorderitemsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class GroupActivitiesInInitialOrderItemsForm : public QDialog, Ui::GroupActivitiesInInitialOrderItemsForm_template  {
	Q_OBJECT

public:
	QList<GroupActivitiesInInitialOrderItem*> visibleItemsList;

	GroupActivitiesInInitialOrderItemsForm(QWidget* parent);
	~GroupActivitiesInInitialOrderItemsForm();

	bool filterOk(const GroupActivitiesInInitialOrderItem& item);
	
public slots:
	void itemChanged(int index);
	void addItem();
	void modifyItem();
	void removeItem();
	
	void help();

	void filterChanged();
	
	void sortItemsByComments();
	void activateItem();
	void deactivateItem();
	void itemComments();
};

#endif
