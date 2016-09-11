/***************************************************************************
                          modifyconstraintsubjectpreferredroomform.h  -  description
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

#ifndef MODIFYCONSTRAINTSUBJECTPREFERREDROOMFORM_H
#define MODIFYCONSTRAINTSUBJECTPREFERREDROOMFORM_H

#include "ui_modifyconstraintsubjectpreferredroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintSubjectPreferredRoomForm : public QDialog, Ui::ModifyConstraintSubjectPreferredRoomForm_template  {
	Q_OBJECT
	
	ConstraintSubjectPreferredRoom* _ctr;
public:
	ModifyConstraintSubjectPreferredRoomForm(QWidget* parent, ConstraintSubjectPreferredRoom* ctr);
	~ModifyConstraintSubjectPreferredRoomForm();

	void updateSubjectsComboBox();
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
