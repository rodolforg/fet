/***************************************************************************
                          addconstraintroomnotavailabletimesform.h  -  description
                             -------------------
    begin                : January 8, 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTROOMNOTAVAILABLETIMESFORM_H
#define ADDCONSTRAINTROOMNOTAVAILABLETIMESFORM_H

#include "ui_addconstraintroomnotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintRoomNotAvailableTimesForm : public QDialog, Ui::AddConstraintRoomNotAvailableTimesForm_template  {
	Q_OBJECT
public:
	AddConstraintRoomNotAvailableTimesForm(QWidget* parent);
	~AddConstraintRoomNotAvailableTimesForm();
	
	void updateRoomsComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:	
	void addConstraint();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);

	void setAllAvailable();
	void setAllNotAvailable();
};

#endif
