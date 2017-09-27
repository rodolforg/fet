#ifndef PARADOXMINDAYSVSTWOGROUPED_H
#define PARADOXMINDAYSVSTWOGROUPED_H

#include "constraintpre.h"

class ParadoxMinDaysVsTwoGrouped : public ConstraintPre
{
public:
	ParadoxMinDaysVsTwoGrouped();

public:
	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSTWOGROUPED_H
