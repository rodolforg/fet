//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "addstudentsyearform.h"
#include "modifystudentsyearform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "yearsform.h"
#include "studentsset.h"

#include "splityearform.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

YearsForm::YearsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	detailsTextEdit->setReadOnly(true);
	
	modifyYearPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addYearPushButton, SIGNAL(clicked()), this, SLOT(addYear()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeYearPushButton, SIGNAL(clicked()), this, SLOT(removeYear()));
	connect(yearsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(yearChanged()));
	connect(modifyYearPushButton, SIGNAL(clicked()), this, SLOT(modifyYear()));

	connect(moveYearUpPushButton, SIGNAL(clicked()), this, SLOT(moveYearUp()));
	connect(moveYearDownPushButton, SIGNAL(clicked()), this, SLOT(moveYearDown()));

	connect(sortYearsPushButton, SIGNAL(clicked()), this, SLOT(sortYears()));
	connect(activateStudentsPushButton, SIGNAL(clicked()), this, SLOT(activateStudents()));
	connect(deactivateStudentsPushButton, SIGNAL(clicked()), this, SLOT(deactivateStudents()));
	connect(divisionsPushButton, SIGNAL(clicked()), this, SLOT(divideYear()));
	connect(yearsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyYear()));

	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(comments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
}

YearsForm::~YearsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void YearsForm::addYear()
{
	AddStudentsYearForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	int i=yearsListWidget->count()-1;
	if(i>=0)
		yearsListWidget->setCurrentRow(i);
}

void YearsForm::removeYear()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete year %1 and all related groups, subgroups, activities and constraints?").arg(yearName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeYear(yearName);
	assert(tmp);
	if(tmp){
		int q=yearsListWidget->currentRow();

		yearsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=yearsListWidget->takeItem(q);
		delete item;

		if(q>=yearsListWidget->count())
			q=yearsListWidget->count()-1;
		if(q>=0)
			yearsListWidget->setCurrentRow(q);
		else
			detailsTextEdit->setPlainText(QString(""));
	}
}

void YearsForm::yearChanged()
{
	if(yearsListWidget->currentRow()<0){
		detailsTextEdit->setPlainText("");
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearsListWidget->currentRow());
	detailsTextEdit->setPlainText(sty->getDetailedDescriptionWithConstraints(gt.rules));
}

void YearsForm::moveYearUp()
{
	if(yearsListWidget->count()<=1)
		return;
	int i=yearsListWidget->currentRow();
	if(i<0 || i>=yearsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=yearsListWidget->item(i)->text();
	QString s2=yearsListWidget->item(i-1)->text();
	
	StudentsYear* sy1=gt.rules.yearsList.at(i);
	StudentsYear* sy2=gt.rules.yearsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	yearsListWidget->item(i)->setText(s2);
	yearsListWidget->item(i-1)->setText(s1);
	
	gt.rules.yearsList[i]=sy2;
	gt.rules.yearsList[i-1]=sy1;
	
	yearsListWidget->setCurrentRow(i-1);
	yearChanged(/*i-1*/);
}

void YearsForm::moveYearDown()
{
	if(yearsListWidget->count()<=1)
		return;
	int i=yearsListWidget->currentRow();
	if(i<0 || i>=yearsListWidget->count())
		return;
	if(i==yearsListWidget->count()-1)
		return;
		
	QString s1=yearsListWidget->item(i)->text();
	QString s2=yearsListWidget->item(i+1)->text();
	
	StudentsYear* sy1=gt.rules.yearsList.at(i);
	StudentsYear* sy2=gt.rules.yearsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	yearsListWidget->item(i)->setText(s2);
	yearsListWidget->item(i+1)->setText(s1);
	
	gt.rules.yearsList[i]=sy2;
	gt.rules.yearsList[i+1]=sy1;
	
	yearsListWidget->setCurrentRow(i+1);
	yearChanged(/*i+1*/);
}

void YearsForm::sortYears()
{
	gt.rules.sortYearsAlphabetically();

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}

	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
}

void YearsForm::modifyYear()
{
	int q=yearsListWidget->currentRow();
	int valv=yearsListWidget->verticalScrollBar()->value();
	int valh=yearsListWidget->horizontalScrollBar()->value();

	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* sset=gt.rules.searchStudentsSet(yearName);
	assert(sset!=NULL);
	int numberOfStudents=sset->numberOfStudents;

	ModifyStudentsYearForm form(this, yearName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	yearsListWidget->verticalScrollBar()->setValue(valv);
	yearsListWidget->horizontalScrollBar()->setValue(valh);

	if(q>=yearsListWidget->count())
		q=yearsListWidget->count()-1;
	if(q>=0)
		yearsListWidget->setCurrentRow(q);
	else
		detailsTextEdit->setPlainText(QString(""));
}

void YearsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void YearsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void YearsForm::divideYear()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	SplitYearForm form(this, yearName);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void YearsForm::comments()
{
	int ind=yearsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* sset=gt.rules.searchStudentsSet(yearName);
	assert(sset!=NULL);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students year comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(accept()));
	connect(cancelPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(reject()));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QPlainTextEdit* commentsPT=new QPlainTextEdit();
	commentsPT->setPlainText(sset->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("StudentsYearCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		sset->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		yearChanged();
	}
}
