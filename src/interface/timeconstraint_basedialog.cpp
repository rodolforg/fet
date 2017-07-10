/***************************************************************************
						  timeconstraint_basedialog.cpp  -  description
							 -------------------
	begin                : 2017
	copyright            : (C) 2017 by Rodolfo RG
	This file is part of a modification of FET timetable (the original is developed by Liviu Lalescu)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timeconstraint_basedialog.h"

#include "editcommentsform.h"

#include "fet.h"

TimeConstraintBaseDialog::TimeConstraintBaseDialog(QWidget *parent)
	: ConstraintBaseDialog(parent)
{

}

QDialog *TimeConstraintBaseDialog::createModifyDialog(void *ctr)
{
	return createModifyDialog(static_cast<TimeConstraint*>(ctr));
}

void TimeConstraintBaseDialog::doRemoveConstraint(void *ctr)
{
	gt.rules.removeTimeConstraint(static_cast<TimeConstraint*>(ctr));
}

void TimeConstraintBaseDialog::fillConstraintList(QList<void *> &list)
{
	foreach (TimeConstraint *ctr, gt.rules.timeConstraintsList){
		if(filterOk(ctr)){
			QString s=ctr->getDescription(gt.rules);
			list.append(ctr);
			constraintsListWidget->addItem(s);
		}
	}
}

QString TimeConstraintBaseDialog::getConstraintDetailedDescription(const void *ctr) const
{
	return static_cast<const TimeConstraint *>(ctr)->getDetailedDescription(gt.rules);
}

void TimeConstraintBaseDialog::editComments(void *ctr)
{
	TimeConstraint *tctr = static_cast<TimeConstraint *>(ctr);
	EditCommentsForm dialog("TimeConstraintCommentsDialog", this, tr("Constraint comments"));
	dialog.setComments(tctr->comments);

	int t=dialog.exec();
	if(t!=QDialog::Accepted)
		return;

	if (tctr->comments == dialog.getComments())
		return;
	tctr->comments=dialog.getComments();
	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
}

bool TimeConstraintBaseDialog::isConstraintActive(const void *ctr) const
{
	return static_cast<const TimeConstraint *>(ctr)->active;
}

void TimeConstraintBaseDialog::toggleActiveConstraint(void *ctr, bool checked) const
{
	TimeConstraint *tctr = static_cast<TimeConstraint *>(ctr);
	tctr->active = checked;
	gt.rules.internalStructureComputed = false;
	gt.rules.setModified(true);
}
