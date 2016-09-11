/***************************************************************************
                          modifyconstraintteachernotavailabletimesform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERNOTAVAILABLETIMESFORM_H
#define MODIFYCONSTRAINTTEACHERNOTAVAILABLETIMESFORM_H

#include "ui_modifyconstraintteachernotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherNotAvailableTimesForm : public QDialog, Ui::ModifyConstraintTeacherNotAvailableTimesForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherNotAvailableTimes* _ctr;

	ModifyConstraintTeacherNotAvailableTimesForm(QWidget* parent, ConstraintTeacherNotAvailableTimes* ctr);
	~ModifyConstraintTeacherNotAvailableTimesForm();

	void updateTeachersComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:
	void ok();
	void cancel();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);

	void setAllAvailable();
	void setAllNotAvailable();
};

#endif
