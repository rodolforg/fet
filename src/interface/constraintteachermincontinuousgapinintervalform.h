/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTTEACHERMINCONTINUOUSGAPININTERVALFORM_H
#define CONSTRAINTTEACHERMINCONTINUOUSGAPININTERVALFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintTeacherMinContinuousGapInIntervalForm : public TimeConstraintBaseDialog
{
	Q_OBJECT
public:
	ConstraintTeacherMinContinuousGapInIntervalForm(QWidget *parent);
	virtual ~ConstraintTeacherMinContinuousGapInIntervalForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	bool filterOk(const TimeConstraint *ctr) const;
};

#endif // CONSTRAINTTEACHERMINCONTINUOUSGAPININTERVALFORM_H
