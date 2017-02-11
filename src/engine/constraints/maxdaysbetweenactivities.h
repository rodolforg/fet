#ifndef MAXDAYSBETWEENACTIVITIES_H
#define MAXDAYSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "rules.h"

class MaxDaysBetweenActivities
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > maxDays;
	Matrix1D<QList<double> > weightPercentages;

	QStringList errors;

	MaxDaysBetweenActivities();

	bool prepare(const Rules &rules);

	QStringList getErrors() const;
};

#endif // MAXDAYSBETWEENACTIVITIES_H
