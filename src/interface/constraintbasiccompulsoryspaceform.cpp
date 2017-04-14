/***************************************************************************
                          constraintbasiccompulsoryspaceform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include <QMessageBox>

#include "constraintbasiccompulsoryspaceform.h"
#include "addconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintbasiccompulsoryspaceform.h"

ConstraintBasicCompulsorySpaceForm::ConstraintBasicCompulsorySpaceForm(QWidget* parent): SpaceConstraintBaseDialog(parent)
{
	const char *context = "ConstraintBasicCompulsorySpaceForm_template";
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate(context, "Constraints basic compulsory space"));

	restoreFETDialogGeometry(this);
	this->filterChanged();
}

ConstraintBasicCompulsorySpaceForm::~ConstraintBasicCompulsorySpaceForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintBasicCompulsorySpaceForm::filterOk(const SpaceConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
		return true;
	else
		return false;
}

bool ConstraintBasicCompulsorySpaceForm::beforeRemoveConstraint()
{
	QString s=tr("Do you really want to remove the basic compulsory space constraint?");
	s+=" ";
	s+=tr("You cannot generate a timetable without this constraint.");
	s+="\n\n";
	s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
		"Miscellaneous -> Basic compulsory space constraints.");

	QMessageBox::StandardButton wr=QMessageBox::warning(this, tr("FET warning"), s,
		QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

	return wr==QMessageBox::Yes;
}

QDialog * ConstraintBasicCompulsorySpaceForm::createAddDialog()
{
	return new AddConstraintBasicCompulsorySpaceForm(this);
}

QDialog * ConstraintBasicCompulsorySpaceForm::createModifyDialog(SpaceConstraint *ctr)
{
	return new ModifyConstraintBasicCompulsorySpaceForm(this, (ConstraintBasicCompulsorySpace*)ctr);
}
