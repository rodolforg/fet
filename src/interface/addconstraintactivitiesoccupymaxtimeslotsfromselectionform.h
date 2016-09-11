/***************************************************************************
                          addconstraintactivitiesoccupymaxtimeslotsfromselectionform.h  -  description
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

#ifndef ADDCONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H
#define ADDCONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H

#include "ui_addconstraintactivitiesoccupymaxtimeslotsfromselectionform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm : public QDialog, Ui::AddConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm_template  {
	Q_OBJECT
	
public:
	AddConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(QWidget* parent);
	~AddConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm();

	void colorItem(QTableWidgetItem* item);
	
	void updateActivitiesListWidget();
	
	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();
	
	void filterChanged();
	
	void clear();

	void addCurrentConstraint();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void setAllUnselected();
	void setAllSelected();
	
private:
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
