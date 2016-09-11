/***************************************************************************
                          constraintactivitiessameroomifconsecutiveform.h  -  description
                             -------------------
    begin                : Sept 14, 2013
    copyright            : (C) 2013 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H
#define CONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H

#include "ui_constraintactivitiessameroomifconsecutiveform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesSameRoomIfConsecutiveForm : public QDialog, Ui::ConstraintActivitiesSameRoomIfConsecutiveForm_template  {
	Q_OBJECT
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintActivitiesSameRoomIfConsecutiveForm(QWidget* parent);
	~ConstraintActivitiesSameRoomIfConsecutiveForm();

	bool filterOk(SpaceConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
	
//	void help();
};

#endif
