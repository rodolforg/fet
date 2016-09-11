/***************************************************************************
                          modifyconstraintteachersmingapsbetweenbuildingchangesform.cpp  -  description
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

#include <QMessageBox>



#include "modifyconstraintteachersmingapsbetweenbuildingchangesform.h"
#include "spaceconstraint.h"

ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm::ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm(QWidget* parent, ConstraintTeachersMinGapsBetweenBuildingChanges* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
		
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(ctr->minGapsBetweenBuildingChanges);	
		
	constraintChanged();
}

ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm::~ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm::constraintChanged()
{
}

void ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->minGapsBetweenBuildingChanges=minGapsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm::cancel()
{
	this->close();
}
