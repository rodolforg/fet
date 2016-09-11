/***************************************************************************
                          addconstraintthreeactivitiesgroupedform.h  -  description
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

#ifndef ADDCONSTRAINTTHREEACTIVITIESGROUPEDFORM_H
#define ADDCONSTRAINTTHREEACTIVITIESGROUPEDFORM_H

#include "ui_addconstraintthreeactivitiesgroupedform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintThreeActivitiesGroupedForm : public QDialog, Ui::AddConstraintThreeActivitiesGroupedForm_template  {
	Q_OBJECT
public:
	AddConstraintThreeActivitiesGroupedForm(QWidget* parent);
	~AddConstraintThreeActivitiesGroupedForm();

	bool filterOk(Activity* a);

	void updateActivitiesComboBox();

public slots:
//	void constraintChanged();
	void addCurrentConstraint();
	
	void filterChanged();
	
private:
	//the id's of the activities listed in the first and second activities combo
	QList<int> firstActivitiesList;
	QList<int> secondActivitiesList;
	QList<int> thirdActivitiesList;
};

#endif
