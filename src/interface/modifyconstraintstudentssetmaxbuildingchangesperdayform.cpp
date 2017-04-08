/***************************************************************************
                          modifyconstraintstudentssetmaxbuildingchangesperdayform.cpp  -  description
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

#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "spaceconstraint.h"

ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm(QWidget* parent, ConstraintStudentsSetMaxBuildingChangesPerDay* ctr): QDialog(parent)
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
	
	updateStudentsComboBox(parent);

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerDay);	
		
	constraintChanged();
}

ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::~ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::updateStudentsComboBox(QWidget* parent)
{
	studentsComboBox->clear();
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->addItem(sty->name);
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->addItem(stg->name);
			if(SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->addItem(sts->name);
			}
		}
	}
	if (studentsComboBox->findText(this->_ctr->studentsName) < 0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->studentsName);
	studentsComboBox->setCurrentText(this->_ctr->studentsName);

	constraintChanged();
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::constraintChanged()
{
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->studentsName);
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
	this->_ctr->maxBuildingChangesPerDay=maxChangesSpinBox->value();

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
