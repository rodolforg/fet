/***************************************************************************
                          constraintsubactivitiespreferredtimeslots.h  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CONSTRAINTSUBACTIVITIESPREFERREDTIMESLOTSFORM_H
#define CONSTRAINTSUBACTIVITIESPREFERREDTIMESLOTSFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintSubactivitiesPreferredTimeSlotsForm : public TimeConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintSubactivitiesPreferredTimeSlotsForm(QWidget* parent);
	~ConstraintSubactivitiesPreferredTimeSlotsForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	virtual bool filterOk(const TimeConstraint *ctr) const;

	void setHelp();
};

#endif
