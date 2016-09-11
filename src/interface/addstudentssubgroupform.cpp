/***************************************************************************
                          addstudentssubgroupform.cpp  -  description
                             -------------------
    begin                : Sat Jan 24 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addstudentssubgroupform.h"

#include <QMessageBox>

AddStudentsSubgroupForm::AddStudentsSubgroupForm(QWidget* parent, const QString& yearName, const QString& groupName): QDialog(parent)
{
	setupUi(this);

	addStudentsSubgroupPushButton->setDefault(true);

	connect(addStudentsSubgroupPushButton, SIGNAL(clicked()), this, SLOT(addStudentsSubgroup()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	yearNameLineEdit->setText(yearName);
	groupNameLineEdit->setText(groupName);

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();

	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);
}

AddStudentsSubgroupForm::~AddStudentsSubgroupForm()
{
	saveFETDialogGeometry(this);
}

void AddStudentsSubgroupForm::addStudentsSubgroup()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	QString subgroupName=nameLineEdit->text();
	QString yearName=yearNameLineEdit->text();
	QString groupName=groupNameLineEdit->text();

	if(gt.rules.searchSubgroup(yearName, groupName, subgroupName)>=0){
		QMessageBox::information( this, tr("Subgroup insertion dialog"),
			tr("Could not insert item. Must be a duplicate"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();
		
		return;
	}
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	StudentsSubgroup* sts;
	if(ss!=NULL && ss->type==STUDENTS_YEAR){
		QMessageBox::information( this, tr("Subgroup insertion dialog"),
			tr("This name is taken for a year - please consider another name"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	if(ss!=NULL && ss->type==STUDENTS_GROUP){
		QMessageBox::information( this, tr("Subgroup insertion dialog"),
			tr("This name is taken for a group - please consider another name"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	if(ss!=NULL){ //already existing subgroup, but in other group. Several groups share the same subgroup.
		assert(ss->type==STUDENTS_SUBGROUP);
		if(QMessageBox::warning( this, tr("FET"),
			tr("This subgroup already exists, but in another group. "
			"If you insert current subgroup to current group, that "
			"means that some groups share the same subgroup (overlap). "
			"If you want to make a new subgroup, independent, "
			"please abort now and give it another name.")+"\n\n"+tr("Note: the number of students for the added subgroup will be the number of students of the already existing subgroup "
			"(you can modify the number of students in the modify subgroup dialog)."),
			tr("Add"), tr("Abort"), 0, 0, 1 ) == 1){

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}

		numberSpinBox->setValue(ss->numberOfStudents);
		sts=(StudentsSubgroup*)ss;
	}
	else{
		sts=new StudentsSubgroup();
		sts->name=subgroupName;
		sts->numberOfStudents=numberSpinBox->value();
	}
	gt.rules.addSubgroup(yearName, groupName, sts);
	QMessageBox::information(this, tr("Subgroup insertion dialog"),
		tr("Subgroup added"));

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
