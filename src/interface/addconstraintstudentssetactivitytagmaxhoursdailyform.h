/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhoursdailyform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSDAILYFORM_H
#define ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSDAILYFORM_H

#include "ui_addconstraintstudentssetactivitytagmaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetActivityTagMaxHoursDailyForm : public QDialog, Ui::AddConstraintStudentsSetActivityTagMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetActivityTagMaxHoursDailyForm(QWidget* parent);
	~AddConstraintStudentsSetActivityTagMaxHoursDailyForm();
	
	void updateStudentsSetComboBox();
	void updateActivityTagsComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
