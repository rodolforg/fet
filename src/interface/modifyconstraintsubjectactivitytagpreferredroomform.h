/***************************************************************************
                          modifyconstraintsubjectactivitytagpreferredroomform.h  -  description
                             -------------------
    begin                : Aug 18, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMFORM_H
#define MODIFYCONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMFORM_H

#include "ui_modifyconstraintsubjectactivitytagpreferredroomform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintSubjectActivityTagPreferredRoomForm : public QDialog, Ui::ModifyConstraintSubjectActivityTagPreferredRoomForm_template  {
	Q_OBJECT
	
	ConstraintSubjectActivityTagPreferredRoom* _ctr;
public:
	ModifyConstraintSubjectActivityTagPreferredRoomForm(QWidget* parent, ConstraintSubjectActivityTagPreferredRoom* ctr);
	~ModifyConstraintSubjectActivityTagPreferredRoomForm();

	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
