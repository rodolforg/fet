/***************************************************************************
                          addstudentsyearform.cpp  -  description
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

#include "addstudentsyearform.h"

#include <QMessageBox>

AddStudentsYearForm::AddStudentsYearForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	addStudentsYearPushButton->setDefault(true);

	connect(addStudentsYearPushButton, SIGNAL(clicked()), this, SLOT(addStudentsYear()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();

	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);
}

AddStudentsYearForm::~AddStudentsYearForm()
{
	saveFETDialogGeometry(this);
}

void AddStudentsYearForm::addStudentsYear()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}

	StudentsSet* ss=gt.rules.searchStudentsSet(nameLineEdit->text());
	if(ss!=NULL){
		if(ss->type==STUDENTS_SUBGROUP){
			QMessageBox::information( this, tr("Year insertion dialog"),
				tr("This name is taken for a subgroup - please consider another name"));

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}
		else if(ss->type==STUDENTS_GROUP){
			QMessageBox::information( this, tr("Year insertion dialog"),
				tr("This name is taken for a group - please consider another name"));

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}
		else if(ss->type==STUDENTS_YEAR){
			QMessageBox::information( this, tr("Year insertion dialog"),
				tr("This name is taken for a year - please consider another name"));

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}
		else
			assert(0);
	}

	StudentsYear* sy=new StudentsYear();
	sy->name=nameLineEdit->text();
	sy->numberOfStudents=numberSpinBox->value();
	
	if(gt.rules.searchYear(sy->name) >=0 ){
		QMessageBox::information( this, tr("Year insertion dialog"),
		tr("Could not insert item. Must be a duplicate"));
		delete sy;
	}
	else{
		bool tmp=gt.rules.addYear(sy);
		assert(tmp);

		QMessageBox::information(this, tr("Year insertion dialog"),
			tr("Year added. You might want to divide it into sections - this is done in the years "
			"dialog - button 'Divide year ...', or by manually adding groups and subgroups in the groups or subgroups menus."
			/*
			"\n\nImportant note: if you plan to use option 'divide', please try to use it only once for each year at the beginning, because"
			" a second use of option 'divide' for the same year will remove all activities and constraints referring to old groups and subgroups"
			" from this year."
			*/
			));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
