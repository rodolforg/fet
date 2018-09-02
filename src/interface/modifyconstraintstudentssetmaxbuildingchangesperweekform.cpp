/***************************************************************************
                          modifyconstraintstudentssetmaxbuildingchangesperweekform.cpp  -  description
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
#include "centerwidgetonscreen.h"
#include "invisiblesubgrouphelper.h"

#include "modifyconstraintstudentssetmaxbuildingchangesperweekform.h"
#include "spaceconstraint.h"

#include "fetguisettings.h"
#include "studentscomboboxhelper.h"

ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm::ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm(QWidget* parent, ConstraintStudentsSetMaxBuildingChangesPerWeek* ctr): QDialog(parent)
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
	
	updateStudentsComboBox();

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerWeek);	
}

ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm::~ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm::updateStudentsComboBox(){
	int j=StudentsComboBoxHelper::populateStudentsComboBox(gt.rules, studentsComboBox, this->_ctr->studentsName, true);
	if(j<0)
		InvisibleSubgroupHelper::showWarningForConstraintCase(this, this->_ctr->studentsName);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		InvisibleSubgroupHelper::showWarningCannotModifyConstraintCase(this, this->_ctr->studentsName);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
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
	this->_ctr->studentsName=students_name;
	this->_ctr->maxBuildingChangesPerWeek=maxChangesSpinBox->value();

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
