/***************************************************************************
                          modifyconstraintstudentssetmaxspanperdayform.cpp  -  description
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

#include "modifyconstraintstudentssetmaxspanperdayform.h"
#include "timeconstraint.h"

#include "fetguisettings.h"
#include "studentscomboboxhelper.h"

ModifyConstraintStudentsSetMaxSpanPerDayForm::ModifyConstraintStudentsSetMaxSpanPerDayForm(QWidget* parent, ConstraintStudentsSetMaxSpanPerDay* ctr): QDialog(parent)
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

	maxSpanSpinBox->setMinimum(1);
	maxSpanSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxSpanSpinBox->setValue(ctr->maxSpanPerDay);
	
	updateStudentsComboBox();
}

ModifyConstraintStudentsSetMaxSpanPerDayForm::~ModifyConstraintStudentsSetMaxSpanPerDayForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMaxSpanPerDayForm::updateStudentsComboBox(){
	int j=StudentsComboBoxHelper::populateStudentsComboBox(studentsComboBox, this->_ctr->students, true);
	if(j<0)
		InvisibleSubgroupHelper::showWarningForConstraintCase(this, this->_ctr->students);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMaxSpanPerDayForm::ok()
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
	this->_ctr->maxSpanPerDay=maxSpanSpinBox->value();
	this->_ctr->students=students_name;

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);

	this->close();
}
