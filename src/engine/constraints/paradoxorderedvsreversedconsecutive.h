/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXORDEREDVSREVERSEDCONSECUTIVE_H
#define PARADOXORDEREDVSREVERSEDCONSECUTIVE_H

#include "constraintpre.h"

class ParadoxOrderedVsReversedConsecutive : public ConstraintPre
{
public:
	ParadoxOrderedVsReversedConsecutive();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXORDEREDVSREVERSEDCONSECUTIVE_H
