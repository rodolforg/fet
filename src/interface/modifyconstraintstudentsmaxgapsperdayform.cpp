/***************************************************************************
                          modifyconstraintstudentsmaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "modifyconstraintstudentsmaxgapsperdayform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxGapsPerDayForm::ModifyConstraintStudentsMaxGapsPerDayForm(QWidget* parent, ConstraintStudentsMaxGapsPerDay* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
		
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	maxGapsSpinBox->setMinimum(0);
	maxGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(ctr->maxGaps);
}

ModifyConstraintStudentsMaxGapsPerDayForm::~ModifyConstraintStudentsMaxGapsPerDayForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMaxGapsPerDayForm::constraintChanged()
{
}

void ModifyConstraintStudentsMaxGapsPerDayForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	
	this->_ctr->maxGaps=maxGapsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMaxGapsPerDayForm::cancel()
{
	this->close();
}
