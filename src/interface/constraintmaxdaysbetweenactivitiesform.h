/***************************************************************************
                          constraintmaxdaysbetweenactivitiesform.h  -  description
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

#ifndef CONSTRAINTMAXDAYSBETWEENACTIVITIESFORM_H
#define CONSTRAINTMAXDAYSBETWEENACTIVITIESFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintMaxDaysBetweenActivitiesForm : public TimeConstraintBaseDialog  {
	Q_OBJECT

public:
	ConstraintMaxDaysBetweenActivitiesForm(QWidget* parent);
	~ConstraintMaxDaysBetweenActivitiesForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	virtual bool filterOk(const TimeConstraint *ctr) const;

	void setHelp();
};

#endif
