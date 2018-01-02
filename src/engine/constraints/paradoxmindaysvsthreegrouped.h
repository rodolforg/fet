#ifndef PARADOXMINDAYSVSTHREEGROUPED_H
#define PARADOXMINDAYSVSTHREEGROUPED_H

#include "constraintpre.h"

class ParadoxMinDaysVsThreeGrouped : public ConstraintPre
{
public:
	ParadoxMinDaysVsThreeGrouped();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSTHREEGROUPED_H
