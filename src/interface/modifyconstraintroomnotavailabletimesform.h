/***************************************************************************
                          modifyconstraintroomnotavailabletimesform.h  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#ifndef MODIFYCONSTRAINTROOMNOTAVAILABLETIMESFORM_H
#define MODIFYCONSTRAINTROOMNOTAVAILABLETIMESFORM_H

#include "ui_modifyconstraintroomnotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintRoomNotAvailableTimesForm : public QDialog, Ui::ModifyConstraintRoomNotAvailableTimesForm_template  {
	Q_OBJECT	

	ConstraintRoomNotAvailableTimes* _ctr;
public:
	ModifyConstraintRoomNotAvailableTimesForm(QWidget* parent, ConstraintRoomNotAvailableTimes* ctr);
	~ModifyConstraintRoomNotAvailableTimesForm();
	
	void updateRoomsComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:
	void ok();
	void cancel();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);

	void setAllAvailable();
	void setAllNotAvailable();
};

#endif
