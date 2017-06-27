#ifndef MINGAPSBETWEENACTIVITIES_H
#define MINGAPSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "constraints/constraintpre.h"

class MinGapsBetweenActivities : public ConstraintPre
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minGaps;
	Matrix1D<QList<double> > weightPercentages;

	MinGapsBetweenActivities();

	bool prepare(const Rules &rules);
};

#endif // MINGAPSBETWEENACTIVITIES_H
