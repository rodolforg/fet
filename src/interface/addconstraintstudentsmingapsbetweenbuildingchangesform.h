/***************************************************************************
                          addconstraintstudentsmingapsbetweenbuildingchangesform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#ifndef ADDCONSTRAINTSTUDENTSMINGAPSBETWEENBUILDINGCHANGESFORM_H
#define ADDCONSTRAINTSTUDENTSMINGAPSBETWEENBUILDINGCHANGESFORM_H

#include "ui_addconstraintstudentsmingapsbetweenbuildingchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMinGapsBetweenBuildingChangesForm : public QDialog, Ui::AddConstraintStudentsMinGapsBetweenBuildingChangesForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMinGapsBetweenBuildingChangesForm(QWidget* parent);
	~AddConstraintStudentsMinGapsBetweenBuildingChangesForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
