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

#include "addstudentsgroupform.h"
#include "modifystudentsgroupform.h"
#include "groupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "studentsset.h"

#include "longtextmessagebox.h"

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

GroupsForm::GroupsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	groupTextEdit->setReadOnly(true);
	
	modifyGroupPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
	connect(addGroupPushButton, SIGNAL(clicked()), this, SLOT(addGroup()));
	connect(removeGroupPushButton, SIGNAL(clicked()), this, SLOT(removeGroup()));
	connect(purgeGroupPushButton, SIGNAL(clicked()), this, SLOT(purgeGroup()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));
	connect(modifyGroupPushButton, SIGNAL(clicked()), this, SLOT(modifyGroup()));

	connect(moveGroupUpPushButton, SIGNAL(clicked()), this, SLOT(moveGroupUp()));
	connect(moveGroupDownPushButton, SIGNAL(clicked()), this, SLOT(moveGroupDown()));

	connect(sortGroupsPushButton, SIGNAL(clicked()), this, SLOT(sortGroups()));
	connect(activateStudentsPushButton, SIGNAL(clicked()), this, SLOT(activateStudents()));
	connect(deactivateStudentsPushButton, SIGNAL(clicked()), this, SLOT(deactivateStudents()));
	connect(groupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyGroup()));

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
	else
		groupsListWidget->clear();
}


GroupsForm::~GroupsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void GroupsForm::addGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	AddStudentsGroupForm form(this, yearName);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearChanged(yearsListWidget->currentItem()->text());
	
	int i=groupsListWidget->count()-1;
	if(i>=0)
		groupsListWidget->setCurrentRow(i);
}

void GroupsForm::removeGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	QList<QString> yearsContainingGroup_List;
	//QSet<QString> yearsContainingGroup_Set;
	foreach(StudentsYear* year, gt.rules.yearsList)
		foreach(StudentsGroup* group, year->groupsList)
			if(group->name==groupName)
				yearsContainingGroup_List.append(year->name);
			
	assert(yearsContainingGroup_List.count()>=1);
	QString s;
	if(yearsContainingGroup_List.count()==1)
		s=tr("This group exists only in year %1. This means that"
		 " all the related activities and constraints will be removed. Do you want to continue?").arg(yearsListWidget->currentItem()->text());
	else{
		s=tr("This group exists in more places, listed below. It will only be removed from the current year,"
		 " and the related activities and constraints will not be removed. Do you want to continue?");
		s+="\n";
		foreach(QString str, yearsContainingGroup_List)
			s+=QString("\n")+str;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete group %1 and all related subgroups, activities and constraints?").arg(groupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.removeGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(tmp);
	if(tmp){
		int q=groupsListWidget->currentRow();
		
		groupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=groupsListWidget->takeItem(q);
		delete item;
		
		if(q>=groupsListWidget->count())
			q=groupsListWidget->count()-1;
		if(q>=0)
			groupsListWidget->setCurrentRow(q);
		else
			groupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(groupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This group still exists into another year. "
			"The related subgroups, activities and constraints were not removed"));*/
}

void GroupsForm::purgeGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	QList<QString> yearsContainingGroup_List;
	//QSet<QString> yearsContainingGroup_Set;
	foreach(StudentsYear* year, gt.rules.yearsList)
		foreach(StudentsGroup* group, year->groupsList)
			if(group->name==groupName)
				yearsContainingGroup_List.append(year->name);
			
	assert(yearsContainingGroup_List.count()>=1);
	QString s;
	if(yearsContainingGroup_List.count()==1)
		s=tr("This group exists only in year %1. All the related activities and constraints "
		 "will be removed. Do you want to continue?").arg(yearsListWidget->currentItem()->text());
	else{
		s=tr("This group exists in more places, listed below. It will be removed from all these places."
		 " All the related activities and constraints will be removed. Do you want to continue?");
		s+="\n";
		foreach(QString str, yearsContainingGroup_List)
			s+=QString("\n")+str;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete group %1 and all related subgroups, activities and constraints?").arg(groupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.purgeGroup(groupName);
	assert(tmp);
	if(tmp){
		int q=groupsListWidget->currentRow();
		
		groupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=groupsListWidget->takeItem(q);
		delete item;
		
		if(q>=groupsListWidget->count())
			q=groupsListWidget->count()-1;
		if(q>=0)
			groupsListWidget->setCurrentRow(q);
		else
			groupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(groupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This group still exists into another year. "
			"The related subgroups, activities and constraints were not removed"));*/
}

void GroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListWidget->clear();
		groupTextEdit->setPlainText(QString(""));
		return;
	}

	groupsListWidget->clear();

	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
	else
		groupTextEdit->setPlainText(QString(""));
}

void GroupsForm::groupChanged(const QString &groupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss==NULL){
		groupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsGroup* sg=(StudentsGroup*)ss;
	groupTextEdit->setPlainText(sg->getDetailedDescriptionWithConstraints(gt.rules));
}

void GroupsForm::moveGroupUp()
{
	if(groupsListWidget->count()<=1)
		return;
	int i=groupsListWidget->currentRow();
	if(i<0 || i>=groupsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=groupsListWidget->item(i)->text();
	QString s2=groupsListWidget->item(i-1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	StudentsGroup* sg1=sy->groupsList.at(i);
	StudentsGroup* sg2=sy->groupsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	groupsListWidget->item(i)->setText(s2);
	groupsListWidget->item(i-1)->setText(s1);
	
	sy->groupsList[i]=sg2;
	sy->groupsList[i-1]=sg1;
	
	groupsListWidget->setCurrentRow(i-1);
	groupChanged(/*i-1*/s1);
}

void GroupsForm::moveGroupDown()
{
	if(groupsListWidget->count()<=1)
		return;
	int i=groupsListWidget->currentRow();
	if(i<0 || i>=groupsListWidget->count())
		return;
	if(i==groupsListWidget->count()-1)
		return;
		
	QString s1=groupsListWidget->item(i)->text();
	QString s2=groupsListWidget->item(i+1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	StudentsGroup* sg1=sy->groupsList.at(i);
	StudentsGroup* sg2=sy->groupsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	groupsListWidget->item(i)->setText(s2);
	groupsListWidget->item(i+1)->setText(s1);
	
	sy->groupsList[i]=sg2;
	sy->groupsList[i+1]=sg1;
	
	groupsListWidget->setCurrentRow(i+1);
	groupChanged(/*i+1*/s1);
}

void GroupsForm::sortGroups()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);
	
	gt.rules.sortGroupsAlphabetically(yearsListWidget->currentItem()->text());

	yearChanged(yearsListWidget->currentItem()->text());
}

void GroupsForm::modifyGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);
	
	int q=groupsListWidget->currentRow();
	int valv=groupsListWidget->verticalScrollBar()->value();
	int valh=groupsListWidget->horizontalScrollBar()->value();

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	StudentsSet* sset=gt.rules.searchStudentsSet(groupName);
	assert(sset!=NULL);
	int numberOfStudents=sset->numberOfStudents;
	
	ModifyStudentsGroupForm form(this, yearName, groupName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearChanged(yearsListWidget->currentItem()->text());
	
	groupsListWidget->verticalScrollBar()->setValue(valv);
	groupsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(q>=groupsListWidget->count())
		q=groupsListWidget->count()-1;
	if(q>=0)
		groupsListWidget->setCurrentRow(q);
	else
		groupTextEdit->setPlainText(QString(""));
}

void GroupsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void GroupsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void GroupsForm::comments()
{
	int ind=groupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	
	QString groupName=groupsListWidget->currentItem()->text();
	
	StudentsSet* sset=gt.rules.searchStudentsSet(groupName);
	assert(sset!=NULL);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students group comments"));
	
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
	
	const QString settingsName=QString("StudentsGroupCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		sset->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		groupChanged(groupName);
	}
}
