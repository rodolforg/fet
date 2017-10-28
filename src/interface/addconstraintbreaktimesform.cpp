/***************************************************************************
                          addconstraintbreaktimesform.cpp  -  description
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintbreaktimesform.h"
#include "timeconstraint.h"

AddConstraintBreakTimesForm::AddConstraintBreakTimesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(setAllAllowedPushButton, SIGNAL(clicked()), this, SLOT(setAllAllowed()));
	connect(setAllBreakPushButton, SIGNAL(clicked()), this, SLOT(setAllBreak()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
							
	notAllowedTimesTable->setHeaders(gt.rules);
}

AddConstraintBreakTimesForm::~AddConstraintBreakTimesForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintBreakTimesForm::setAllAllowed()
{
	notAllowedTimesTable->setAllUnmarked();
}

void AddConstraintBreakTimesForm::setAllBreak()
{
	notAllowedTimesTable->setAllMarked();
}

void AddConstraintBreakTimesForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->isMarked(i, j)){
				days.append(j);
				hours.append(i);
			}

	ctr=new ConstraintBreakTimes(weight, days, hours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - there must be another constraint of this "
			  "type. Please edit that one"));
		delete ctr;
	}
}
