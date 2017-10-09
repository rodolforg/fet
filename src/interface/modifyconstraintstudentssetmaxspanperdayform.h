/***************************************************************************
                          modifyconstraintstudentssetmaxspanperdayform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXSPANPERDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXSPANPERDAYFORM_H

#include "ui_modifyconstraintstudentssetmaxspanperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxSpanPerDayForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxSpanPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxSpanPerDay* _ctr;

	ModifyConstraintStudentsSetMaxSpanPerDayForm(QWidget* parent, ConstraintStudentsSetMaxSpanPerDay* ctr);
	~ModifyConstraintStudentsSetMaxSpanPerDayForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
