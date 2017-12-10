#ifndef PARADOXMINDAYSVSCONSECUTIVE_H
#define PARADOXMINDAYSVSCONSECUTIVE_H

#include "constraintpre.h"

class ParadoxMinDaysVsConsecutive : public ConstraintPre
{
public:
	ParadoxMinDaysVsConsecutive();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSCONSECUTIVE_H
