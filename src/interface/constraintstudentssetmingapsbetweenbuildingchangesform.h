/***************************************************************************
                          constraintstudentssetmingapsbetweenbuildingchangesform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTSTUDENTSSETMINGAPSBETWEENBUILDINGCHANGESFORM_H
#define CONSTRAINTSTUDENTSSETMINGAPSBETWEENBUILDINGCHANGESFORM_H

#include "ui_constraintstudentssetmingapsbetweenbuildingchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintStudentsSetMinGapsBetweenBuildingChangesForm : public QDialog, Ui::ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template  {
	Q_OBJECT
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintStudentsSetMinGapsBetweenBuildingChangesForm(QWidget* parent);
	~ConstraintStudentsSetMinGapsBetweenBuildingChangesForm();

	bool filterOk(SpaceConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
