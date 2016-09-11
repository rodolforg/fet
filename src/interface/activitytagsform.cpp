//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "activitytagsform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"

#include <QInputDialog>

#include <QMessageBox>

#include <QListWidget>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

ActivityTagsForm::ActivityTagsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentActivityTagTextEdit->setReadOnly(true);

	renameActivityTagPushButton->setDefault(true);

	activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(addActivityTagPushButton, SIGNAL(clicked()), this, SLOT(addActivityTag()));
	connect(removeActivityTagPushButton, SIGNAL(clicked()), this, SLOT(removeActivityTag()));
	connect(renameActivityTagPushButton, SIGNAL(clicked()), this, SLOT(renameActivityTag()));

	connect(moveActivityTagUpPushButton, SIGNAL(clicked()), this, SLOT(moveActivityTagUp()));
	connect(moveActivityTagDownPushButton, SIGNAL(clicked()), this, SLOT(moveActivityTagDown()));

	connect(sortActivityTagsPushButton, SIGNAL(clicked()), this, SLOT(sortActivityTags()));
	connect(activityTagsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activityTagChanged(int)));
	connect(activateActivityTagPushButton, SIGNAL(clicked()), this, SLOT(activateActivityTag()));
	connect(deactivateActivityTagPushButton, SIGNAL(clicked()), this, SLOT(deactivateActivityTag()));
	connect(activityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(renameActivityTag()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(comments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
		
	activityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sbt=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(sbt->name);
	}
		
	if(activityTagsListWidget->count()>0)
		activityTagsListWidget->setCurrentRow(0);
}


ActivityTagsForm::~ActivityTagsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void ActivityTagsForm::addActivityTag()
{
	bool ok = false;
	ActivityTag* sbt=new ActivityTag();
	sbt->name = QInputDialog::getText( this, tr("Add activity tag"), tr("Please enter activity tag's name") ,
	 QLineEdit::Normal, QString(), &ok );

	if ( ok && !((sbt->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addActivityTag(sbt)){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete sbt;
		}
		else{
			activityTagsListWidget->addItem(sbt->name);
			activityTagsListWidget->setCurrentRow(activityTagsListWidget->count()-1);
		}
	}
	else{
		if(ok){ //the user entered nothing
			QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		}
		delete sbt;// user entered nothing or pressed Cancel
	}
}

void ActivityTagsForm::removeActivityTag()
{
	int i=activityTagsListWidget->currentRow();
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int activity_tag_ID=gt.rules.searchActivityTag(text);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this activity tag?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeActivityTag(text);
	if(tmp){
		activityTagsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=activityTagsListWidget->takeItem(i);
		delete item;
		
		if(i>=activityTagsListWidget->count())
			i=activityTagsListWidget->count()-1;
		if(i>=0)
			activityTagsListWidget->setCurrentRow(i);
		else
			currentActivityTagTextEdit->setPlainText(QString(""));
	}
}

void ActivityTagsForm::renameActivityTag()
{
	int i=activityTagsListWidget->currentRow();
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	QString initialActivityTagName=activityTagsListWidget->currentItem()->text();

	int activity_tag_ID=gt.rules.searchActivityTag(initialActivityTagName);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	bool ok = false;
	QString finalActivityTagName;
	finalActivityTagName = QInputDialog::getText( this, tr("Rename activity tag"), tr("Please enter new activity tag's name") ,
	 QLineEdit::Normal, initialActivityTagName, &ok );

	if ( ok && !(finalActivityTagName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchActivityTag(finalActivityTagName)>=0){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifyActivityTag(initialActivityTagName, finalActivityTagName);
			activityTagsListWidget->item(i)->setText(finalActivityTagName);
			activityTagChanged(activityTagsListWidget->currentRow());
		}
	}
}

void ActivityTagsForm::moveActivityTagUp()
{
	if(activityTagsListWidget->count()<=1)
		return;
	int i=activityTagsListWidget->currentRow();
	if(i<0 || i>=activityTagsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=activityTagsListWidget->item(i)->text();
	QString s2=activityTagsListWidget->item(i-1)->text();
	
	ActivityTag* at1=gt.rules.activityTagsList.at(i);
	ActivityTag* at2=gt.rules.activityTagsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	activityTagsListWidget->item(i)->setText(s2);
	activityTagsListWidget->item(i-1)->setText(s1);
	
	gt.rules.activityTagsList[i]=at2;
	gt.rules.activityTagsList[i-1]=at1;
	
	activityTagsListWidget->setCurrentRow(i-1);
	activityTagChanged(i-1);
}

void ActivityTagsForm::moveActivityTagDown()
{
	if(activityTagsListWidget->count()<=1)
		return;
	int i=activityTagsListWidget->currentRow();
	if(i<0 || i>=activityTagsListWidget->count())
		return;
	if(i==activityTagsListWidget->count()-1)
		return;
		
	QString s1=activityTagsListWidget->item(i)->text();
	QString s2=activityTagsListWidget->item(i+1)->text();
	
	ActivityTag* at1=gt.rules.activityTagsList.at(i);
	ActivityTag* at2=gt.rules.activityTagsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	activityTagsListWidget->item(i)->setText(s2);
	activityTagsListWidget->item(i+1)->setText(s1);
	
	gt.rules.activityTagsList[i]=at2;
	gt.rules.activityTagsList[i+1]=at1;
	
	activityTagsListWidget->setCurrentRow(i+1);
	activityTagChanged(i+1);
}

void ActivityTagsForm::sortActivityTags()
{
	gt.rules.sortActivityTagsAlphabetically();

	activityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sbt=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(sbt->name);
	}

	if(activityTagsListWidget->count()>0)
		activityTagsListWidget->setCurrentRow(0);
}

void ActivityTagsForm::activityTagChanged(int index)
{
	if(index<0){
		currentActivityTagTextEdit->setPlainText(QString(""));
		return;
	}
	
	ActivityTag* st=gt.rules.activityTagsList.at(index);
	assert(st);
	QString s=st->getDetailedDescriptionWithConstraints(gt.rules);
	currentActivityTagTextEdit->setPlainText(s);
}

void ActivityTagsForm::activateActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int count=gt.rules.activateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void ActivityTagsForm::deactivateActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int count=gt.rules.deactivateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void ActivityTagsForm::help()
{
	QMessageBox::information(this, tr("FET help on activity tags"), 
	 tr("Activity tag is a field which can be used or not, depending on your wish (optional field)."
	 " It is designed to help you with some constraints. Each activity has a possible empty list of activity tags"
	 " (if you don't use activity tags, the list will be empty)"));
}

void ActivityTagsForm::comments()
{
	int ind=activityTagsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	ActivityTag* at=gt.rules.activityTagsList[ind];
	assert(at!=NULL);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Activity tag comments"));
	
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
	commentsPT->setPlainText(at->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("ActivityTagCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		at->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		activityTagChanged(ind);
	}
}

