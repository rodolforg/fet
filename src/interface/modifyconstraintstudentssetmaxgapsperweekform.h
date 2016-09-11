/***************************************************************************
                          modifyconstraintstudentssetmaxgapsperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXGAPSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXGAPSPERWEEKFORM_H

#include "ui_modifyconstraintstudentssetmaxgapsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxGapsPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxGapsPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxGapsPerWeek* _ctr;

	ModifyConstraintStudentsSetMaxGapsPerWeekForm(QWidget* parent, ConstraintStudentsSetMaxGapsPerWeek* ctr);
	~ModifyConstraintStudentsSetMaxGapsPerWeekForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
