/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "conflictinfo.h"
#include <cassert>

void ConflictInfo::clear()
{
	weights.clear();
	descriptions.clear();
}

void ConflictInfo::append(double weight, const QString& description)
{
	weights.append(weight);
	descriptions.append(description);
}

int ConflictInfo::count() const
{
	assert(weights.count() == descriptions.count());
	return weights.count();
}
