/***************************************************************************
                          addconstraintactivitypreferredroomsform.h  -  description
                             -------------------
    begin                : March 28, 2005
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

#ifndef ADDCONSTRAINTACTIVITYPREFERREDROOMSFORM_H
#define ADDCONSTRAINTACTIVITYPREFERREDROOMSFORM_H

#include "ui_addconstraintactivitypreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintActivityPreferredRoomsForm : public QDialog, Ui::AddConstraintActivityPreferredRoomsForm_template  {
	Q_OBJECT
public:
	AddConstraintActivityPreferredRoomsForm(QWidget* parent);
	~AddConstraintActivityPreferredRoomsForm();

	void updateRoomsListWidget();
	void updateActivitiesComboBox();
	
	bool filterOk(Activity* a);

public slots:
	void addRoom();
	void removeRoom();

	void addConstraint();

	void filterChanged();
	
	void clear();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
};

#endif
