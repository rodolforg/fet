/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXMINGAPSVSCONSECUTIVE_H
#define PARADOXMINGAPSVSCONSECUTIVE_H

#include "constraintpre.h"

class ParadoxMinGapsVsConsecutive : public ConstraintPre
{
public:
	ParadoxMinGapsVsConsecutive();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINGAPSVSCONSECUTIVE_H
