/***************************************************************************
                          modifyconstraintstudentssethomeroomform.cpp  -  description
                             -------------------
    begin                : 8 Apr 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "modifyconstraintstudentssethomeroomform.h"
#include "spaceconstraint.h"

#include "fetguisettings.h"

ModifyConstraintStudentsSetHomeRoomForm::ModifyConstraintStudentsSetHomeRoomForm(QWidget* parent, ConstraintStudentsSetHomeRoom* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	updateStudentsComboBox();
	updateRoomsComboBox();
}

ModifyConstraintStudentsSetHomeRoomForm::~ModifyConstraintStudentsSetHomeRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetHomeRoomForm::updateStudentsComboBox(){
	studentsComboBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->addItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->addItem(stg->name);
			if(SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->addItem(sts->name);
			}
		}
	}
	if (studentsComboBox->findText(this->_ctr->studentsName) < 0)
		InvisibleSubgroupHelper::showWarningForConstraintCase(this, this->_ctr->studentsName);
	studentsComboBox->setCurrentText(this->_ctr->studentsName);
}

void ModifyConstraintStudentsSetHomeRoomForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* rm=gt.rules.roomsList[k];
		roomsComboBox->addItem(rm->name);
		if(rm->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetHomeRoomForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		InvisibleSubgroupHelper::showWarningCannotModifyConstraintCase(this, this->_ctr->studentsName);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	QString students=studentsComboBox->currentText();
	assert(gt.rules.searchStudentsSet(students)!=NULL);

	int i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	this->_ctr->weightPercentage=weight;
	this->_ctr->roomName=room;
	this->_ctr->studentsName=students;

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
