/***************************************************************************
                          modifyconstraintteacherhomeroomform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERHOMEROOMFORM_H
#define MODIFYCONSTRAINTTEACHERHOMEROOMFORM_H

#include "ui_modifyconstraintteacherhomeroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherHomeRoomForm : public QDialog, Ui::ModifyConstraintTeacherHomeRoomForm_template  {
	Q_OBJECT
	
	ConstraintTeacherHomeRoom* _ctr;
public:
	ModifyConstraintTeacherHomeRoomForm(QWidget* parent, ConstraintTeacherHomeRoom* ctr);
	~ModifyConstraintTeacherHomeRoomForm();

	void updateTeachersComboBox();
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
