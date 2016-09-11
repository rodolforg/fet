/***************************************************************************
                          teachersubjectsqualificationsform.cpp  -  description
                             -------------------
    begin                : 2016
    copyright            : (C) 2016 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "teachersubjectsqualificationsform.h"
#include "spaceconstraint.h"
#include "teacher.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QSet>
#include <QLinkedList>

TeacherSubjectsQualificationsForm::TeacherSubjectsQualificationsForm(QWidget* parent, Teacher* teacher): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	subjectsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedSubjectsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(subjectsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSubject()));
	connect(selectedSubjectsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeSubject()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(addAllSubjectsPushButton, SIGNAL(clicked()), this, SLOT(addAllSubjects()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSet<QString> allSubjects;
	foreach(Subject* subject, gt.rules.subjectsList){
		allSubjects.insert(subject->name);
		subjectsListWidget->addItem(subject->name);
	}
	
	foreach(QString subject, teacher->qualifiedSubjectsList){
		assert(allSubjects.contains(subject));
		selectedSubjectsListWidget->addItem(subject);
	}
	
	teacherTextLabel->setText(tr("Teacher: %1").arg(teacher->name));
	
	_teacher=teacher;
}

TeacherSubjectsQualificationsForm::~TeacherSubjectsQualificationsForm()
{
	saveFETDialogGeometry(this);
}

void TeacherSubjectsQualificationsForm::ok()
{
	QLinkedList<QString> newSubjectsList;
	QHash<QString, QLinkedList<QString>::Iterator> newSubjectsHash;
	for(int i=0; i<selectedSubjectsListWidget->count(); i++){
		newSubjectsList.append(selectedSubjectsListWidget->item(i)->text());
		newSubjectsHash.insert(selectedSubjectsListWidget->item(i)->text(), newSubjectsList.end()-1);
	}
	
	_teacher->qualifiedSubjectsList=newSubjectsList;
	_teacher->qualifiedSubjectsHash=newSubjectsHash;
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void TeacherSubjectsQualificationsForm::addSubject()
{
	if(subjectsListWidget->currentRow()<0)
		return;
	QString sbName=subjectsListWidget->currentItem()->text();
	assert(sbName!="");
	int i;
	//duplicate?
	for(i=0; i<selectedSubjectsListWidget->count(); i++)
		if(sbName==selectedSubjectsListWidget->item(i)->text())
			break;
	if(i<selectedSubjectsListWidget->count())
		return;
	selectedSubjectsListWidget->addItem(sbName);
	selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}

void TeacherSubjectsQualificationsForm::removeSubject()
{
	if(selectedSubjectsListWidget->currentRow()<0 || selectedSubjectsListWidget->count()<=0)
		return;
	int tmp=selectedSubjectsListWidget->currentRow();
	
	selectedSubjectsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedSubjectsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedSubjectsListWidget->count())
		selectedSubjectsListWidget->setCurrentRow(tmp);
	else
		selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}

void TeacherSubjectsQualificationsForm::clear()
{
	selectedSubjectsListWidget->clear();
}

void TeacherSubjectsQualificationsForm::addAllSubjects()
{
	selectedSubjectsListWidget->clear();
	
	for(int i=0; i<subjectsListWidget->count(); i++)
		selectedSubjectsListWidget->addItem(subjectsListWidget->item(i)->text());
	
	if(selectedSubjectsListWidget->count()>0)
		selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}
