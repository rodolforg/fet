/***************************************************************************
                          splityearform.cpp  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include <QtGlobal>

#include "splityearform.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QMessageBox>

#include <QSettings>

#include <QListWidget>
#include <QAbstractItemView>

#include <QInputDialog>

#include <QSet>
#include <QHash>
#include <QMap>

#include <QSignalMapper>

#include "centerwidgetonscreen.h"

#include "longtextmessagebox.h"

extern const QString COMPANY;
extern const QString PROGRAM;

SplitYearForm::SplitYearForm(QWidget* parent, const QString& _year): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabIndexChanged(int)));

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(categoriesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfCategoriesChanged()));
	
	listWidgets[0]=listWidget1;
	listWidgets[1]=listWidget2;
	listWidgets[2]=listWidget3;
	listWidgets[3]=listWidget4;
	listWidgets[4]=listWidget5;
	listWidgets[5]=listWidget6;
	listWidgets[6]=listWidget7;
	listWidgets[7]=listWidget8;
	listWidgets[8]=listWidget9;
	listWidgets[9]=listWidget10;
	listWidgets[10]=listWidget11;
	listWidgets[11]=listWidget12;
	listWidgets[12]=listWidget13;
	listWidgets[13]=listWidget14;
	
	for(int i=0; i<MAX_CATEGORIES; i++){
		listWidgets[i]->clear();
		listWidgets[i]->setSelectionMode(QAbstractItemView::SingleSelection);

		mapperModify.connect(listWidgets[i], SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(map()));
		mapperModify.setMapping(listWidgets[i], i);
	}
	
	connect(&mapperModify, SIGNAL(mapped(int)), SLOT(modifyDoubleClicked(int)));
	
	connect(addPushButton,  SIGNAL(clicked()), this, SLOT(addClicked()));
	connect(modifyPushButton,  SIGNAL(clicked()), this, SLOT(modifyClicked()));
	connect(removePushButton,  SIGNAL(clicked()), this, SLOT(removeClicked()));
	connect(removeAllPushButton,  SIGNAL(clicked()), this, SLOT(removeAllClicked()));
	
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(resetPushButton, SIGNAL(clicked()), this, SLOT(reset()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
	
	_sep=settings.value(this->metaObject()->className()+QString("/separator-string"), QString(" ")).toString();
	
	_nCategories=settings.value(this->metaObject()->className()+QString("/number-of-categories"), 1).toInt();
	
	for(int i=0; i<_nCategories; i++){
		_nDivisions[i]=settings.value(this->metaObject()->className()+QString("/category/%1/number-of-divisions").arg(i+1), 0).toInt();
		for(int j=0; j<_nDivisions[i]; j++){
			QString ts=settings.value(this->metaObject()->className()+QString("/category/%1/division/%2").arg(i+1).arg(j+1), QString("")).toString();
			if(!ts.isEmpty())
				_divisions[i].append(ts);
			else
				_nDivisions[i]--;
		}
		if(_nDivisions[i]!=_divisions[i].count()){
			QMessageBox::warning(this, tr("FET warning"), tr("You have met a minor bug in FET, please report it. FET expected to read"
			 " from settings %1 divisions in category %2, but read %3. FET will now continue operation, nothing will be lost.")
			 .arg(_nDivisions[i]).arg(i).arg(_divisions[i].count()));
			_nDivisions[i]=_divisions[i].count();
		}
	}

	year=_year;

	QString s=tr("Splitting year: %1").arg(year);
	splitYearTextLabel->setText(s);
	
	//restore saved values
	separatorLineEdit->setText(_sep);
	categoriesSpinBox->setValue(_nCategories);
	for(int i=0; i<_nCategories; i++)
		for(int j=0; j<_nDivisions[i]; j++)
			listWidgets[i]->addItem(_divisions[i].at(j));
	
	maxSubgroupsPerYearLabel->setText(tr("Max subgroups per year: %1").arg(MAX_TOTAL_SUBGROUPS));
	maxTotalSubgroupsLabel->setText(tr("Max total subgroups: %1").arg(MAX_TOTAL_SUBGROUPS));
	
	numberOfCategoriesChanged();
	
	tabIndexChanged(0);
}

SplitYearForm::~SplitYearForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);
	
	settings.setValue(this->metaObject()->className()+QString("/separator-string"), _sep);
	
	settings.setValue(this->metaObject()->className()+QString("/number-of-categories"), _nCategories);

	settings.remove(this->metaObject()->className()+QString("/category"));

	for(int i=0; i<_nCategories; i++){
		settings.setValue(this->metaObject()->className()+QString("/category/%1/number-of-divisions").arg(i+1), _nDivisions[i]);
		assert(_nDivisions[i]==_divisions[i].count());
		for(int j=0; j<_nDivisions[i]; j++)
			settings.setValue(this->metaObject()->className()+QString("/category/%1/division/%2").arg(i+1).arg(j+1), _divisions[i].at(j));
	}
}

void SplitYearForm::tabIndexChanged(int i)
{
	if(i>=0){
		assert(i<MAX_CATEGORIES);
		if(listWidgets[i]->count()>0)
			listWidgets[i]->setCurrentRow(0);
	}
}

void SplitYearForm::numberOfCategoriesChanged()
{
	for(int i=0; i<MAX_CATEGORIES; i++)
		if(i<categoriesSpinBox->value())
			tabWidget->setTabEnabled(i, true);
		else
			tabWidget->setTabEnabled(i, false);
			
	updateNumberOfSubgroups();
	updateDivisionsLabel();
}

void SplitYearForm::addClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	bool ok;
	QString text=QInputDialog::getText(this, tr("FET - Add division to category %1").arg(i+1),
	 tr("Please input division name:"), QLineEdit::Normal, QString(""), &ok);
	if(ok && !text.isEmpty()){
		for(int k=0; k<categoriesSpinBox->value(); k++)
			for(int j=0; j<listWidgets[k]->count(); j++)
				if(listWidgets[k]->item(j)->text()==text){
					QMessageBox::information(this, tr("FET information"), tr("Duplicates not allowed!"));
					return;
				}
	
		listWidgets[i]->addItem(text);
		listWidgets[i]->setCurrentRow(listWidgets[i]->count()-1);

		updateNumberOfSubgroups();
		updateDivisionsLabel();
	}
}

void SplitYearForm::modifyClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	modifyDoubleClicked(i);
}

void SplitYearForm::modifyDoubleClicked(int i)
{
	if(listWidgets[i]->currentRow()>=0 && listWidgets[i]->currentRow()<listWidgets[i]->count()){
		QString ts=listWidgets[i]->currentItem()->text();

		bool ok;
		QString text=QInputDialog::getText(this, tr("FET - Modify division to category %1").arg(i+1),
		 tr("Please input the new division name:"), QLineEdit::Normal, ts, &ok);
		if(ok && !text.isEmpty()){
			if(text!=ts){
				for(int k=0; k<categoriesSpinBox->value(); k++)
					for(int j=0; j<listWidgets[k]->count(); j++)
						if(listWidgets[k]->item(j)->text()==text){
							QMessageBox::information(this, tr("FET information"), tr("Duplicates not allowed!"));
							return;
						}
						
				listWidgets[i]->currentItem()->setText(text);
			}
		}
	}
}

void SplitYearForm::removeClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	if(listWidgets[i]->currentRow()>=0 && listWidgets[i]->currentRow()<listWidgets[i]->count()){
		QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
		 tr("Do you want to remove division %1 from category %2?").arg(listWidgets[i]->currentItem()->text())
		 .arg(i+1), QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	
		int j=listWidgets[i]->currentRow();
		listWidgets[i]->setCurrentRow(-1);
		QListWidgetItem* item;
		item=listWidgets[i]->takeItem(j);
		delete item;
		
		if(j>=listWidgets[i]->count())
			j=listWidgets[i]->count()-1;
		if(j>=0)
			listWidgets[i]->setCurrentRow(j);

		updateNumberOfSubgroups();
		updateDivisionsLabel();
	}
}

void SplitYearForm::removeAllClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
	 tr("Do you really want to remove all divisions from category %1?").arg(i+1),
	 QMessageBox::Yes|QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
	
	listWidgets[i]->clear();

	updateNumberOfSubgroups();
	updateDivisionsLabel();
}

void SplitYearForm::ok()
{
	if(categoriesSpinBox->value()>4){
		QMessageBox::StandardButton ret=QMessageBox::warning(this, tr("FET warning"),
		 tr("You want to divide the year by %1 categories. The recommended number of categories"
		 " is 2, 3 or maximum 4 (to ensure the timetable generation speed and feasibility). Are you sure?")
		 .arg(categoriesSpinBox->value()),
		 QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	}

	qint64 product=1;
	
	for(int i=0; i<categoriesSpinBox->value(); i++){
		product*=listWidgets[i]->count();

		if(product>MAX_SUBGROUPS_PER_YEAR){
			QMessageBox::information(this, tr("FET information"), tr("The current number of subgroups for this year is too large"
			 " (the maximum allowed value is %1, but computing up to category %2 gives %3 subgroups)")
			 .arg(MAX_SUBGROUPS_PER_YEAR).arg(i+1).arg(product));
			return;
		}
	}
	
	if(product==0){
		QMessageBox::information(this, tr("FET information"), tr("Each category must contain at least one division"));
		return;
	}
	
	//warn too many total subgroups - suggested by Volker Dirr
	QSet<QString> tmpSet;
	foreach(StudentsYear* sty, gt.rules.yearsList){
		if(sty->name!=year){
			if(sty->groupsList.count()==0){
				tmpSet.insert(sty->name);
			}
			else{
				foreach(StudentsGroup* stg, sty->groupsList){
					if(stg->subgroupsList.count()==0){
						tmpSet.insert(stg->name);
					}
					else{
						foreach(StudentsSubgroup* sts, stg->subgroupsList)
							tmpSet.insert(sts->name);
					}
				}
			}
		}
	}
	int totalEstimated=tmpSet.count()+int(product);
	if(totalEstimated>MAX_TOTAL_SUBGROUPS){
		QMessageBox::StandardButton ret=QMessageBox::warning(this, tr("FET warning"),
		 tr("Please note that the current configuration will lead you to %1 total number of subgroups."
		 " The file format supports any number of students sets, but for the timetable generation to be"
		 " possible the maximum allowed total number of subgroups is %2.").arg(totalEstimated).arg(MAX_TOTAL_SUBGROUPS)
		 +QString("\n\n")+tr("Are you sure you want to continue?"),
		 QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	}
	
	QString separator=separatorLineEdit->text();
	
/*	StudentsYear* y=NULL;
	foreach(StudentsYear* ty, gt.rules.yearsList)
		if(ty->name==year){
			y=ty;
			break;
		}
	assert(y!=NULL);
	
	if(y->groupsList.count()>0){
		int t=QMessageBox::question(this, tr("FET question"), tr("Year %1 is not empty and it will be emptied before adding"
		" the divisions you selected. This means that all the activities and constraints for"
		" the groups and subgroups in this year will be removed. It is strongly recommended to save your file before continuing."
		" You might also want, as an alternative, to modify manually the groups/subgroups from the corresponding menu, so that"
		" you will not lose constraints and activities referring to them."
		" Do you really want to empty year?").arg(year),
		 QMessageBox::Yes, QMessageBox::Cancel);
		 
		if(t==QMessageBox::Cancel)
			return;

		t=QMessageBox::warning(this, tr("FET warning"), tr("Year %1 will be emptied."
		 " This means that all constraints and activities referring to groups/subgroups in year %1 will be removed."
		 " Are you absolutely sure?").arg(year),
		 QMessageBox::Yes, QMessageBox::Cancel);
		 
		if(t==QMessageBox::Cancel)
			return;
			
		gt.rules.emptyYear(year);
	}*/
	
	QSet<QString> tmp;
	for(int i=0; i<categoriesSpinBox->value(); i++)
		for(int j=0; j<listWidgets[i]->count(); j++){
			QString ts=listWidgets[i]->item(j)->text();
			if(tmp.contains(ts)){
				QMessageBox::information(this, tr("FET information"), tr("Duplicate names not allowed"));
				return;
			}
			else if(ts.isEmpty()){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			tmp.insert(ts);
		}
		
	QSet<QString> existingNames;
	foreach(StudentsYear* sty, gt.rules.yearsList){
		assert(!existingNames.contains(sty->name));
		existingNames.insert(sty->name);
		if(sty->name!=year){
			foreach(StudentsGroup* group, sty->groupsList){
				if(!existingNames.contains(group->name))
					existingNames.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(!existingNames.contains(subgroup->name))
						existingNames.insert(subgroup->name);
				}
			}
		}
	}

	QSet<QString> newStudentsSets;
	for(int i=0; i<categoriesSpinBox->value(); i++)
		for(int j=0; j<listWidgets[i]->count(); j++){
			QString ts=year+separator+listWidgets[i]->item(j)->text();
			if(existingNames.contains(ts)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with the same name exists."
				 " Please choose another name or remove the old set").arg(ts));
				return;
			}
			newStudentsSets.insert(ts);
		}
		
	//As in Knuth TAOCP vol 4A, generate all tuples
	int b[MAX_CATEGORIES];
	int ii;
	
	if(categoriesSpinBox->value()>=2){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QString sb=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sb+=separator+listWidgets[i]->item(b[i])->text();
			if(existingNames.contains(sb)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add subgroup %1, because a set with the same name exists. "
				 "Please choose another name or remove the old set").arg(sb));
				return;
			}
			newStudentsSets.insert(sb);
			ii=categoriesSpinBox->value()-1;
again_here_1:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_1;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}
	
	QHash<QString, StudentsGroup*> groupsHash;
	
	StudentsYear* yearPointer=NULL;
	int yearIndex=-1;
	for(int i=0; i<gt.rules.yearsList.count(); i++)
		if(gt.rules.yearsList[i]->name==year){
			yearPointer=gt.rules.yearsList[i];
			yearIndex=i;
			break;
		}
	assert(yearPointer!=NULL);
	assert(yearIndex>=0);
	
	QSet<QString> notExistingGroupsSet;
	QSet<QString> notExistingSubgroupsSet;
	QStringList notExistingGroupsList;
	QStringList notExistingSubgroupsList;
	foreach(StudentsGroup* group, yearPointer->groupsList){
		if(!existingNames.contains(group->name) && !newStudentsSets.contains(group->name) && !notExistingGroupsSet.contains(group->name)){
			notExistingGroupsSet.insert(group->name);
			notExistingGroupsList.append(group->name);
		}
		foreach(StudentsSubgroup* subgroup, group->subgroupsList){
			if(!existingNames.contains(subgroup->name) && !newStudentsSets.contains(subgroup->name) && !notExistingSubgroupsSet.contains(subgroup->name)){
				notExistingSubgroupsSet.insert(subgroup->name);
				notExistingSubgroupsList.append(subgroup->name);
			}
		}
	}
	
	bool removeGroupsOrSubgroups = notExistingGroupsList.count()>0 || notExistingSubgroupsList.count()>0;
	
	QString description=QString("");
	if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()>0)
		description+=tr("WARNING: There are groups and subgroups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");
	else if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()==0)
		description+=tr("WARNING: There are groups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");
	else if(notExistingGroupsList.count()==0 && notExistingSubgroupsList.count()>0)
		description+=tr("WARNING: There are subgroups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");

	if(notExistingGroupsList.count()>0 || notExistingSubgroupsList.count()>0){
		description+=" (";
		if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()>0)
			description+=tr("Notes:");
		else
			description+=tr("Note:");
		description+=" ";
		if(notExistingGroupsList.count()>0){
			description+=tr("To keep a group, you need to keep the corresponding division name and use the same separator(s) character(s).");
			description+=" ";
		}
		if(notExistingSubgroupsList.count()>0){
			description+=tr("Probably you can safely ignore the warning about the removal of the subgroups.");
			description+=" ";
		}
		description+=tr("Read the divide year dialog Help for details.");
		description+=")";
	}
	
	if(!description.isEmpty())
		description+="\n\n";
	
	if(notExistingGroupsList.count()>0){
		description+=tr("The following groups will no longer be available:");
		description+="\n\n";
		description+=notExistingGroupsList.join("\n");
	}
	if(notExistingSubgroupsList.count()>0){
		if(notExistingGroupsList.count()>0)
			description+="\n\n";
	
		description+=tr("The following subgroups will no longer be available:");
		description+="\n\n";
		description+=notExistingSubgroupsList.join("\n");
	}
	
	if(!description.isEmpty()){
		int lres=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
		 description, tr("Yes"), tr("No"), 0, 0, 1);
		if(lres!=0){
			return;
		}
		
		QMessageBox::StandardButton t=QMessageBox::warning(this, tr("FET warning"), tr("Year %1 will be split again."
		 " All groups and subgroups of this year which will no longer exist (listed before) and the associated activities and constraints"
		 " will be removed. Are you absolutely sure?").arg(year), QMessageBox::Yes|QMessageBox::Cancel);
		
		if(t==QMessageBox::Cancel)
			return;
	}
	
	StudentsYear* newYear=new StudentsYear;
	newYear->name=yearPointer->name;
	newYear->numberOfStudents=yearPointer->numberOfStudents;
	newYear->indexInAugmentedYearsList=yearPointer->indexInAugmentedYearsList;
	
	QHash<QString, int> numberOfStudents;
	foreach(StudentsGroup* group, yearPointer->groupsList){
		numberOfStudents.insert(group->name, group->numberOfStudents);
		foreach(StudentsSubgroup* subgroup, group->subgroupsList)
			numberOfStudents.insert(subgroup->name, subgroup->numberOfStudents);
	}
	
	//add groups and subgroups
	for(int i=0; i<categoriesSpinBox->value(); i++)
		for(int j=0; j<listWidgets[i]->count(); j++){
			QString ts=year+separator+listWidgets[i]->item(j)->text();
			StudentsGroup* gr=new StudentsGroup;
			gr->name=ts;
			if(numberOfStudents.contains(gr->name))
				gr->numberOfStudents=numberOfStudents.value(gr->name);
			bool t=gt.rules.addGroupFast(newYear, gr);
			
			assert(t);
			
			assert(!groupsHash.contains(gr->name));
			groupsHash.insert(gr->name, gr);
		}
	
	if(categoriesSpinBox->value()>=2){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QStringList groups;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				groups.append(year+separator+listWidgets[i]->item(b[i])->text());
	
			QString sbn=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sbn+=separator+listWidgets[i]->item(b[i])->text();
				
			StudentsSubgroup* sb=new StudentsSubgroup;
			sb->name=sbn;
			if(numberOfStudents.contains(sb->name))
				sb->numberOfStudents=numberOfStudents.value(sb->name);

			for(int i=0; i<categoriesSpinBox->value(); i++){
				assert(groupsHash.contains(groups.at(i)));
				bool t=gt.rules.addSubgroupFast(newYear, groupsHash.value(groups.at(i)), sb);
				assert(t);
			}

			ii=categoriesSpinBox->value()-1;
again_here_2:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_2;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}
	
	assert(yearIndex>=0 && yearIndex<gt.rules.yearsList.count());
	assert(gt.rules.yearsList[yearIndex]==yearPointer);
	gt.rules.yearsList[yearIndex]=newYear;
	
	QString s=QString("");
	
	int nActivitiesBefore=gt.rules.activitiesList.count();
	int nTimeConstraintsBefore=gt.rules.timeConstraintsList.count();
	int nSpaceConstraintsBefore=gt.rules.spaceConstraintsList.count();
	int nGroupActivitiesInInitialOrderItemsBefore=gt.rules.groupActivitiesInInitialOrderList.count();
	
	gt.rules.computePermanentStudentsHash();
	gt.rules.removeYearPointerAfterSplit(yearPointer);

	int nActivitiesAfter=gt.rules.activitiesList.count();
	int nTimeConstraintsAfter=gt.rules.timeConstraintsList.count();
	int nSpaceConstraintsAfter=gt.rules.spaceConstraintsList.count();
	int nGroupActivitiesInInitialOrderItemsAfter=gt.rules.groupActivitiesInInitialOrderList.count();
	
	if(removeGroupsOrSubgroups){
		s+="\n\n";
		s+=tr("There were removed %1 activities, %2 time constraints and %3 space constraints.")
		 .arg(nActivitiesBefore-nActivitiesAfter)
		 .arg(nTimeConstraintsBefore-nTimeConstraintsAfter)
		 .arg(nSpaceConstraintsBefore-nSpaceConstraintsAfter);
	
		if(nGroupActivitiesInInitialOrderItemsBefore!=nGroupActivitiesInInitialOrderItemsAfter)
			s+=QString(" ")+tr("There were removed %1 'group activities in the initial order' items.")
			 .arg(nGroupActivitiesInInitialOrderItemsBefore-nGroupActivitiesInInitialOrderItemsAfter);
	}
	else{
		assert(nActivitiesBefore==nActivitiesAfter);
		assert(nTimeConstraintsBefore==nTimeConstraintsAfter);
		assert(nSpaceConstraintsBefore==nSpaceConstraintsAfter);
		assert(nGroupActivitiesInInitialOrderItemsBefore==nGroupActivitiesInInitialOrderItemsAfter);
	}
	
	QMessageBox::information(this, tr("FET information"), tr("Split of the year complete, please check the groups and subgroups"
	 " of the year to make sure that everything is OK.")+s);
	
	//saving page
	_sep=separatorLineEdit->text();
	
	_nCategories=categoriesSpinBox->value();
	
	for(int i=0; i<_nCategories; i++){
		_nDivisions[i]=listWidgets[i]->count();
		
		_divisions[i].clear();
		for(int j=0; j<listWidgets[i]->count(); j++)
			_divisions[i].append(listWidgets[i]->item(j)->text());
	}
	
	this->close();
}

void SplitYearForm::help()
{
	QString s;

	s+=tr("You might first want to consider if dividing a year is necessary and on what options. Please remember"
	 " that FET can handle activities with multiple teachers/students sets. If you have say students set 9a, which is split"
	 " into 2 parts: English (teacher TE) and French (teacher TF), and language activities must be simultaneous, then you might not want to divide"
	 " according to this category, but add more larger activities, with students set 9a and teachers TE+TF."
	 " The only drawback is that each activity can take place only in one room in FET, so you might need to find a way to overcome that.");
	
	s+="\n\n";
	
	s+=tr("Please choose a number of categories and in each category the number of divisions. You can choose for instance"
	 " 3 categories, 5 divisions for the first category: a, b, c, d and e, 2 divisions for the second category: boys and girls,"
	 " and 3 divisions for the third: English, German and French.");

	s+="\n\n";

	/*s+=tr("Please input from the beginning the correct divisions. After you inputted activities and constraints"
	 " for this year's groups and subgroups, dividing it again will remove the activities and constraints referring"
	 " to these groups/subgroups. I know this is not elegant, I hope I'll solve that in the future."
	 " You might want to use the alternative of manually adding/editing/removing groups/subgroups"
	 " in the groups/subgroups menu, though removing a group/subgroup will also remove the activities");
	
	s+="\n\n";*/

	s+=tr("If your number of subgroups is reasonable, probably you need not worry about empty subgroups (regarding speed of generation)."
		" But more tests need to be done. You just need to know that for the moment the maximum total number of subgroups is %1 (which can be changed,"
		" but nobody needed larger values)").arg(MAX_TOTAL_SUBGROUPS);

	s+="\n\n";

	s+=tr("Please note that the dialog here will keep the last configuration of the last"
		" divided year, it will not remember the values for a specific year you need to modify.");
	s+=" ";
	s+=tr("If you intend to divide again a year by categories and you want to keep (the majority of) the existing groups in this year,"
		" you will need to use the exact same separator character(s) for dividing this year as you used when previously dividing this year,"
		" and the same division names (any old division which is no longer entered means a group which will be removed from this year).");
		
	s+="\n\n";
	
	s+=tr("When dividing again a year, you might get a warning about subgroups which will be removed. If you didn't explicitly use"
		" (these) subgroups in your activities or constraints, probably you can safely ignore this warning. Generally, if you use years' division"
		" by categories, it is groups that matter.");
	
	s+="\n\n";

	s+=tr("Separator character(s) is of your choice (default is space)");
	
	s+="\n\n";
	
	s+=tr("If you have many subgroups and you don't explicitly use them, it is recommended to use the three global settings: hide subgroups"
		" in combo boxes, hide subgroups in activity planning, and do not write subgroups timetables on hard disk.");
	s+="\n";
	s+=tr("Note that if you are only working to get a feasible timetable, without the need to obtain the students timetable (XML or HTML) on"
		" the disk at all, and if you have many total subgroups, a good idea is to disable writing the subgroups, groups AND years timetables"
		" to the hard disk, as these take a long time to compute (not only subgroups, but also groups and years!).");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is big.)");
	s+=" ";
	s+=tr("After that, you can re-enable writing the students timetables on the disk, and re-generate.");
	
	s+="\n\n";
	s+=tr("About using a large number of categories, divisions per category and subgroups: it is highly recommended to"
		" keep these to a minimum, especially the number of categories, by using any kind of tricks. Otherwise the timetable"
		" might become impossible (taking too much time to generate).");
	s+=" ";
	s+=tr("Maybe a reasonable number of categories could be 2, 3 or maximum 4. The divide year dialog allows much higher values, but"
		" these are not at all recommended.");
	s+="\n";
	s+=tr("Maybe an alternative to dividing a year into many categories/subgroups would be to enter individual students as FET subgroups and add into"
		" each group the corresponding subgroups. But this is hard to do from the FET interface - maybe a solution would be to use an automatic"
		" tool to convert your institution data into a file in .fet format.");
	s+=" ";
	s+=tr("Or you might use the FET feature to import students sets from comma separated values (CSV) files.");
	s+=" ";
	s+=tr("In such cases (individual students as FET subgroups), remember that a smaller number of total subgroups means faster generation time, so"
		" you might want to consider a single subgroup for two or more students who have the exact same activities and constraints.");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on dividing a year"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
}

void SplitYearForm::reset() //reset to defaults
{
	QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
	 tr("Do you really want to reset the form values to defaults (empty)?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;

	separatorLineEdit->setText(" ");
	
	categoriesSpinBox->setValue(1);
	
	for(int i=0; i<MAX_CATEGORIES; i++)
		listWidgets[i]->clear();
	
	numberOfCategoriesChanged();

	tabIndexChanged(0);
}

void SplitYearForm::updateNumberOfSubgroups()
{
	qint64 n=1;
	for(int i=0; i<categoriesSpinBox->value(); i++)
		n*=listWidgets[i]->count();
	currentSubgroupsLabel->setText(tr("Subgroups: %1", "%1 is the number of subgroups").arg(n));
}

void SplitYearForm::updateDivisionsLabel()
{
	QString ts;
	
	ts=CustomFETString::number(listWidgets[0]->count());
	for(int i=1; i<categoriesSpinBox->value(); i++)
		ts+=QString(2, ' ')+CustomFETString::number(listWidgets[i]->count());

	divisionsLabel->setText(ts);
}
