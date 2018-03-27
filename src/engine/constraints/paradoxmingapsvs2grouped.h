/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXMINGAPSVS2GROUPED_H
#define PARADOXMINGAPSVS2GROUPED_H

#include "constraintpre.h"

class ParadoxMinGapsVs2Grouped : public ConstraintPre
{
public:
	ParadoxMinGapsVs2Grouped();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINGAPSVS2GROUPED_H
