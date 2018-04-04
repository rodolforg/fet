/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "invisiblesubgrouphelper.h"

#include <QCoreApplication>
#include <QMessageBox>

QString InvisibleSubgroupHelper::getWarningMessageForConstraintCase(const QString& initialSubgroupName)
{
	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current constraint is referring to subgroup %1, so the constraint is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");
	return message;
}

QString InvisibleSubgroupHelper::getWarningMessageCannotModifyConstraintCase(const QString& initialSubgroupName)
{
	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " You are trying to modify a constraint, leaving it to refer to the original subgroup %1, which is invisible in the combo box - but this is impossible.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");
	return message;
}

QString InvisibleSubgroupHelper::getWarningMessageForActivityCase(const QString& initialSubgroupName)
{
	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current activity is referring to subgroup %1, so the activity is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");
	return message;
}

void InvisibleSubgroupHelper::showWarningForConstraintCase(QWidget* parent, const QString& initialSubgroupName)
{
	showWarning(parent, getWarningMessageForConstraintCase(initialSubgroupName));
}

void InvisibleSubgroupHelper::showWarningCannotModifyConstraintCase(QWidget* parent, const QString& initialSubgroupName)
{
	showWarning(parent, getWarningMessageCannotModifyConstraintCase(initialSubgroupName));
}

void InvisibleSubgroupHelper::showWarningForActivityCase(QWidget* parent, const QString& initialSubgroupName)
{
	showWarning(parent, getWarningMessageForActivityCase(initialSubgroupName));
}

void InvisibleSubgroupHelper::showWarning(QWidget* parent, const QString& message)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");
	QMessageBox::warning(parent, title, message);
}
