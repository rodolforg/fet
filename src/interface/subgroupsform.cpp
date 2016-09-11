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

#include "addstudentssubgroupform.h"
#include "modifystudentssubgroupform.h"
#include "subgroupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QSet>
#include <QList>
#include <QPair>

extern const QString COMPANY;
extern const QString PROGRAM;

SubgroupsForm::SubgroupsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	subgroupTextEdit->setReadOnly(true);

	modifySubgroupPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	subgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
	connect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));
	connect(addSubgroupPushButton, SIGNAL(clicked()), this, SLOT(addSubgroup()));
	connect(removeSubgroupPushButton, SIGNAL(clicked()), this, SLOT(removeSubgroup()));
	connect(purgeSubgroupPushButton, SIGNAL(clicked()), this, SLOT(purgeSubgroup()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));
	connect(modifySubgroupPushButton, SIGNAL(clicked()), this, SLOT(modifySubgroup()));

	connect(moveSubgroupUpPushButton, SIGNAL(clicked()), this, SLOT(moveSubgroupUp()));
	connect(moveSubgroupDownPushButton, SIGNAL(clicked()), this, SLOT(moveSubgroupDown()));

	connect(sortSubgroupsPushButton, SIGNAL(clicked()), this, SLOT(sortSubgroups()));
	connect(activateStudentsPushButton, SIGNAL(clicked()), this, SLOT(activateStudents()));
	connect(deactivateStudentsPushButton, SIGNAL(clicked()), this, SLOT(deactivateStudents()));
	connect(subgroupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifySubgroup()));

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
	else{
		groupsListWidget->clear();
		subgroupsListWidget->clear();
	}
}

SubgroupsForm::~SubgroupsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void SubgroupsForm::addSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	AddStudentsSubgroupForm form(this, yearName, groupName);
	setParentAndOtherThings(&form, this);
	form.exec();

	groupChanged(groupsListWidget->currentItem()->text());
	
	int i=subgroupsListWidget->count()-1;
	if(i>=0)
		subgroupsListWidget->setCurrentRow(i);
}

void SubgroupsForm::removeSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	QList<QPair<QString, QString> > yearsGroupsContainingSubgroup_List;
	//QSet<QPair<QString, QString> > yearsGroupsContainingSubgroup_Set;
	foreach(StudentsYear* year, gt.rules.yearsList)
		foreach(StudentsGroup* group, year->groupsList)
			foreach(StudentsSubgroup* subgroup, group->subgroupsList)
				if(subgroup->name==subgroupName)
					yearsGroupsContainingSubgroup_List.append(QPair<QString, QString>(year->name, group->name));
			
	assert(yearsGroupsContainingSubgroup_List.count()>=1);
	QString s;
	if(yearsGroupsContainingSubgroup_List.count()==1)
		s=tr("This subgroup exists only in year %1, group %2. This means that"
		 " all the related activities and constraints will be removed. Do you want to continue?").arg(yearName).arg(groupName);
	else{
		s=tr("This subgroup exists in more places, listed below. It will only be removed from the current year/group,"
		 " and the related activities and constraints will not be removed. Do you want to continue?");
		s+="\n";
		QPair<QString, QString> pair;
		foreach(pair, yearsGroupsContainingSubgroup_List)
			s+=QString("\n")+pair.first+QString(", ")+pair.second;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete subgroup %1 and all related activities and constraints?").arg(subgroupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.removeSubgroup(yearName, groupName, subgroupName);
	assert(tmp);
	if(tmp){
		int q=subgroupsListWidget->currentRow();
		
		subgroupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=subgroupsListWidget->takeItem(q);
		delete item;
		
		if(q>=subgroupsListWidget->count())
			q=subgroupsListWidget->count()-1;
		if(q>=0)
			subgroupsListWidget->setCurrentRow(q);
		else
			subgroupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(subgroupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This subgroup still exists into another group. "
		"The related activities and constraints were not removed"));*/
}

void SubgroupsForm::purgeSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	QList<QPair<QString, QString> > yearsGroupsContainingSubgroup_List;
	//QSet<QPair<QString, QString> > yearsGroupsContainingSubgroup_Set;
	foreach(StudentsYear* year, gt.rules.yearsList)
		foreach(StudentsGroup* group, year->groupsList)
			foreach(StudentsSubgroup* subgroup, group->subgroupsList)
				if(subgroup->name==subgroupName)
					yearsGroupsContainingSubgroup_List.append(QPair<QString, QString>(year->name, group->name));
			
	assert(yearsGroupsContainingSubgroup_List.count()>=1);
	QString s;
	if(yearsGroupsContainingSubgroup_List.count()==1)
		s=tr("This subgroup exists only in year %1, group %2. All the related activities and constraints "
		 "will be removed. Do you want to continue?").arg(yearName).arg(groupName);
	else{
		s=tr("This subgroup exists in more places, listed below. It will be removed from all these places."
		 " All the related activities and constraints will be removed. Do you want to continue?");
		s+="\n";
		QPair<QString, QString> pair;
		foreach(pair, yearsGroupsContainingSubgroup_List)
			s+=QString("\n")+pair.first+QString(", ")+pair.second;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete subgroup %1 and all related activities and constraints?").arg(subgroupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.purgeSubgroup(subgroupName);
	assert(tmp);
	if(tmp){
		int q=subgroupsListWidget->currentRow();
		
		subgroupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=subgroupsListWidget->takeItem(q);
		delete item;
		
		if(q>=subgroupsListWidget->count())
			q=subgroupsListWidget->count()-1;
		if(q>=0)
			subgroupsListWidget->setCurrentRow(q);
		else
			subgroupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(subgroupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This subgroup still exists into another group. "
		"The related activities and constraints were not removed"));*/
}

void SubgroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListWidget->clear();
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);

	groupsListWidget->clear();
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
	else{
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
	}
}

void SubgroupsForm::groupChanged(const QString &groupName)
{
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	if(groupIndex<0){
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}

	StudentsGroup* stg=sty->groupsList.at(groupIndex);

	subgroupsListWidget->clear();
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListWidget->addItem(sts->name);
	}

	if(subgroupsListWidget->count()>0)
		subgroupsListWidget->setCurrentRow(0);
	else
		subgroupTextEdit->setPlainText(QString(""));
}

void SubgroupsForm::subgroupChanged(const QString &subgroupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	if(ss==NULL){
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsSubgroup* s=(StudentsSubgroup*)ss;
	subgroupTextEdit->setPlainText(s->getDetailedDescriptionWithConstraints(gt.rules));
}

void SubgroupsForm::moveSubgroupUp()
{
	if(subgroupsListWidget->count()<=1)
		return;
	int i=subgroupsListWidget->currentRow();
	if(i<0 || i>=subgroupsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=subgroupsListWidget->item(i)->text();
	QString s2=subgroupsListWidget->item(i-1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	assert(groupsListWidget->currentRow()>=0);
	assert(groupsListWidget->currentRow()<sy->groupsList.count());
	StudentsGroup* sg=sy->groupsList.at(groupsListWidget->currentRow());
	
	StudentsSubgroup* ss1=sg->subgroupsList.at(i);
	StudentsSubgroup* ss2=sg->subgroupsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	subgroupsListWidget->item(i)->setText(s2);
	subgroupsListWidget->item(i-1)->setText(s1);
	
	sg->subgroupsList[i]=ss2;
	sg->subgroupsList[i-1]=ss1;
	
	subgroupsListWidget->setCurrentRow(i-1);
	subgroupChanged(/*i-1*/s1);
}

void SubgroupsForm::moveSubgroupDown()
{
	if(subgroupsListWidget->count()<=1)
		return;
	int i=subgroupsListWidget->currentRow();
	if(i<0 || i>=subgroupsListWidget->count())
		return;
	if(i==subgroupsListWidget->count()-1)
		return;
		
	QString s1=subgroupsListWidget->item(i)->text();
	QString s2=subgroupsListWidget->item(i+1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	assert(groupsListWidget->currentRow()>=0);
	assert(groupsListWidget->currentRow()<sy->groupsList.count());
	StudentsGroup* sg=sy->groupsList.at(groupsListWidget->currentRow());
	
	StudentsSubgroup* ss1=sg->subgroupsList.at(i);
	StudentsSubgroup* ss2=sg->subgroupsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	subgroupsListWidget->item(i)->setText(s2);
	subgroupsListWidget->item(i+1)->setText(s1);
	
	sg->subgroupsList[i]=ss2;
	sg->subgroupsList[i+1]=ss1;
	
	subgroupsListWidget->setCurrentRow(i+1);
	subgroupChanged(/*i+1*/s1);
}

void SubgroupsForm::sortSubgroups()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);
	
	gt.rules.sortSubgroupsAlphabetically(yearName, groupName);
	
	groupChanged(groupName);
}

void SubgroupsForm::modifySubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	int q=subgroupsListWidget->currentRow();
	int valv=subgroupsListWidget->verticalScrollBar()->value();
	int valh=subgroupsListWidget->horizontalScrollBar()->value();

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	StudentsSet* sset=gt.rules.searchStudentsSet(subgroupName);
	assert(sset!=NULL);
	int numberOfStudents=sset->numberOfStudents;
	
	ModifyStudentsSubgroupForm form(this, yearName, groupName, subgroupName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	groupChanged(groupName);
	
	subgroupsListWidget->verticalScrollBar()->setValue(valv);
	subgroupsListWidget->horizontalScrollBar()->setValue(valh);

	if(q>=subgroupsListWidget->count())
		q=subgroupsListWidget->count()-1;
	if(q>=0)
		subgroupsListWidget->setCurrentRow(q);
	else
		subgroupTextEdit->setPlainText(QString(""));
}

void SubgroupsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void SubgroupsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void SubgroupsForm::comments()
{
	int ind=subgroupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	
	StudentsSet* sset=gt.rules.searchStudentsSet(subgroupName);
	assert(sset!=NULL);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students subgroup comments"));
	
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
	
	const QString settingsName=QString("StudentsSubgroupCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		sset->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subgroupChanged(subgroupName);
	}
}
