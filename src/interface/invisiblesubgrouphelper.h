/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef INVISIBLESUBGROUPHELPER_H
#define INVISIBLESUBGROUPHELPER_H

#include <QString>
#include <QWidget>

class InvisibleSubgroupHelper
{
public:
	static QString getWarningMessageForConstraintCase(const QString& initialSubgroupName);
	static QString getWarningMessageCannotModifyConstraintCase(const QString& initialSubgroupName);
	static QString getWarningMessageForActivityCase(const QString& initialSubgroupName);
	static void showWarningForConstraintCase(QWidget* parent, const QString& initialSubgroupName);
	static void showWarningCannotModifyConstraintCase(QWidget* parent, const QString& initialSubgroupName);
	static void showWarningForActivityCase(QWidget* parent, const QString& initialSubgroupName);

private:
	static void showWarning(QWidget* parent, const QString& message);
};

#endif // INVISIBLESUBGROUPHELPER_H
