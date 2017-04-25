#ifndef TEACHERSMINCONTINUOUSGAPININTERVAL_H
#define TEACHERSMINCONTINUOUSGAPININTERVAL_H

#include <QList>
#include "matrix.h"
#include "rules.h"

class MinContinuousGapInIntervalForTeachers
{
	QStringList errors;

public:
	static const int MAX = 3;
	struct Data {
		int minGapDuration;
		int startHour;
		int endHour;
		double weightPercentage;
	} data[MAX_TEACHERS][MAX];

	MinContinuousGapInIntervalForTeachers();

	bool prepare(const Rules &rules);

	QStringList getErrors() const;
};

#endif // TEACHERSMINCONTINUOUSGAPININTERVAL_H
