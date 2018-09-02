/***************************************************************************
                          modifyconstraintstudentssetminrestinghoursform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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
#include "centerwidgetonscreen.h"
#include "invisiblesubgrouphelper.h"

#include "modifyconstraintstudentssetminrestinghoursform.h"
#include "timeconstraint.h"

#include "fetguisettings.h"
#include "studentscomboboxhelper.h"

ModifyConstraintStudentsSetMinRestingHoursForm::ModifyConstraintStudentsSetMinRestingHoursForm(QWidget* parent, ConstraintStudentsSetMinRestingHours* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	circularCheckBox->setChecked(ctr->circular);
	
	minRestingHoursSpinBox->setMinimum(1);
	minRestingHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minRestingHoursSpinBox->setValue(ctr->minRestingHours);
	
	updateStudentsComboBox();
}

ModifyConstraintStudentsSetMinRestingHoursForm::~ModifyConstraintStudentsSetMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMinRestingHoursForm::updateStudentsComboBox(){
	int j=StudentsComboBoxHelper::populateStudentsComboBox(studentsComboBox, this->_ctr->students, true);
	if(j<0)
		InvisibleSubgroupHelper::showWarningForConstraintCase(this, this->_ctr->students);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMinRestingHoursForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		InvisibleSubgroupHelper::showWarningCannotModifyConstraintCase(this, this->_ctr->students);
		return;
	}

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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;

	this->_ctr->minRestingHours=minRestingHoursSpinBox->value();
	this->_ctr->circular=circularCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);

	this->close();
}
