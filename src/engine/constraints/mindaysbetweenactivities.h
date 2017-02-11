#ifndef MINDAYSBETWEENACTIVITIES_H
#define MINDAYSBETWEENACTIVITIES_H

#include "matrix.h"
#include <QList>

class QWidget;

class MinDaysBetweenActivities
{
public:
	Matrix1D<QList<int> > activities;
	Matrix1D<QList<int> > minDays;
	Matrix1D<QList<double> > weightPercentages;
	Matrix1D<QList<bool> > consecutiveIfSameDay;

	MinDaysBetweenActivities();

	bool prepare(QWidget *parent);
};

#endif // MINDAYSBETWEENACTIVITIES_H
