#ifndef PARADOXMINDAYSVSSAMEDAY_H
#define PARADOXMINDAYSVSSAMEDAY_H

#include "constraintpre.h"

class ParadoxMinDaysVsSameDay : public ConstraintPre
{
public:
	ParadoxMinDaysVsSameDay();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSSAMEDAY_H
