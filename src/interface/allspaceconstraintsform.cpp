/***************************************************************************
                          allspaceconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "allspaceconstraintsform.h"

#include "modifyconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintroomnotavailabletimesform.h"
#include "modifyconstraintactivitypreferredroomform.h"
#include "modifyconstraintactivitypreferredroomsform.h"

#include "modifyconstraintstudentssethomeroomform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include "modifyconstraintteacherhomeroomform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentsmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintteachermaxbuildingchangesperdayform.h"
#include "modifyconstraintteachersmaxbuildingchangesperdayform.h"
#include "modifyconstraintteachermaxbuildingchangesperweekform.h"
#include "modifyconstraintteachersmaxbuildingchangesperweekform.h"
#include "modifyconstraintteachermingapsbetweenbuildingchangesform.h"
#include "modifyconstraintteachersmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintsubjectpreferredroomform.h"
#include "modifyconstraintsubjectpreferredroomsform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomsform.h"

#include "modifyconstraintactivitytagpreferredroomform.h"
#include "modifyconstraintactivitytagpreferredroomsform.h"

#include "modifyconstraintactivitiesoccupymaxdifferentroomsform.h"
#include "modifyconstraintactivitiessameroomifconsecutiveform.h"

#include "lockunlock.h"

#include "advancedfilterform.h"

#include <QMessageBox>

#include <QPlainTextEdit>

#include <QRegExp>

#include <QListWidget>
#include <QScrollBar>

#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QBrush>
#include <QPalette>

#include <QtAlgorithms>

#include <algorithm>
using namespace std;

extern const QString COMPANY;
extern const QString PROGRAM;

const int DESCRIPTION=0;
//const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

AllSpaceConstraintsForm::AllSpaceConstraintsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);
	sortedCheckBox->setChecked(false);
	
	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));
	connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));

	connect(moveSpaceConstraintUpPushButton, SIGNAL(clicked()), this, SLOT(moveSpaceConstraintUp()));
	connect(moveSpaceConstraintDownPushButton, SIGNAL(clicked()), this, SLOT(moveSpaceConstraintDown()));

	connect(sortedCheckBox, SIGNAL(toggled(bool)), this, SLOT(sortedChanged(bool)));
	connect(activatePushButton, SIGNAL(clicked()), this, SLOT(activateConstraint()));
	connect(deactivatePushButton, SIGNAL(clicked()), this, SLOT(deactivateConstraint()));
	//connect(sortByCommentsPushButton, SIGNAL(clicked()), this, SLOT(sortConstraintsByComments()));
	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(constraintComments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());

	QString settingsName="AllSpaceConstraintsAdvancedFilterForm";

	all=settings.value(settingsName+"/all-conditions", "true").toBool();

	descrDetDescr.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescr.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());

	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());

	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());

	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();

	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);
	assert(sortedCheckBox->isChecked()==false);
	
	filterChanged();
}

AllSpaceConstraintsForm::~AllSpaceConstraintsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	QString settingsName="AllSpaceConstraintsAdvancedFilterForm";

	settings.setValue(settingsName+"/all-conditions", all);

	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescr.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescr.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescr.at(i));

	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));

	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));

	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool AllSpaceConstraintsForm::filterOk(SpaceConstraint* ctr)
{
	if(!useFilter)
		return true;

	assert(descrDetDescr.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<bool> okPartial;
	
	for(int i=0; i<descrDetDescr.count(); i++){
		QString s;
		if(descrDetDescr.at(i)==DESCRIPTION)
			s=ctr->getDescription(gt.rules);
		else
			s=ctr->getDetailedDescription(gt.rules);
			
		QString t=text.at(i);
		if(contains.at(i)==CONTAINS){
			okPartial.append(s.contains(t, csens));
		}
		else if(contains.at(i)==DOESNOTCONTAIN){
			okPartial.append(!(s.contains(t, csens)));
		}
		else if(contains.at(i)==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)>=0);
		}
		else if(contains.at(i)==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)<0);
		}
		else
			assert(0);
	}
	
	if(all){
		bool ok=true;
		foreach(bool b, okPartial)
			ok = ok && b;
			
		return ok;
	}
	else{ //any
		bool ok=false;
		foreach(bool b, okPartial)
			ok = ok || b;
			
		return ok;
	}
}

void AllSpaceConstraintsForm::moveSpaceConstraintUp()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint, the 'Sorted' check box must not be checked."));
		return;
	}
	
	if(constraintsListWidget->count()<=1)
		return;
	int i=constraintsListWidget->currentRow();
	if(i<0 || i>=constraintsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=constraintsListWidget->item(i)->text();
	QString s2=constraintsListWidget->item(i-1)->text();
	
	assert(gt.rules.spaceConstraintsList.count()==visibleSpaceConstraintsList.count());
	SpaceConstraint* sc1=gt.rules.spaceConstraintsList.at(i);
	assert(sc1==visibleSpaceConstraintsList.at(i));
	SpaceConstraint* sc2=gt.rules.spaceConstraintsList.at(i-1);
	assert(sc2==visibleSpaceConstraintsList.at(i-1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i-1)->setText(s1);
	
	gt.rules.spaceConstraintsList[i]=sc2;
	gt.rules.spaceConstraintsList[i-1]=sc1;
	
	visibleSpaceConstraintsList[i]=sc2;
	visibleSpaceConstraintsList[i-1]=sc1;
	
	if(USE_GUI_COLORS){
		if(sc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(sc1->active)
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i-1);
	constraintChanged(/*i-1*/);
}

void AllSpaceConstraintsForm::moveSpaceConstraintDown()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint, the 'Sorted' check box must not be checked."));
		return;
	}
	
	if(constraintsListWidget->count()<=1)
		return;
	int i=constraintsListWidget->currentRow();
	if(i<0 || i>=constraintsListWidget->count())
		return;
	if(i==constraintsListWidget->count()-1)
		return;
		
	QString s1=constraintsListWidget->item(i)->text();
	QString s2=constraintsListWidget->item(i+1)->text();
	
	assert(gt.rules.spaceConstraintsList.count()==visibleSpaceConstraintsList.count());
	SpaceConstraint* sc1=gt.rules.spaceConstraintsList.at(i);
	assert(sc1==visibleSpaceConstraintsList.at(i));
	SpaceConstraint* sc2=gt.rules.spaceConstraintsList.at(i+1);
	assert(sc2==visibleSpaceConstraintsList.at(i+1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i+1)->setText(s1);
	
	gt.rules.spaceConstraintsList[i]=sc2;
	gt.rules.spaceConstraintsList[i+1]=sc1;
	
	visibleSpaceConstraintsList[i]=sc2;
	visibleSpaceConstraintsList[i+1]=sc1;
	
	if(USE_GUI_COLORS){
		if(sc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(sc1->active)
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i+1);
	constraintChanged(/*i+1*/);
}

void AllSpaceConstraintsForm::sortedChanged(bool checked)
{
	Q_UNUSED(checked);
	
	filterChanged();
}

static int spaceConstraintsAscendingByDescription(SpaceConstraint* s1, SpaceConstraint* s2)
{
	return s1->getDescription(gt.rules) < s2->getDescription(gt.rules);
}

void AllSpaceConstraintsForm::filterChanged()
{
	visibleSpaceConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	foreach(SpaceConstraint* ctr, gt.rules.spaceConstraintsList)
		if(filterOk(ctr))
			visibleSpaceConstraintsList.append(ctr);
			
	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleSpaceConstraintsList.begin(), visibleSpaceConstraintsList.end(), spaceConstraintsAscendingByDescription);
	
	foreach(SpaceConstraint* ctr, visibleSpaceConstraintsList){
		assert(filterOk(ctr));
		constraintsListWidget->addItem(ctr->getDescription(gt.rules));

		if(USE_GUI_COLORS && !ctr->active)
			constraintsListWidget->item(constraintsListWidget->count()-1)->setBackground(constraintsListWidget->palette().alternateBase());

		if(ctr->active)
			n_active++;
	}
		
	if(constraintsListWidget->count()<=0)
		currentConstraintTextEdit->setPlainText("");
	else
		constraintsListWidget->setCurrentRow(0);
	
	constraintsTextLabel->setText(tr("%1 / %2 space constraints",
	 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
	 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
}

void AllSpaceConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;

	assert(index<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(index);
	assert(ctr!=NULL);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void AllSpaceConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
	
	//1
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
		ModifyConstraintBasicCompulsorySpaceForm form(this, (ConstraintBasicCompulsorySpace*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//2
	else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
		ModifyConstraintRoomNotAvailableTimesForm form(this, (ConstraintRoomNotAvailableTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//3
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		ModifyConstraintActivityPreferredRoomForm form(this, (ConstraintActivityPreferredRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//4
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
		ModifyConstraintActivityPreferredRoomsForm form(this, (ConstraintActivityPreferredRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//5
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
		ModifyConstraintStudentsSetHomeRoomForm form(this, (ConstraintStudentsSetHomeRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//6
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ModifyConstraintStudentsSetHomeRoomsForm form(this, (ConstraintStudentsSetHomeRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//7
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
		ModifyConstraintTeacherHomeRoomForm form(this, (ConstraintTeacherHomeRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//8
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ModifyConstraintTeacherHomeRoomsForm form(this, (ConstraintTeacherHomeRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//9
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
		ModifyConstraintSubjectPreferredRoomForm form(this, (ConstraintSubjectPreferredRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//10
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
		ModifyConstraintSubjectPreferredRoomsForm form(this, (ConstraintSubjectPreferredRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//11
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintSubjectActivityTagPreferredRoomForm form(this, (ConstraintSubjectActivityTagPreferredRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//12
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintSubjectActivityTagPreferredRoomsForm form(this, (ConstraintSubjectActivityTagPreferredRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	///6 apr 2009
	//13
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintActivityTagPreferredRoomForm form(this, (ConstraintActivityTagPreferredRoom*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//14
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintActivityTagPreferredRoomsForm form(this, (ConstraintActivityTagPreferredRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	///
	//15
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//16
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsMaxBuildingChangesPerDayForm form(this, (ConstraintStudentsMaxBuildingChangesPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//17
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//18
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsMaxBuildingChangesPerWeekForm form(this, (ConstraintStudentsMaxBuildingChangesPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//19
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm form(this, (ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//20
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsMinGapsBetweenBuildingChangesForm form(this, (ConstraintStudentsMinGapsBetweenBuildingChanges*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//21
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeacherMaxBuildingChangesPerDayForm form(this, (ConstraintTeacherMaxBuildingChangesPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//22
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeachersMaxBuildingChangesPerDayForm form(this, (ConstraintTeachersMaxBuildingChangesPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//23
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeacherMaxBuildingChangesPerWeekForm form(this, (ConstraintTeacherMaxBuildingChangesPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//24
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeachersMaxBuildingChangesPerWeekForm form(this, (ConstraintTeachersMaxBuildingChangesPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//25
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeacherMinGapsBetweenBuildingChangesForm form(this, (ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//26
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm form(this, (ConstraintTeachersMinGapsBetweenBuildingChanges*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//27
	else if(ctr->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS){
		ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm form(this, (ConstraintActivitiesOccupyMaxDifferentRooms*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//28
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE){
		ModifyConstraintActivitiesSameRoomIfConsecutiveForm form(this, (ConstraintActivitiesSameRoomIfConsecutive*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	else{
		QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
		 " is not correctly recognized in all space constraints dialog. FET will skip this error, so that you can continue work."
		 " Probably the constraint can be modified from the specific constraint dialog."));
//		assert(0);
//		exit(1);
	}

	filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	
	constraintsListWidget->setFocus();
}

void AllSpaceConstraintsForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	bool recompute, t;
	
	QListWidgetItem* item;
	
	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		//The user clicked the OK button or pressed Enter
		
		QMessageBox::StandardButton wr=QMessageBox::Yes;
		
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){ //additional confirmation for this one
			QString s=tr("Do you really want to remove the basic compulsory space constraint?");
			s+=" ";
			s+=tr("You cannot generate a timetable without this constraint.");
			s+="\n\n";
			s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
				"Miscellaneous -> Basic compulsory space constraints.");
			
			wr=QMessageBox::warning(this, tr("FET warning"), s,
				QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		}
		
		if(wr==QMessageBox::Yes){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				recompute=true;
			}
			else{
				recompute=false;
			}
	
			t=gt.rules.removeSpaceConstraint(ctr);
			assert(t);
			visibleSpaceConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			item=constraintsListWidget->takeItem(i);
			delete item;
			
			constraintsTextLabel->setText(tr("%1 Space Constraints", "%1 represents the number of constraints").arg(visibleSpaceConstraintsList.count()));
	
			if(recompute){
				LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
				LockUnlock::increaseCommunicationSpinBox();
			}
		}
	}
	//else if(lres==1){
		//The user clicked the Cancel button or pressed Escape
	//}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		currentConstraintTextEdit->setPlainText(QString(""));
}

void AllSpaceConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, all, descrDetDescr, contains, text, caseSensitive, "AllSpaceConstraintsAdvancedFilterForm");

	int t=filterForm->exec();
	
	if(t==QDialog::Accepted){
		assert(useFilter==false);
		useFilter=true;
	
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
			
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
			
		descrDetDescr.clear();
		contains.clear();
		text.clear();
			
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescr.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
		
		disconnect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
	}
	
	delete filterForm;
}

void AllSpaceConstraintsForm::activateConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
	
	if(!ctr->active){
		ctr->active=true;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			constraintsListWidget->currentItem()->setBackground(constraintsListWidget->palette().base());
		constraintChanged();
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
			LockUnlock::increaseCommunicationSpinBox();
		}
	
		int n_active=0;
		foreach(SpaceConstraint* ctr2, gt.rules.spaceConstraintsList)
			if(filterOk(ctr2)){
				if(ctr2->active)
					n_active++;
			}
		
		constraintsTextLabel->setText(tr("%1 / %2 space constraints",
		 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
		 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
	}
	
	constraintsListWidget->setFocus();
}

void AllSpaceConstraintsForm::deactivateConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	if(ctr->active){
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
			QMessageBox::warning(this, tr("FET warning"), tr("You are not allowed to deactivate the basic compulsory space constraints"));
			return;
		}
	
		ctr->active=false;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			constraintsListWidget->currentItem()->setBackground(constraintsListWidget->palette().alternateBase());
		constraintChanged();
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
			LockUnlock::increaseCommunicationSpinBox();
		}
	
		int n_active=0;
		foreach(SpaceConstraint* ctr2, gt.rules.spaceConstraintsList)
			if(filterOk(ctr2)){
				if(ctr2->active)
					n_active++;
			}
		
		constraintsTextLabel->setText(tr("%1 / %2 space constraints",
		 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
		 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
	}
	
	constraintsListWidget->setFocus();
}

/*static int spaceConstraintsAscendingByComments(const SpaceConstraint* s1, const SpaceConstraint* s2)
{
	return s1->comments < s2->comments;
}

void AllSpaceConstraintsForm::sortConstraintsByComments()
{
	QMessageBox::StandardButton t=QMessageBox::question(this, tr("Sort constraints?"),
	 tr("This will sort the space constraints list ascending according to their comments. You can obtain "
	 "a custom ordering by adding comments to some or all space constraints, for example 'rank #1 ... other comments', "
	 "'rank #2 ... other different comments'.")
	 +" "+tr("Are you sure you want to continue?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	
	if(t==QMessageBox::Cancel)
		return;
	
	std::stable_sort(gt.rules.spaceConstraintsList.begin(), gt.rules.spaceConstraintsList.end(), spaceConstraintsAscendingByComments);

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	filterChanged();
}*/

void AllSpaceConstraintsForm::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Constraint comments"));
	
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
	commentsPT->setPlainText(ctr->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("SpaceConstraintCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		ctr->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
		constraintChanged();
	}
}
