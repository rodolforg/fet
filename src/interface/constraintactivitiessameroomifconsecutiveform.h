/***************************************************************************
                          constraintactivitiessameroomifconsecutiveform.h  -  description
                             -------------------
    begin                : Sept 14, 2013
    copyright            : (C) 2013 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H
#define CONSTRAINTACTIVITIESSAMEROOMIFCONSECUTIVEFORM_H

#include "spaceconstraint_basedialog.h"

class ConstraintActivitiesSameRoomIfConsecutiveForm : public SpaceConstraintBaseDialog  {
	Q_OBJECT
public:
	ConstraintActivitiesSameRoomIfConsecutiveForm(QWidget* parent);
	~ConstraintActivitiesSameRoomIfConsecutiveForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(SpaceConstraint *ctr);
	virtual bool filterOk(const SpaceConstraint *ctr) const;
};

#endif
