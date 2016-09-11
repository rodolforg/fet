/***************************************************************************
                          constraintsubactivitiespreferredtimeslots.h  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CONSTRAINTSUBACTIVITIESPREFERREDTIMESLOTSFORM_H
#define CONSTRAINTSUBACTIVITIESPREFERREDTIMESLOTSFORM_H

#include "ui_constraintsubactivitiespreferredtimeslotsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintSubactivitiesPreferredTimeSlotsForm : public QDialog, Ui::ConstraintSubactivitiesPreferredTimeSlotsForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintSubactivitiesPreferredTimeSlotsForm(QWidget* parent);
	~ConstraintSubactivitiesPreferredTimeSlotsForm();

	void refreshConstraintsListWidget();

	bool filterOk(TimeConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	
	void help();
};

#endif
