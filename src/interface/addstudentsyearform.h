/***************************************************************************
                          addstudentsyearform.h  -  description
                             -------------------
    begin                : Sat Jan 24 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef ADDSTUDENTSYEARFORM_H
#define ADDSTUDENTSYEARFORM_H

#include "ui_addstudentsyearform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddStudentsYearForm : public QDialog, Ui::AddStudentsYearForm_template {
	Q_OBJECT
public:
	AddStudentsYearForm(QWidget* parent);
	~AddStudentsYearForm();

public slots:
	void addStudentsYear();
};

#endif
