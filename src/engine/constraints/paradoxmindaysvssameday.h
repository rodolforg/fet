/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

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
