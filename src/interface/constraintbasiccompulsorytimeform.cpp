/***************************************************************************
                          constraintbasiccompulsorytimeform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "constraintbasiccompulsorytimeform.h"
#include "addconstraintbasiccompulsorytimeform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"

#include <QMessageBox>

ConstraintBasicCompulsoryTimeForm::ConstraintBasicCompulsoryTimeForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintBasicCompulsoryTimeForm_template", "Constraints basic compulsory time"));

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintBasicCompulsoryTimeForm::~ConstraintBasicCompulsoryTimeForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintBasicCompulsoryTimeForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
		return true;
	else
		return false;
}

QDialog * ConstraintBasicCompulsoryTimeForm::createAddDialog()
{
	return new AddConstraintBasicCompulsoryTimeForm(this);
}

QDialog * ConstraintBasicCompulsoryTimeForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintBasicCompulsoryTimeForm(this, (ConstraintBasicCompulsoryTime*)ctr);
}

bool ConstraintBasicCompulsoryTimeForm::beforeRemoveConstraint()
{
	QString s=tr("Do you really want to remove the basic compulsory time constraint?");
	s+=" ";
	s+=tr("You cannot generate a timetable without this constraint.");
	s+="\n\n";
	s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
		"Miscellaneous -> Basic compulsory time constraints.");

	QMessageBox::StandardButton wr=QMessageBox::warning(this, tr("FET warning"), s,
														QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	return wr == QMessageBox::Yes;
}
