/***************************************************************************
                          modifyconstraintactivitypreferredroomform.h  -  description
                             -------------------
    begin                : February 13, 2005
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDROOMFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDROOMFORM_H

#include "ui_modifyconstraintactivitypreferredroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityPreferredRoomForm : public QDialog, Ui::ModifyConstraintActivityPreferredRoomForm_template  {
	Q_OBJECT
	
	ConstraintActivityPreferredRoom* _ctr;
public:
	ModifyConstraintActivityPreferredRoomForm(QWidget* parent, ConstraintActivityPreferredRoom* ctr);
	~ModifyConstraintActivityPreferredRoomForm();

	void updateActivitiesComboBox();
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
