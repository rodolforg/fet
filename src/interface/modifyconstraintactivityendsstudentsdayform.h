/***************************************************************************
                          modifyconstraintactivityendsstudentsdayform.h  -  description
                             -------------------
    begin                : Sept 14, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTACTIVITYENDSSTUDENTSDAYFORM_H
#define MODIFYCONSTRAINTACTIVITYENDSSTUDENTSDAYFORM_H

#include "ui_modifyconstraintactivityendsstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityEndsStudentsDayForm : public QDialog, Ui::ModifyConstraintActivityEndsStudentsDayForm_template  {
	Q_OBJECT
public:
	ConstraintActivityEndsStudentsDay* _ctr;

	ModifyConstraintActivityEndsStudentsDayForm(QWidget* parent, ConstraintActivityEndsStudentsDay* ctr);
	~ModifyConstraintActivityEndsStudentsDayForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

public slots:
	void constraintChanged();
	void ok();
	void cancel();

	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;

};

#endif
