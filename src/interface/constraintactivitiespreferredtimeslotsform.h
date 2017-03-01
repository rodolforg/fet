/***************************************************************************
                          constraintactivitiespreferredtimeslots.h  -  description
                             -------------------
    begin                : 15 May 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITIESPREFERREDTIMESLOTSFORM_H
#define CONSTRAINTACTIVITIESPREFERREDTIMESLOTSFORM_H

#include "constraint_basedialog.h"

class ConstraintActivitiesPreferredTimeSlotsForm : public ConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintActivitiesPreferredTimeSlotsForm(QWidget* parent);
	~ConstraintActivitiesPreferredTimeSlotsForm();

	bool filterOk(const TimeConstraint* ctr) const;

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
};

#endif
