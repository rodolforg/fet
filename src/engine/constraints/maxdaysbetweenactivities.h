#ifndef MAXDAYSBETWEENACTIVITIES_H
#define MAXDAYSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "constraints/constraintpre.h"

class MaxDaysBetweenActivities : public ConstraintPre
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > maxDays;
	Matrix1D<QList<double> > weightPercentages;

	MaxDaysBetweenActivities();

	bool prepare(const Rules &rules);
};

#endif // MAXDAYSBETWEENACTIVITIES_H
