#ifndef PARADOXMINDAYSVSCONSECUTIVE_H
#define PARADOXMINDAYSVSCONSECUTIVE_H

#include "constraintpre.h"

class ParadoxMinDaysVsConsecutive : public ConstraintPre
{
public:
	ParadoxMinDaysVsConsecutive();

public:
	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSCONSECUTIVE_H
