/***************************************************************************
                          constraintteacherminhoursdailyform.h  -  description
                             -------------------
    begin                : Sept 21, 2007
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

#ifndef CONSTRAINTTEACHERMINHOURSDAILYFORM_H
#define CONSTRAINTTEACHERMINHOURSDAILYFORM_H

#include "constraint_basedialog.h"

class ConstraintTeacherMinHoursDailyForm : public ConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintTeacherMinHoursDailyForm(QWidget* parent);
	~ConstraintTeacherMinHoursDailyForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	virtual bool filterOk(const TimeConstraint *ctr) const;
};

#endif
