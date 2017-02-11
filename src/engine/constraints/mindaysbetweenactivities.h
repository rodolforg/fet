#ifndef MINDAYSBETWEENACTIVITIES_H
#define MINDAYSBETWEENACTIVITIES_H

#include <QList>
#include "matrix.h"
#include "rules.h"

class MinDaysBetweenActivities
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minDays;
	Matrix1D<QList<double> > weightPercentages;
	Matrix1D<QList<bool> > consecutiveIfSameDay;

	QStringList errors;

	MinDaysBetweenActivities();

	bool prepare(const Rules &rules);

	QStringList getErrors() const;
};

#endif // MINDAYSBETWEENACTIVITIES_H
