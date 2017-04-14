/***************************************************************************
                          constraintteachersmaxbuildingchangesperdayform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTTEACHERSMAXBUILDINGCHANGESPERDAYFORM_H
#define CONSTRAINTTEACHERSMAXBUILDINGCHANGESPERDAYFORM_H

#include "spaceconstraint_basedialog.h"

class ConstraintTeachersMaxBuildingChangesPerDayForm : public SpaceConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintTeachersMaxBuildingChangesPerDayForm(QWidget* parent);
	~ConstraintTeachersMaxBuildingChangesPerDayForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(SpaceConstraint *ctr);
	virtual bool filterOk(const SpaceConstraint *ctr) const;
};

#endif
