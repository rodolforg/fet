/***************************************************************************
                          addconstraintactivitiesendstudentsdayform.h  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H
#define ADDCONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H

#include "ui_addconstraintactivitiesendstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintActivitiesEndStudentsDayForm : public QDialog, Ui::AddConstraintActivitiesEndStudentsDayForm_template  {
	Q_OBJECT

public:
	AddConstraintActivitiesEndStudentsDayForm(QWidget* parent);
	~AddConstraintActivitiesEndStudentsDayForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox();
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

public slots:
	void addConstraint();
};

#endif
