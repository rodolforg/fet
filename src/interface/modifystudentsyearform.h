/***************************************************************************
                          modifystudentsyearform.h  -  description
                             -------------------
    begin                : Feb 8, 2005
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

#ifndef MODIFYSTUDENTSYEARFORM_H
#define MODIFYSTUDENTSYEARFORM_H

#include "ui_modifystudentsyearform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyStudentsYearForm : public QDialog, Ui::ModifyStudentsYearForm_template {
	Q_OBJECT
public:
	QString _initialYearName;
	int _initialNumberOfStudents;

	ModifyStudentsYearForm(QWidget* parent, const QString& initialYearName, int initialNumberOfStudents);
	~ModifyStudentsYearForm();

public slots:
	void ok();
	void cancel();
};

#endif
