/***************************************************************************
                          addconstraintactivitypreferredroomform.h  -  description
                             -------------------
    begin                : February 7, 2005
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

#ifndef ADDCONSTRAINTACTIVITYPREFERREDROOMFORM_H
#define ADDCONSTRAINTACTIVITYPREFERREDROOMFORM_H

#include "ui_addconstraintactivitypreferredroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintActivityPreferredRoomForm : public QDialog, Ui::AddConstraintActivityPreferredRoomForm_template  {
	Q_OBJECT
public:
	AddConstraintActivityPreferredRoomForm(QWidget* parent);
	~AddConstraintActivityPreferredRoomForm();

	void updateActivitiesComboBox();
	void updateRoomsComboBox();

	bool filterOk(Activity* a);

public slots:
	void addConstraint();

	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
};

#endif
