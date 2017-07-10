/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H
#define CONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintStudentsMinContinuousGapInIntervalForm : public TimeConstraintBaseDialog
{
	Q_OBJECT
public:
	ConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent);
	virtual ~ConstraintStudentsMinContinuousGapInIntervalForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	bool filterOk(const TimeConstraint *ctr) const;
};

#endif // CONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H
