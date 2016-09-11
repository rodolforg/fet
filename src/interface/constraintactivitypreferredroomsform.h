/***************************************************************************
                          constraintactivitypreferredroomsform.h  -  description
                             -------------------
    begin                : 28 March 2005
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

#ifndef CONSTRAINTACTIVITYPREFERREDROOMSFORM_H
#define CONSTRAINTACTIVITYPREFERREDROOMSFORM_H

#include "ui_constraintactivitypreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivityPreferredRoomsForm : public QDialog, Ui::ConstraintActivityPreferredRoomsForm_template  {
	Q_OBJECT

public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintActivityPreferredRoomsForm(QWidget* parent);
	~ConstraintActivityPreferredRoomsForm();

	bool filterOk(SpaceConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void removeConstraint();
	void modifyConstraint();
	
	void filterChanged();
};

#endif
