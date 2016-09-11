/***************************************************************************
                          constraintmindaysbetweenactivitiesform.h  -  description
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

#ifndef CONSTRAINTMINDAYSBETWEENACTIVITIESFORM_H
#define CONSTRAINTMINDAYSBETWEENACTIVITIESFORM_H

#include "ui_constraintmindaysbetweenactivitiesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintMinDaysBetweenActivitiesForm : public QDialog, Ui::ConstraintMinDaysBetweenActivitiesForm_template  {
	Q_OBJECT

public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintMinDaysBetweenActivitiesForm(QWidget* parent);
	~ConstraintMinDaysBetweenActivitiesForm();

	bool filterOk(TimeConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();

	//void changeAllWeights();
	void changeSelectively();
};

#endif
