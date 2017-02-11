#ifndef MINDAYSBETWEENACTIVITIES_H
#define MINDAYSBETWEENACTIVITIES_H

#include "matrix.h"
#include <QList>

class MinDaysBetweenActivities
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minDays;
	Matrix1D<QList<double> > weightPercentages;
	Matrix1D<QList<bool> > consecutiveIfSameDay;

	QStringList errors;

	MinDaysBetweenActivities();

	bool prepare();

	QStringList getErrors() const;
};

#endif // MINDAYSBETWEENACTIVITIES_H
