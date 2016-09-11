/***************************************************************************
                          modifyconstraintactivitiesendstudentsdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H
#define MODIFYCONSTRAINTACTIVITIESENDSTUDENTSDAYFORM_H

#include "ui_modifyconstraintactivitiesendstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivitiesEndStudentsDayForm : public QDialog, Ui::ModifyConstraintActivitiesEndStudentsDayForm_template  {
	Q_OBJECT

public:
	ConstraintActivitiesEndStudentsDay* _ctr;

	ModifyConstraintActivitiesEndStudentsDayForm(QWidget* parent, ConstraintActivitiesEndStudentsDay* ctr);
	~ModifyConstraintActivitiesEndStudentsDayForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox(QWidget* parent);
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
