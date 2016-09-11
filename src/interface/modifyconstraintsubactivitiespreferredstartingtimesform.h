/***************************************************************************
                          modifyconstraintsubactivitiespreferredstartingtimesform.h  -  description
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

#ifndef MODIFYCONSTRAINTSUBACTIVITIESPREFERREDSTARTINGTIMESFORM_H
#define MODIFYCONSTRAINTSUBACTIVITIESPREFERREDSTARTINGTIMESFORM_H

#include "ui_modifyconstraintsubactivitiespreferredstartingtimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintSubactivitiesPreferredStartingTimesForm : public QDialog, Ui::ModifyConstraintSubactivitiesPreferredStartingTimesForm_template  {
	Q_OBJECT

public:
	ConstraintSubactivitiesPreferredStartingTimes* _ctr;

	ModifyConstraintSubactivitiesPreferredStartingTimesForm(QWidget* parent, ConstraintSubactivitiesPreferredStartingTimes* ctr);
	~ModifyConstraintSubactivitiesPreferredStartingTimesForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox(QWidget* parent);
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:
	void ok();
	void cancel();
	
	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void setAllSlotsAllowed();
	void setAllSlotsNotAllowed();
};

#endif
