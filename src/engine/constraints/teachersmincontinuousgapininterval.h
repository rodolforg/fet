/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TEACHERSMINCONTINUOUSGAPININTERVAL_H
#define TEACHERSMINCONTINUOUSGAPININTERVAL_H

#include <QList>
#include "matrix.h"
#include "rules.h"

class MinContinuousGapInIntervalForTeachers
{
	QStringList errors;

public:
	static const int MAX = 3;
	struct Data {
		int minGapDuration;
		int startHour;
		int endHour;
		double weightPercentage;
	} data[MAX_TEACHERS][MAX];

	MinContinuousGapInIntervalForTeachers();

	bool prepare(const Rules &rules);

	QStringList getErrors() const;
};

#endif // TEACHERSMINCONTINUOUSGAPININTERVAL_H
