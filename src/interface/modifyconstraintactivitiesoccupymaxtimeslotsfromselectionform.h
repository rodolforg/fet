/***************************************************************************
                          modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform.h  -  description
                             -------------------
    begin                : Sept 26, 2011
    copyright            : (C) 2011 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H
#define MODIFYCONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H

#include "ui_modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm : public QDialog, Ui::ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm_template  {
	Q_OBJECT
	
public:
	ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(QWidget* parent, ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* ctr);
	~ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm();

	void colorItem(QTableWidgetItem* item);
	
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

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void setAllUnselected();
	void setAllSelected();
	
private:
	ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* _ctr;

	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
