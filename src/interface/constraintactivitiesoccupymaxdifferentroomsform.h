/***************************************************************************
                          constraintactivitiesoccupymaxdifferentroomsform.h  -  description
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

#ifndef CONSTRAINTACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H
#define CONSTRAINTACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H

#include "ui_constraintactivitiesoccupymaxdifferentroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesOccupyMaxDifferentRoomsForm : public QDialog, Ui::ConstraintActivitiesOccupyMaxDifferentRoomsForm_template  {
	Q_OBJECT
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent);
	~ConstraintActivitiesOccupyMaxDifferentRoomsForm();

	bool filterOk(SpaceConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
	
	void help();
};

#endif
