/***************************************************************************
                          modifyconstraintstudentssethomeroomform.h  -  description
                             -------------------
    begin                : April 8, 2005
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETHOMEROOMFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETHOMEROOMFORM_H

#include "ui_modifyconstraintstudentssethomeroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetHomeRoomForm : public QDialog, Ui::ModifyConstraintStudentsSetHomeRoomForm_template  {
	Q_OBJECT
	
	ConstraintStudentsSetHomeRoom* _ctr;
public:
	ModifyConstraintStudentsSetHomeRoomForm(QWidget* parent, ConstraintStudentsSetHomeRoom* ctr);
	~ModifyConstraintStudentsSetHomeRoomForm();

	void updateStudentsComboBox(QWidget* parent);
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
