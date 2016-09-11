/***************************************************************************
                          addconstraintstudentssethomeroomform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETHOMEROOMFORM_H
#define ADDCONSTRAINTSTUDENTSSETHOMEROOMFORM_H

#include "ui_addconstraintstudentssethomeroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetHomeRoomForm : public QDialog, Ui::AddConstraintStudentsSetHomeRoomForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetHomeRoomForm(QWidget* parent);
	~AddConstraintStudentsSetHomeRoomForm();

	void updateStudentsComboBox();
	void updateRoomsComboBox();

public slots:
	void addConstraint();
};

#endif
