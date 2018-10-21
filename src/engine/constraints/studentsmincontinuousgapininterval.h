/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef STUDENTSMINCONTINUOUSGAPININTERVAL_H
#define STUDENTSMINCONTINUOUSGAPININTERVAL_H

#include <QList>
#include "matrix.h"
#include "constraints/constraintpre.h"

class MinContinuousGapInIntervalForStudents : public ConstraintPre
{
public:
	static const int MAX = 3;
	struct Data {
		int minGapDuration;
		int startHour;
		int endHour;
		double weightPercentage;
	} (*data)[MAX];

	MinContinuousGapInIntervalForStudents();
	virtual ~MinContinuousGapInIntervalForStudents();

	bool prepare(const Rules &rules);
};

#endif // STUDENTSMINCONTINUOUSGAPININTERVAL_H
