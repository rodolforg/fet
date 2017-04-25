/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H
#define CONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H

#include "timeconstraint_basedialog.h"

class ConstraintTeachersMinContinuousGapInIntervalForm : public TimeConstraintBaseDialog
{
	Q_OBJECT
public:
	ConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent);
	virtual ~ConstraintTeachersMinContinuousGapInIntervalForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(TimeConstraint *ctr);
	bool filterOk(const TimeConstraint *ctr) const;
};

#endif // CONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H
