/***************************************************************************
                          modifyconstraintactivitypreferredroomsform.h  -  description
                             -------------------
    begin                : March 28, 2005
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDROOMSFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDROOMSFORM_H

#include "ui_modifyconstraintactivitypreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityPreferredRoomsForm : public QDialog, Ui::ModifyConstraintActivityPreferredRoomsForm_template  {
	Q_OBJECT
public:
	ModifyConstraintActivityPreferredRoomsForm(QWidget* parent, ConstraintActivityPreferredRooms* ctr);
	~ModifyConstraintActivityPreferredRoomsForm();

	void updateRoomsListWidget();
	
public slots:
	void addRoom();
	void removeRoom();

	void ok();
	void cancel();
	
	void clear();
private:
	ConstraintActivityPreferredRooms* _ctr;
};

#endif
