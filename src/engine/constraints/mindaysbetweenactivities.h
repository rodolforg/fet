#ifndef MINDAYSBETWEENACTIVITIES_H
#define MINDAYSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "constraints/constraintpre.h"

class MinDaysBetweenActivities : public ConstraintPre
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minDays;
	Matrix1D<QList<double> > weightPercentages;
	Matrix1D<QList<bool> > consecutiveIfSameDay;

	MinDaysBetweenActivities();

	bool prepare(const Rules &rules);
};

#endif // MINDAYSBETWEENACTIVITIES_H
