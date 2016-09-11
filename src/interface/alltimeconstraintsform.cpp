/***************************************************************************
                          alltimeconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "alltimeconstraintsform.h"

#include "modifyconstraintbreaktimesform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"

#include "modifyconstraintactivitypreferredtimeslotsform.h"
#include "modifyconstraintactivitypreferredstartingtimesform.h"
#include "modifyconstrainttwoactivitiesconsecutiveform.h"
#include "modifyconstrainttwoactivitiesgroupedform.h"
#include "modifyconstraintthreeactivitiesgroupedform.h"
#include "modifyconstrainttwoactivitiesorderedform.h"

#include "modifyconstraintactivitiespreferredtimeslotsform.h"
#include "modifyconstraintactivitiespreferredstartingtimesform.h"

#include "modifyconstraintsubactivitiespreferredtimeslotsform.h"
#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"

#include "modifyconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartingdayform.h"
#include "modifyconstraintactivitypreferredstartingtimeform.h"
#include "modifyconstraintactivitiesnotoverlappingform.h"
#include "modifyconstraintmindaysbetweenactivitiesform.h"
#include "modifyconstraintmaxdaysbetweenactivitiesform.h"
#include "modifyconstraintmingapsbetweenactivitiesform.h"
#include "modifyconstraintactivityendsstudentsdayform.h"

#include "modifyconstraintactivitiesendstudentsdayform.h"

#include "modifyconstraintteachernotavailabletimesform.h"
#include "modifyconstraintteachersmaxgapsperweekform.h"
#include "modifyconstraintteachermaxgapsperweekform.h"
#include "modifyconstraintteachersmaxgapsperdayform.h"
#include "modifyconstraintteachermaxgapsperdayform.h"
#include "modifyconstraintteachermaxdaysperweekform.h"
#include "modifyconstraintteachersmaxdaysperweekform.h"
#include "modifyconstraintteachersmaxhoursdailyform.h"
#include "modifyconstraintteachermaxhoursdailyform.h"
#include "modifyconstraintteachersmaxhourscontinuouslyform.h"
#include "modifyconstraintteachermaxhourscontinuouslyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteacherminhoursdailyform.h"

#include "modifyconstraintteachermindaysperweekform.h"
#include "modifyconstraintteachersmindaysperweekform.h"

#include "modifyconstraintteachersactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintteacheractivitytagmaxhourscontinuouslyform.h"

#include "modifyconstraintteachersactivitytagmaxhoursdailyform.h"
#include "modifyconstraintteacheractivitytagmaxhoursdailyform.h"

#include "modifyconstraintstudentssetnotavailabletimesform.h"
#include "modifyconstraintstudentssetmaxgapsperweekform.h"
#include "modifyconstraintstudentsmaxgapsperweekform.h"

#include "modifyconstraintstudentssetmaxgapsperdayform.h"
#include "modifyconstraintstudentsmaxgapsperdayform.h"

#include "modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetmaxhoursdailyform.h"
#include "modifyconstraintstudentsmaxhoursdailyform.h"
#include "modifyconstraintstudentssetmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentssetminhoursdailyform.h"
#include "modifyconstraintstudentsminhoursdailyform.h"

#include "modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include "modifyconstraintstudentssetactivitytagmaxhoursdailyform.h"
#include "modifyconstraintstudentsactivitytagmaxhoursdailyform.h"

#include "modifyconstraintteacherintervalmaxdaysperweekform.h"
#include "modifyconstraintteachersintervalmaxdaysperweekform.h"

#include "modifyconstraintstudentssetintervalmaxdaysperweekform.h"
#include "modifyconstraintstudentsintervalmaxdaysperweekform.h"

#include "modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform.h"
#include "modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"

#include "modifyconstraintstudentssetmaxdaysperweekform.h"
#include "modifyconstraintstudentsmaxdaysperweekform.h"

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

AllTimeConstraintsForm::AllTimeConstraintsForm(QWidget* parent): QDialog(parent)
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

	connect(moveTimeConstraintUpPushButton, SIGNAL(clicked()), this, SLOT(moveTimeConstraintUp()));
	connect(moveTimeConstraintDownPushButton, SIGNAL(clicked()), this, SLOT(moveTimeConstraintDown()));

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
		
	QString settingsName="AllTimeConstraintsAdvancedFilterForm";
	
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

AllTimeConstraintsForm::~AllTimeConstraintsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	QString settingsName="AllTimeConstraintsAdvancedFilterForm";
	
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

bool AllTimeConstraintsForm::filterOk(TimeConstraint* ctr)
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

void AllTimeConstraintsForm::moveTimeConstraintUp()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint, the 'Sorted' check box must not be checked."));
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
	
	assert(gt.rules.timeConstraintsList.count()==visibleTimeConstraintsList.count());
	TimeConstraint* tc1=gt.rules.timeConstraintsList.at(i);
	assert(tc1==visibleTimeConstraintsList.at(i));
	TimeConstraint* tc2=gt.rules.timeConstraintsList.at(i-1);
	assert(tc2==visibleTimeConstraintsList.at(i-1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i-1)->setText(s1);
	
	gt.rules.timeConstraintsList[i]=tc2;
	gt.rules.timeConstraintsList[i-1]=tc1;
	
	visibleTimeConstraintsList[i]=tc2;
	visibleTimeConstraintsList[i-1]=tc1;
	
	if(USE_GUI_COLORS){
		if(tc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(tc1->active)
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i-1);
	constraintChanged(/*i-1*/);
}

void AllTimeConstraintsForm::moveTimeConstraintDown()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint, the 'Sorted' check box must not be checked."));
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
	
	assert(gt.rules.timeConstraintsList.count()==visibleTimeConstraintsList.count());
	TimeConstraint* tc1=gt.rules.timeConstraintsList.at(i);
	assert(tc1==visibleTimeConstraintsList.at(i));
	TimeConstraint* tc2=gt.rules.timeConstraintsList.at(i+1);
	assert(tc2==visibleTimeConstraintsList.at(i+1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i+1)->setText(s1);
	
	gt.rules.timeConstraintsList[i]=tc2;
	gt.rules.timeConstraintsList[i+1]=tc1;
	
	visibleTimeConstraintsList[i]=tc2;
	visibleTimeConstraintsList[i+1]=tc1;
	
	if(USE_GUI_COLORS){
		if(tc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(tc1->active)
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i+1);
	constraintChanged(/*i+1*/);
}

void AllTimeConstraintsForm::sortedChanged(bool checked)
{
	Q_UNUSED(checked);

	filterChanged();
}

static int timeConstraintsAscendingByDescription(TimeConstraint* t1, TimeConstraint* t2)
{
	return t1->getDescription(gt.rules) < t2->getDescription(gt.rules);
}

void AllTimeConstraintsForm::filterChanged()
{
	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	foreach(TimeConstraint* ctr, gt.rules.timeConstraintsList)
		if(filterOk(ctr))
			visibleTimeConstraintsList.append(ctr);
		
	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleTimeConstraintsList.begin(), visibleTimeConstraintsList.end(), timeConstraintsAscendingByDescription);

	foreach(TimeConstraint* ctr, visibleTimeConstraintsList){
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
	
	constraintsTextLabel->setText(tr("%1 / %2 time constraints",
	 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
	 .arg(n_active).arg(visibleTimeConstraintsList.count()));
}

void AllTimeConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;
		
	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=NULL);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void AllTimeConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
	
	//1
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
		ModifyConstraintBasicCompulsoryTimeForm form(this, (ConstraintBasicCompulsoryTime*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//2
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE){
		ModifyConstraintTwoActivitiesConsecutiveForm form(this, (ConstraintTwoActivitiesConsecutive*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//3
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED){
		ModifyConstraintTwoActivitiesOrderedForm form(this, (ConstraintTwoActivitiesOrdered*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//4
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
		ModifyConstraintActivityPreferredTimeSlotsForm form(this, (ConstraintActivityPreferredTimeSlots*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//5
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
		ModifyConstraintActivityPreferredStartingTimesForm form(this, (ConstraintActivityPreferredStartingTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//6
	else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
		ModifyConstraintActivitiesPreferredTimeSlotsForm form(this, (ConstraintActivitiesPreferredTimeSlots*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//7
	else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
		ModifyConstraintActivitiesPreferredStartingTimesForm form(this, (ConstraintActivitiesPreferredStartingTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//8
	else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
		ModifyConstraintSubactivitiesPreferredTimeSlotsForm form(this, (ConstraintSubactivitiesPreferredTimeSlots*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//9
	else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
		ModifyConstraintSubactivitiesPreferredStartingTimesForm form(this, (ConstraintSubactivitiesPreferredStartingTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//10
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
		ModifyConstraintActivitiesSameStartingTimeForm form(this, (ConstraintActivitiesSameStartingTime*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//11
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
		ModifyConstraintActivitiesSameStartingHourForm form(this, (ConstraintActivitiesSameStartingHour*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//12
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
		ModifyConstraintActivitiesSameStartingDayForm form(this, (ConstraintActivitiesSameStartingDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//13
	else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
		ModifyConstraintTeacherNotAvailableTimesForm form(this, (ConstraintTeacherNotAvailableTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//14
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
		ModifyConstraintStudentsSetNotAvailableTimesForm form(this, (ConstraintStudentsSetNotAvailableTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//15
	else if(ctr->type==CONSTRAINT_BREAK_TIMES){
		ModifyConstraintBreakTimesForm form(this, (ConstraintBreakTimes*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//16
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherMaxDaysPerWeekForm form(this, (ConstraintTeacherMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//17
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
		ModifyConstraintTeachersMaxHoursDailyForm form(this, (ConstraintTeachersMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//18
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
		ModifyConstraintTeacherMaxHoursDailyForm form(this, (ConstraintTeacherMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//19
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersMaxHoursContinuouslyForm form(this, (ConstraintTeachersMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//20
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeacherMaxHoursContinuouslyForm form(this, (ConstraintTeacherMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//21
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
		ModifyConstraintTeachersMinHoursDailyForm form(this, (ConstraintTeachersMinHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//22
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
		ModifyConstraintTeacherMinHoursDailyForm form(this, (ConstraintTeacherMinHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//23
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeachersMaxGapsPerWeekForm form(this, (ConstraintTeachersMaxGapsPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//24
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeacherMaxGapsPerWeekForm form(this, (ConstraintTeacherMaxGapsPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//25
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
		ModifyConstraintTeachersMaxGapsPerDayForm form(this, (ConstraintTeachersMaxGapsPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//26
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
		ModifyConstraintTeacherMaxGapsPerDayForm form(this, (ConstraintTeacherMaxGapsPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//27
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
		ModifyConstraintActivityPreferredStartingTimeForm form(this, (ConstraintActivityPreferredStartingTime*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//28
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsSetMaxGapsPerWeekForm form(this, (ConstraintStudentsSetMaxGapsPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//29
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsMaxGapsPerWeekForm form(this, (ConstraintStudentsMaxGapsPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//30
	else if(ctr->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//31
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//32
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
		ModifyConstraintStudentsSetMaxHoursDailyForm form(this, (ConstraintStudentsSetMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//33
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
		ModifyConstraintStudentsMaxHoursDailyForm form(this, (ConstraintStudentsMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//34
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsSetMaxHoursContinuouslyForm form(this, (ConstraintStudentsSetMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//35
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsMaxHoursContinuouslyForm form(this, (ConstraintStudentsMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//36
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
		ModifyConstraintStudentsSetMinHoursDailyForm form(this, (ConstraintStudentsSetMinHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//37
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
		ModifyConstraintStudentsMinHoursDailyForm form(this, (ConstraintStudentsMinHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//38
	else if(ctr->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
		ModifyConstraintActivitiesNotOverlappingForm form(this, (ConstraintActivitiesNotOverlapping*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//39
	else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
		ModifyConstraintMinDaysBetweenActivitiesForm form(this, (ConstraintMinDaysBetweenActivities*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//40
	else if(ctr->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES){
		ModifyConstraintMinGapsBetweenActivitiesForm form(this, (ConstraintMinGapsBetweenActivities*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//41
	else if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
		ModifyConstraintActivityEndsStudentsDayForm form(this, (ConstraintActivityEndsStudentsDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//42
	else if(ctr->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherIntervalMaxDaysPerWeekForm form(this, (ConstraintTeacherIntervalMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//43
	else if(ctr->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeachersIntervalMaxDaysPerWeekForm form(this, (ConstraintTeachersIntervalMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//44
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//45
	else if(ctr->type==CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsIntervalMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//46
	else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
		ModifyConstraintActivitiesEndStudentsDayForm form(this, (ConstraintActivitiesEndStudentsDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//47
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED){
		ModifyConstraintTwoActivitiesGroupedForm form(this, (ConstraintTwoActivitiesGrouped*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//48
	else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm form(this, (ConstraintTeachersActivityTagMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//49
	else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeacherActivityTagMaxHoursContinuouslyForm form(this, (ConstraintTeacherActivityTagMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//50
	else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm form(this, (ConstraintStudentsActivityTagMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//51
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm form(this, (ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//52
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeachersMaxDaysPerWeekForm form(this, (ConstraintTeachersMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//53
	else if(ctr->type==CONSTRAINT_THREE_ACTIVITIES_GROUPED){
		ModifyConstraintThreeActivitiesGroupedForm form(this, (ConstraintThreeActivitiesGrouped*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//54
	else if(ctr->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
		ModifyConstraintMaxDaysBetweenActivitiesForm form(this, (ConstraintMaxDaysBetweenActivities*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//55
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK){
		ModifyConstraintTeachersMinDaysPerWeekForm form(this, (ConstraintTeachersMinDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//56
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
		ModifyConstraintTeacherMinDaysPerWeekForm form(this, (ConstraintTeacherMinDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//57
	else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintTeachersActivityTagMaxHoursDailyForm form(this, (ConstraintTeachersActivityTagMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//58
	else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintTeacherActivityTagMaxHoursDailyForm form(this, (ConstraintTeacherActivityTagMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//59
	else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintStudentsActivityTagMaxHoursDailyForm form(this, (ConstraintStudentsActivityTagMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//60
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintStudentsSetActivityTagMaxHoursDailyForm form(this, (ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}

	//61
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
		ModifyConstraintStudentsSetMaxGapsPerDayForm form(this, (ConstraintStudentsSetMaxGapsPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//62
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY){
		ModifyConstraintStudentsMaxGapsPerDayForm form(this, (ConstraintStudentsMaxGapsPerDay*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//63
	else if(ctr->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION){
		ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm form(this, (ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//64
	else if(ctr->type==CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
		ModifyConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm form(this, (ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//65
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsSetMaxDaysPerWeekForm form(this, (ConstraintStudentsSetMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
	//66
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsMaxDaysPerWeekForm form(this, (ConstraintStudentsMaxDaysPerWeek*)ctr);
		setParentAndOtherThings(&form, this);
		form.exec();
	}

	else{
		QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
		 " is not correctly recognized in all time constraints dialog. FET will skip this error, so that you can continue work."
		 " Probably the constraint can be modified from the specific constraint dialog."));
		//assert(0);
		//exit(1);
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

void AllTimeConstraintsForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	bool t;
	
	bool recompute;
	
	QListWidgetItem* item;
	
	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		//The user clicked the OK button or pressed Enter
		
		QMessageBox::StandardButton wr=QMessageBox::Yes;
		
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){ //additional confirmation for this one
			QString s=tr("Do you really want to remove the basic compulsory time constraint?");
			s+=" ";
			s+=tr("You cannot generate a timetable without this constraint.");
			s+="\n\n";
			s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
				"Miscellaneous -> Basic compulsory time constraints.");
				
			wr=QMessageBox::warning(this, tr("FET warning"), s,
				QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		}
		
		if(wr==QMessageBox::Yes){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				recompute=true;
			}
			else{
				recompute=false;
			}
		
			t=gt.rules.removeTimeConstraint(ctr);
			assert(t);
			visibleTimeConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			item=constraintsListWidget->takeItem(i);
			delete item;
		
			constraintsTextLabel->setText(tr("%1 Time Constraints", "%1 represents the number of constraints").arg(visibleTimeConstraintsList.count()));
		
			if(recompute){
				LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
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

void AllTimeConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, all, descrDetDescr, contains, text, caseSensitive, "AllTimeConstraintsAdvancedFilterForm");

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

void AllTimeConstraintsForm::activateConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
	
	if(!ctr->active){
		ctr->active=true;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			constraintsListWidget->currentItem()->setBackground(constraintsListWidget->palette().base());
		constraintChanged();
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
			LockUnlock::increaseCommunicationSpinBox();
		}
		
		int n_active=0;
		foreach(TimeConstraint* ctr2, gt.rules.timeConstraintsList)
			if(filterOk(ctr2)){
				if(ctr2->active)
					n_active++;
			}
	
		constraintsTextLabel->setText(tr("%1 / %2 time constraints",
		 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
		 .arg(n_active).arg(visibleTimeConstraintsList.count()));
	}
	
	constraintsListWidget->setFocus();
}

void AllTimeConstraintsForm::deactivateConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	if(ctr->active){
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			QMessageBox::warning(this, tr("FET warning"), tr("You are not allowed to deactivate the basic compulsory time constraints"));
			return;
		}
	
		ctr->active=false;
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
		if(USE_GUI_COLORS)
			constraintsListWidget->currentItem()->setBackground(constraintsListWidget->palette().alternateBase());
		constraintChanged();
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
			LockUnlock::increaseCommunicationSpinBox();
		}

		int n_active=0;
		foreach(TimeConstraint* ctr2, gt.rules.timeConstraintsList)
			if(filterOk(ctr2)){
				if(ctr2->active)
					n_active++;
			}
	
		constraintsTextLabel->setText(tr("%1 / %2 time constraints",
		 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
		 .arg(n_active).arg(visibleTimeConstraintsList.count()));
	}
	
	constraintsListWidget->setFocus();
}

/*static int timeConstraintsAscendingByComments(const TimeConstraint* t1, const TimeConstraint* t2)
{
	return t1->comments < t2->comments;
}

void AllTimeConstraintsForm::sortConstraintsByComments()
{
	QMessageBox::StandardButton t=QMessageBox::question(this, tr("Sort constraints?"),
	 tr("This will sort the time constraints list ascending according to their comments. You can obtain "
	 "a custom ordering by adding comments to some or all time constraints, for example 'rank #1 ... other comments', "
	 "'rank #2 ... other different comments'.")
	 +" "+tr("Are you sure you want to continue?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	
	if(t==QMessageBox::Cancel)
		return;
	
	std::stable_sort(gt.rules.timeConstraintsList.begin(), gt.rules.timeConstraintsList.end(), timeConstraintsAscendingByComments);

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	filterChanged();
}*/

void AllTimeConstraintsForm::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

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
	
	const QString settingsName=QString("TimeConstraintCommentsDialog");
	
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
