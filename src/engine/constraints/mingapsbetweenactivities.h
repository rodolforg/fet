#ifndef MINGAPSBETWEENACTIVITIES_H
#define MINGAPSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "rules.h"

class MinGapsBetweenActivities
{
	QStringList errors;

public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minGaps;
	Matrix1D<QList<double> > weightPercentages;

	MinGapsBetweenActivities();

	bool prepare(const Rules &rules);

	QStringList getErrors() const;
};

#endif // MINGAPSBETWEENACTIVITIES_H
