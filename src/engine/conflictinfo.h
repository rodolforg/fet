/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFLICTINFO_H
#define CONFLICTINFO_H

#include <QList>
#include <QString>

struct ConflictInfo {
	QList<double> weights;
	QList<QString> descriptions;
};

#endif // CONFLICTINFO_H
