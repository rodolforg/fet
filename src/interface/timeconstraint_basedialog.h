/***************************************************************************
						  timeconstraint_basedialog.h  -  description
							 -------------------
	begin                : 2017
	copyright            : (C) 2017 by Rodolfo RG
	This file is part of a modification of FET timetable (the original is developed by Liviu Lalescu)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMECONSTRAINTBASEDIALOG_H
#define TIMECONSTRAINTBASEDIALOG_H

#include "constraint_basedialog.h"
#include "timeconstraint.h"

class TimeConstraintBaseDialog : public ConstraintBaseDialog
{
public:
	TimeConstraintBaseDialog(QWidget* parent);

protected:
	virtual QDialog *createModifyDialog(TimeConstraint *ctr) = 0;

	/**
	 * @brief Check if a constraint passes in the filter, i.e it should be shown in constraint list
	 * @param ctr the constraint
	 * @return true if it matches the filter
	 */
	virtual bool filterOk(const TimeConstraint *ctr) const = 0;

private:
	virtual QDialog *createModifyDialog(void *ctr);

	virtual void doRemoveConstraint(void *ctr);

	virtual void fillConstraintList(QList<void*>& list);

	virtual QString getConstraintDetailedDescription(const void* ctr) const;
};

#endif // TIMECONSTRAINTBASEDIALOG_H
