/***************************************************************************
                          modifyconstraintactivitypreferredstartingtimeform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMEFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMEFORM_H

#include "ui_modifyconstraintactivitypreferredstartingtimeform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityPreferredStartingTimeForm : public QDialog, Ui::ModifyConstraintActivityPreferredStartingTimeForm_template  {
	Q_OBJECT
public:
	ConstraintActivityPreferredStartingTime* _ctr;

	ModifyConstraintActivityPreferredStartingTimeForm(QWidget* parent, ConstraintActivityPreferredStartingTime* ctr);
	~ModifyConstraintActivityPreferredStartingTimeForm();

	void updatePeriodGroupBox();
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
