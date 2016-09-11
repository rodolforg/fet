/***************************************************************************
                          modifyconstraintsubjectpreferredroomsform.h  -  description
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

#ifndef MODIFYCONSTRAINTSUBJECTPREFERREDROOMSFORM_H
#define MODIFYCONSTRAINTSUBJECTPREFERREDROOMSFORM_H

#include "ui_modifyconstraintsubjectpreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintSubjectPreferredRoomsForm : public QDialog, Ui::ModifyConstraintSubjectPreferredRoomsForm_template  {
	Q_OBJECT
public:
	ModifyConstraintSubjectPreferredRoomsForm(QWidget* parent, ConstraintSubjectPreferredRooms* ctr);
	~ModifyConstraintSubjectPreferredRoomsForm();

	void updateRoomsListWidget();
	
public slots:
	void addRoom();
	void removeRoom();

	void ok();
	void cancel();
	
	void clear();
private:
	ConstraintSubjectPreferredRooms* _ctr;
};

#endif
