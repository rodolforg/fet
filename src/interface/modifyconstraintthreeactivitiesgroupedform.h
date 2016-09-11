/***************************************************************************
                          modifyconstraintthreeactivitiesgroupedform.h  -  description
                             -------------------
    begin                : Aug 14, 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTHREEACTIVITIESGROUPEDFORM_H
#define MODIFYCONSTRAINTTHREEACTIVITIESGROUPEDFORM_H

#include "ui_modifyconstraintthreeactivitiesgroupedform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintThreeActivitiesGroupedForm : public QDialog, Ui::ModifyConstraintThreeActivitiesGroupedForm_template  {
	Q_OBJECT
public:
	ConstraintThreeActivitiesGrouped* _ctr;

	ModifyConstraintThreeActivitiesGroupedForm(QWidget* parent, ConstraintThreeActivitiesGrouped* ctr);
	~ModifyConstraintThreeActivitiesGroupedForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

public slots:
	//void constraintChanged();
	void ok();
	void cancel();

	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> firstActivitiesList;
	QList<int> secondActivitiesList;
	QList<int> thirdActivitiesList;
};

#endif
