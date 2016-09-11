/***************************************************************************
                          constraintactivitiessamestartingtime.h  -  description
                             -------------------
    begin                : 23 June 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITIESSAMESTARTINGTIMEFORM_H
#define CONSTRAINTACTIVITIESSAMESTARTINGTIMEFORM_H

#include "ui_constraintactivitiessamestartingtimeform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesSameStartingTimeForm : public QDialog, Ui::ConstraintActivitiesSameStartingTimeForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesSameStartingTimeForm(QWidget* parent);
	~ConstraintActivitiesSameStartingTimeForm();

	void refreshConstraintsListWidget();

	bool filterOk(TimeConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void removeConstraint();
	void modifyConstraint();
	
	void filterChanged();
	
	void help();
};

#endif
