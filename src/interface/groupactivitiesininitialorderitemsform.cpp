/***************************************************************************
                          groupactivitiesininitialorderitemsform.cpp  -  description
                             -------------------
    begin                : 2014
    copyright            : (C) 2014 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "groupactivitiesininitialorderitemsform.h"
#include "addgroupactivitiesininitialorderitemform.h"
#include "modifygroupactivitiesininitialorderitemform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <algorithm>
using namespace std;

GroupActivitiesInInitialOrderItemsForm::GroupActivitiesInInitialOrderItemsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentItemTextEdit->setReadOnly(true);
	
	modifyItemPushButton->setDefault(true);

	itemsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(itemsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(itemChanged(int)));
	connect(addItemPushButton, SIGNAL(clicked()), this, SLOT(addItem()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeItemPushButton, SIGNAL(clicked()), this, SLOT(removeItem()));
	connect(modifyItemPushButton, SIGNAL(clicked()), this, SLOT(modifyItem()));
	connect(itemsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyItem()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	
	connect(sortByCommentsPushButton, SIGNAL(clicked()), this, SLOT(sortItemsByComments()));
	connect(activatePushButton, SIGNAL(clicked()), this, SLOT(activateItem()));
	connect(deactivatePushButton, SIGNAL(clicked()), this, SLOT(deactivateItem()));
	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(itemComments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
/////////////
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

	studentsComboBox->addItem("");
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
	studentsComboBox->setCurrentIndex(0);
///////////////

	this->filterChanged();
}

GroupActivitiesInInitialOrderItemsForm::~GroupActivitiesInInitialOrderItemsForm()
{
	saveFETDialogGeometry(this);
}

bool GroupActivitiesInInitialOrderItemsForm::filterOk(const GroupActivitiesInInitialOrderItem& item)
{
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<item.ids.count(); i++){
		int id=item.ids.at(i);
		Activity* act=NULL;
		foreach(Activity* a, gt.rules.activitiesList)
			if(a->id==id)
				act=a;
		
		if(act!=NULL){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				foundSubject=true;
		
			//activity tag
			if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
				;
			else
				foundActivityTag=true;
		
			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
					if(*it == stn){
						ok2=true;
						break;
				}
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
	}
	
	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

void GroupActivitiesInInitialOrderItemsForm::filterChanged()
{
	this->visibleItemsList.clear();
	itemsListWidget->clear();
	int n_active=0;
	for(int i=0; i<gt.rules.groupActivitiesInInitialOrderList.count(); i++){
		GroupActivitiesInInitialOrderItem* item=gt.rules.groupActivitiesInInitialOrderList[i];
		if(filterOk(*item)){
			visibleItemsList.append(item);
			itemsListWidget->addItem(item->getDescription(gt.rules));

			if(USE_GUI_COLORS && !item->active)
				itemsListWidget->item(itemsListWidget->count()-1)->setBackground(itemsListWidget->palette().alternateBase());

			if(item->active)
				n_active++;
		}
	}
	
	if(itemsListWidget->count()>0)
		itemsListWidget->setCurrentRow(0);
	else
		currentItemTextEdit->setPlainText(QString(""));

	itemsTextLabel->setText(tr("%1 / %2 items",
	 "%1 represents the number of visible active 'group activities in initial order' items, %2 represents the total number of visible items")
	 .arg(n_active).arg(visibleItemsList.count()));
}

void GroupActivitiesInInitialOrderItemsForm::itemChanged(int index)
{
	if(index<0){
		currentItemTextEdit->setPlainText("");
	
		return;
	}
	assert(index<this->visibleItemsList.size());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(index);
	QString s=item->getDetailedDescription(gt.rules);
	currentItemTextEdit->setPlainText(s);
}

void GroupActivitiesInInitialOrderItemsForm::addItem()
{
	AddGroupActivitiesInInitialOrderItemForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	itemsListWidget->setCurrentRow(itemsListWidget->count()-1);
}

void GroupActivitiesInInitialOrderItemsForm::modifyItem()
{
	int valv=itemsListWidget->verticalScrollBar()->value();
	int valh=itemsListWidget->horizontalScrollBar()->value();

	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	GroupActivitiesInInitialOrderItem* item=visibleItemsList[i];

	ModifyGroupActivitiesInInitialOrderItemForm form(this, item);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	itemsListWidget->verticalScrollBar()->setValue(valv);
	itemsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(i>=itemsListWidget->count())
		i=itemsListWidget->count()-1;

	if(i>=0)
		itemsListWidget->setCurrentRow(i);
	else
		this->itemChanged(-1);
}

void GroupActivitiesInInitialOrderItemsForm::removeItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	QString s;
	s=tr("Remove item?");
	s+="\n\n";
	s+=item->getDetailedDescription(gt.rules);
	
	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		for(int j=0; j<gt.rules.groupActivitiesInInitialOrderList.count(); j++)
			if(visibleItemsList.at(i) == gt.rules.groupActivitiesInInitialOrderList[j]){
				gt.rules.groupActivitiesInInitialOrderList.removeAt(j);
				
				gt.rules.internalStructureComputed=false;
				setRulesModifiedAndOtherThings(&gt.rules);

				break;
			}

		visibleItemsList.removeAt(i);
		itemsListWidget->setCurrentRow(-1);
		itemsListWidget->takeItem(i);
		delete item;

		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}
	
	if(i>=itemsListWidget->count())
		i=itemsListWidget->count()-1;
	if(i>=0)
		itemsListWidget->setCurrentRow(i);
	else
		this->itemChanged(-1);
}

void GroupActivitiesInInitialOrderItemsForm::help()
{
	QString s=tr("This option is an advanced one. It is intended for experienced users."
	 " It is useful in cases of peculiar uses of constraints. In most cases, FET will care"
	 " automatically about a good initial order of the activities before the generation will"
	 " begin, but in some very rare cases you can help FET. An example is: if you use constraints activities"
	 " occupy max time slots from selection in such a way that some activities are clearly meant to be"
	 " near each other when the generation starts, but FET is not clever enough to deduce that."
	 " Other usage might be that you want to leave some activities at the end and, in case the timetable"
	 " cannot be found, you can use a partial one. In this case, you need to begin to generate once"
	 " (you can interrupt generation after only a few activities were placed), see the initial order of the"
	 " activities, and use the option to group the first activities with the ones you wish to"
	 " bring forward. The group of activities will be put near the earliest one in the group.");
	s+="\n\n";
	s+=tr("Note: Each activity id must appear at most once in all the active 'group activities in initial order' items.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void GroupActivitiesInInitialOrderItemsForm::activateItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	
	if(!item->active){
		item->active=true;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			itemsListWidget->currentItem()->setBackground(itemsListWidget->palette().base());
		itemChanged(itemsListWidget->currentRow());
	
		int n_active=0;
		foreach(GroupActivitiesInInitialOrderItem* item2, gt.rules.groupActivitiesInInitialOrderList)
			if(filterOk(*item2)){
				if(item2->active)
					n_active++;
			}
	
		itemsTextLabel->setText(tr("%1 / %2 items",
		 "%1 represents the number of visible active 'group activities in initial order' items, %2 represents the total number of visible items")
		 .arg(n_active).arg(visibleItemsList.count()));
	}
}

void GroupActivitiesInInitialOrderItemsForm::deactivateItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	
	if(item->active){
		item->active=false;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			itemsListWidget->currentItem()->setBackground(itemsListWidget->palette().alternateBase());
		itemChanged(itemsListWidget->currentRow());

		int n_active=0;
		foreach(GroupActivitiesInInitialOrderItem* item2, gt.rules.groupActivitiesInInitialOrderList)
			if(filterOk(*item2)){
				if(item2->active)
					n_active++;
			}
	
		itemsTextLabel->setText(tr("%1 / %2 items",
		 "%1 represents the number of visible active 'group activities in initial order' items, %2 represents the total number of visible items")
		 .arg(n_active).arg(visibleItemsList.count()));
	}
}

static int itemsAscendingByComments(const GroupActivitiesInInitialOrderItem* item1, const GroupActivitiesInInitialOrderItem* item2)
{
	return item1->comments < item2->comments;
}

void GroupActivitiesInInitialOrderItemsForm::sortItemsByComments()
{
	QMessageBox::StandardButton t=QMessageBox::question(this, tr("Sort items?"),
	 tr("This will sort the 'group activities in initial order' items list ascending according to their comments. You can obtain "
	 "a custom ordering by adding comments to some or all items, for example 'rank #1 ... other comments', "
	 "'rank #2 ... other different comments'.")
	 +" "+tr("Are you sure you want to continue?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	
	if(t==QMessageBox::Cancel)
		return;
	
	//qStableSort(gt.rules.groupActivitiesInInitialOrderList.begin(), gt.rules.groupActivitiesInInitialOrderList.end(), itemsAscendingByComments);
	std::stable_sort(gt.rules.groupActivitiesInInitialOrderList.begin(), gt.rules.groupActivitiesInInitialOrderList.end(), itemsAscendingByComments);

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	filterChanged();
}

void GroupActivitiesInInitialOrderItemsForm::itemComments()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Group activities in initial form item comments"));
	
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
	commentsPT->setPlainText(item->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("GroupActivitiesInInitialOrderItemCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		item->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		itemChanged(itemsListWidget->currentRow());
	}
}
