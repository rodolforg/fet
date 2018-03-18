/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXMINGAPSVSMINDAYSCONSECUTIVE_H
#define PARADOXMINGAPSVSMINDAYSCONSECUTIVE_H

#include "constraintpre.h"

class ParadoxMinGapsVsMinDaysConsecutive : public ConstraintPre
{
public:
	ParadoxMinGapsVsMinDaysConsecutive();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINGAPSVSMINDAYSCONSECUTIVE_H
