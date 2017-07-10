/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTSTUDENTSSETMINCONTINUOUSGAPININTERVALFORM_H
#define CONSTRAINTSTUDENTSSETMINCONTINUOUSGAPININTERVALFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintStudentsSetMinContinuousGapInIntervalForm : public TimeConstraintBaseDialog
{
	Q_OBJECT
public:
	ConstraintStudentsSetMinContinuousGapInIntervalForm(QWidget *parent);
	virtual ~ConstraintStudentsSetMinContinuousGapInIntervalForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	bool filterOk(const TimeConstraint *ctr) const;
};

#endif // CONSTRAINTSTUDENTSSETMINCONTINUOUSGAPININTERVALFORM_H
