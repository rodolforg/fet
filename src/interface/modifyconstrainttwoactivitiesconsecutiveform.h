/***************************************************************************
                          modifyconstrainttwoactivitiesconsecutiveform.h  -  description
                             -------------------
    begin                : Aug 21, 2007
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

#ifndef MODIFYCONSTRAINTTWOACTIVITIESCONSECUTIVEFORM_H
#define MODIFYCONSTRAINTTWOACTIVITIESCONSECUTIVEFORM_H

#include "ui_modifyconstrainttwoactivitiesconsecutiveform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTwoActivitiesConsecutiveForm : public QDialog, Ui::ModifyConstraintTwoActivitiesConsecutiveForm_template  {
	Q_OBJECT
public:
	ConstraintTwoActivitiesConsecutive* _ctr;

	ModifyConstraintTwoActivitiesConsecutiveForm(QWidget* parent, ConstraintTwoActivitiesConsecutive* ctr);
	~ModifyConstraintTwoActivitiesConsecutiveForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

public slots:
	void constraintChanged();
	void ok();
	void cancel();

	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> firstActivitiesList;
	QList<int> secondActivitiesList;
};

#endif
