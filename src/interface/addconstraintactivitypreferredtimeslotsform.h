/***************************************************************************
                          addconstraintactivitypreferredtimeslotsform.h  -  description
                             -------------------
    begin                : October 3, 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITYPREFERREDTIMESLOTSFORM_H
#define ADDCONSTRAINTACTIVITYPREFERREDTIMESLOTSFORM_H

#include "ui_addconstraintactivitypreferredtimeslotsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivityPreferredTimeSlotsForm : public QDialog, Ui::AddConstraintActivityPreferredTimeSlotsForm_template  {
	Q_OBJECT

public:
	AddConstraintActivityPreferredTimeSlotsForm(QWidget* parent);
	~AddConstraintActivityPreferredTimeSlotsForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

	void colorItem(QTableWidgetItem* item);

private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
	
public slots:
	void addConstraint();
	
	void filterChanged();
	
	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void setAllSlotsAllowed();
	void setAllSlotsNotAllowed();
};

#endif
