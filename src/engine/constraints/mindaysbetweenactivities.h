#ifndef MINDAYSBETWEENACTIVITIES_H
#define MINDAYSBETWEENACTIVITIES_H

#include "matrix.h"
#include <QList>

class QWidget;

class MinDaysBetweenActivities
{
public:
	Matrix1D<QList<int> > minDaysListOfActivities;
	Matrix1D<QList<int> > minDaysListOfMinDays;
	Matrix1D<QList<double> > minDaysListOfWeightPercentages;
	Matrix1D<QList<bool> > minDaysListOfConsecutiveIfSameDay;

	MinDaysBetweenActivities();

	bool computeMinDays(QWidget *parent);
};

#endif // MINDAYSBETWEENACTIVITIES_H
