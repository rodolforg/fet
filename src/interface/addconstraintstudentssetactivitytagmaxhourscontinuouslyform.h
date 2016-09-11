/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhourscontinuouslyform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSCONTINUOUSLYFORM_H
#define ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSCONTINUOUSLYFORM_H

#include "ui_addconstraintstudentssetactivitytagmaxhourscontinuouslyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm : public QDialog, Ui::AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(QWidget* parent);
	~AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm();
	
	void updateStudentsSetComboBox();
	void updateActivityTagsComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
