/***************************************************************************
                          addconstraintteachermaxbuildingchangesperweekform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERWEEKFORM_H

#include "ui_addconstraintteachermaxbuildingchangesperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxBuildingChangesPerWeekForm : public QDialog, Ui::AddConstraintTeacherMaxBuildingChangesPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxBuildingChangesPerWeekForm(QWidget* parent);
	~AddConstraintTeacherMaxBuildingChangesPerWeekForm();

	void updateTeachersComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
