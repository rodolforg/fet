/***************************************************************************
                          addconstraintteachersminrestinghoursform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERSMINRESTINGHOURSFORM_H
#define ADDCONSTRAINTTEACHERSMINRESTINGHOURSFORM_H

#include "ui_addconstraintteachersminrestinghoursform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMinRestingHoursForm : public QDialog, Ui::AddConstraintTeachersMinRestingHoursForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMinRestingHoursForm(QWidget* parent);
	~AddConstraintTeachersMinRestingHoursForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
