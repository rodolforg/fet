/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PARADOXMINGAPSVS3GROUPED_H
#define PARADOXMINGAPSVS3GROUPED_H

#include "constraintpre.h"

class ParadoxMinGapsVs3Grouped : public ConstraintPre
{
public:
	ParadoxMinGapsVs3Grouped();

	virtual bool prepare(const Rules& rules);
};

#endif // PARADOXMINGAPSVS3GROUPED_H
