/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXMINDAYSVSTWOGROUPED_H
#define PARADOXMINDAYSVSTWOGROUPED_H

#include "constraintpre.h"

class ParadoxMinDaysVsTwoGrouped : public ConstraintPre
{
public:
	ParadoxMinDaysVsTwoGrouped();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINDAYSVSTWOGROUPED_H
