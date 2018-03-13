/***************************************************************************
                          constraintsubactivitiespreferredstartingtimesform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "constraintsubactivitiespreferredstartingtimesform.h"
#include "addconstraintsubactivitiespreferredstartingtimesform.h"
#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"

#include "centerwidgetonscreen.h"

ConstraintSubactivitiesPreferredStartingTimesForm::ConstraintSubactivitiesPreferredStartingTimesForm(QWidget* parent): TimeConstraintBaseDialog(parent)
{
	//: This is the title of the dialog to see the list of all constraints of this type
	setWindowTitle(QCoreApplication::translate("ConstraintSubactivitiesPreferredStartingTimesForm_template", "Constraints subactivities preferred starting times"));

	setInstructionText(QCoreApplication::translate("ConstraintSubactivitiesPreferredStartingTimesForm_template", "This constraint is useful to constrain only the n-th component for activities. Please click Help button for details."));
	setHelp();

	restoreFETDialogGeometry(this);
	filterChanged();
}

ConstraintSubactivitiesPreferredStartingTimesForm::~ConstraintSubactivitiesPreferredStartingTimesForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintSubactivitiesPreferredStartingTimesForm::filterOk(const TimeConstraint* ctr) const
{
	if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES)
		return true;
	else
		return false;
}

QDialog * ConstraintSubactivitiesPreferredStartingTimesForm::createAddDialog()
{
	return new AddConstraintSubactivitiesPreferredStartingTimesForm(this);
}

QDialog * ConstraintSubactivitiesPreferredStartingTimesForm::createModifyDialog(TimeConstraint *ctr)
{
	return new ModifyConstraintSubactivitiesPreferredStartingTimesForm(this, (ConstraintSubactivitiesPreferredStartingTimes*)ctr);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::setHelp()
{
	QString s = tr(
	 "This constraint is used to specify that for some components of a type of"
	" activities, for instance for Mathematics activities, you need that from say 4-5"
	" per week, at least the first 2 to start early (say in the first 4 hours).")+
	+"\n\n"+
	tr("This is simple: just add 2 constraints of this type, with the component"
	" number 1, (then 2) and the corresponding subject (you can also"
	" define teacher, students set and activity tag, of course)."
	 )
	 +"\n\n"+
	 tr("Note: if you have Maths with 4 and 5 splits and want to constrain 2 subactivities for those with 4 per week and 3 subactivities"
	 " for those with 5 per week, you can add constraints only for Maths with components 3, 4 and 5 (nice trick)")
	 +"\n\n"+
	 tr("Note: if an activity does not belong to a larger group (is a single activity),"
	 " it is considered to have component number 1. As an advice, if you have activities of say Biology split into 1"
	 " or 2 per week and want to constrain only one component if there are 2 per week, add a constraint for Biology with component number=2")
	 +"\n\n"+
	 tr("Note: if a teacher or a students set cannot have lessons in some slots because of this constraint, gaps will be counted (if you have max gaps constraints)"
	  ". So be careful if you add this constraint for all sub-activities, for only a teacher or for only a students set");

	setHelpText(s);
}
