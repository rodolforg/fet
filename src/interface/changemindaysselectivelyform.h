/***************************************************************************
                          changemindaysselectivelyform.h  -  description
                             -------------------
    begin                : July 30, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CHANGEMINDAYSSELECTIVELYFORM_H
#define CHANGEMINDAYSSELECTIVELYFORM_H

#include "ui_changemindaysselectivelyform_template.h"
#include "timetable_defs.h"

class QLineEdit;
class QComboBox;
class QSpinBox;

class ChangeMinDaysSelectivelyForm : public QDialog, Ui::ChangeMinDaysSelectivelyForm_template  {
	Q_OBJECT

public:
	ChangeMinDaysSelectivelyForm(QWidget* parent);
	~ChangeMinDaysSelectivelyForm();
	
	double oldWeight;
	int oldDays;
	int oldConsecutive;
	int oldNActs;
	
	double newWeight;
	int newDays;
	int newConsecutive;

public slots:
	void ok();
	void cancel();
};

#endif
