/***************************************************************************
                          constraintactivitypreferredroomform.h  -  description
                             -------------------
    begin                : Feb 13, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITYPREFERREDROOMFORM_H
#define CONSTRAINTACTIVITYPREFERREDROOMFORM_H

#include "spaceconstraint_basedialog.h"

class ConstraintActivityPreferredRoomForm : public SpaceConstraintBaseDialog  {
	Q_OBJECT

public:
	ConstraintActivityPreferredRoomForm(QWidget* parent);
	~ConstraintActivityPreferredRoomForm();

protected:
	virtual QDialog *createAddDialog();
	virtual QDialog *createModifyDialog(SpaceConstraint *ctr);
	virtual bool filterOk(const SpaceConstraint *ctr) const;

	void afterRemoveConstraint();
};

#endif
