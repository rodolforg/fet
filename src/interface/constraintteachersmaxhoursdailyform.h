/***************************************************************************
                          constraintteachersmaxhoursdailyform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTTEACHERSMAXHOURSDAILYFORM_H
#define CONSTRAINTTEACHERSMAXHOURSDAILYFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintTeachersMaxHoursDailyForm : public TimeConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintTeachersMaxHoursDailyForm(QWidget* parent);
	~ConstraintTeachersMaxHoursDailyForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	virtual bool filterOk(const TimeConstraint *ctr) const;
};

#endif
