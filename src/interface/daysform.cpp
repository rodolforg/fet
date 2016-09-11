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

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "daysform.h"

#include <QLineEdit>
#include <QMessageBox>

extern Timetable gt;

static QLineEdit* daysNames[35];
static int nDays;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

DaysForm::DaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(daysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(daysChanged()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	nDays=gt.rules.nDaysPerWeek;
	
	daysNames[0]=day1LineEdit;
	daysNames[1]=day2LineEdit;
	daysNames[2]=day3LineEdit;
	daysNames[3]=day4LineEdit;
	daysNames[4]=day5LineEdit;
	daysNames[5]=day6LineEdit;
	daysNames[6]=day7LineEdit;

	daysNames[7]=day8LineEdit;
	daysNames[8]=day9LineEdit;
	daysNames[9]=day10LineEdit;
	daysNames[10]=day11LineEdit;
	daysNames[11]=day12LineEdit;
	daysNames[12]=day13LineEdit;
	daysNames[13]=day14LineEdit;

	daysNames[14]=day15LineEdit;
	daysNames[15]=day16LineEdit;
	daysNames[16]=day17LineEdit;
	daysNames[17]=day18LineEdit;
	daysNames[18]=day19LineEdit;
	daysNames[19]=day20LineEdit;
	daysNames[20]=day21LineEdit;

	daysNames[21]=day22LineEdit;
	daysNames[22]=day23LineEdit;
	daysNames[23]=day24LineEdit;
	daysNames[24]=day25LineEdit;
	daysNames[25]=day26LineEdit;
	daysNames[26]=day27LineEdit;
	daysNames[27]=day28LineEdit;

	daysNames[28]=day29LineEdit;
	daysNames[29]=day30LineEdit;
	daysNames[30]=day31LineEdit;
	daysNames[31]=day32LineEdit;
	daysNames[32]=day33LineEdit;
	daysNames[33]=day34LineEdit;
	daysNames[34]=day35LineEdit;

	daysSpinBox->setMinimum(1);
	daysSpinBox->setMaximum(35);
	daysSpinBox->setValue(gt.rules.nDaysPerWeek);

	for(int i=0; i<35; i++)
		if(i<nDays){
			daysNames[i]->setEnabled(true);
			daysNames[i]->setText(gt.rules.daysOfTheWeek[i]);
		}
		else
			daysNames[i]->setDisabled(true);
}

DaysForm::~DaysForm()
{
	saveFETDialogGeometry(this);
}

void DaysForm::daysChanged()
{
	nDays=daysSpinBox->value();
	assert(nDays <= MAX_DAYS_PER_WEEK);
	for(int i=0; i<35; i++)
		if(i<nDays)
			daysNames[i]->setEnabled(true);
		else
			daysNames[i]->setDisabled(true);
}

void DaysForm::ok()
{
	for(int i=0; i<nDays; i++)
		if(daysNames[i]->text()==""){
			QMessageBox::warning(this, tr("FET information"),
				tr("Empty names not allowed"));
			return;
		}
	for(int i=0; i<nDays-1; i++)
		for(int j=i+1; j<nDays; j++)
			if(daysNames[i]->text()==daysNames[j]->text()){
				QMessageBox::warning(this, tr("FET information"),
					tr("Duplicates not allowed"));
				return;
			}
	
	//2011-10-18
	int cnt_mod=0;
	int cnt_rem=0;
	int oldDays=gt.rules.nDaysPerWeek;
	gt.rules.nDaysPerWeek=nDays;
	
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
		if(tc->hasWrongDayOrHour(gt.rules)){
			if(tc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}

	foreach(SpaceConstraint* sc, gt.rules.spaceConstraintsList)
		if(sc->hasWrongDayOrHour(gt.rules)){
			if(sc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}
	
	gt.rules.nDaysPerWeek=oldDays;
			
	if(cnt_mod>0 || cnt_rem>0){
		QString s=QString("");
		if(cnt_rem>0){
			s+=tr("%1 constraints will be removed.", "%1 is the number of constraints").arg(cnt_rem);
			s+=" ";
		}
		if(cnt_mod>0){
			s+=tr("%1 constraints will be modified.", "%1 is the number of constraints").arg(cnt_mod);
			s+=" ";
		}
		s+=tr("Do you want to continue?");

		int res=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::Cancel);
		
		if(res==QMessageBox::Cancel)
			return;
			
		int _oldDays=gt.rules.nDaysPerWeek;
		gt.rules.nDaysPerWeek=nDays;

		//time
		QList<TimeConstraint*> toBeRemovedTime;
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
			if(tc->hasWrongDayOrHour(gt.rules)){
				bool tmp=tc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=tc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedTime.append(tc);
				}
			}
		}
		bool recomputeTime=false;

		if(toBeRemovedTime.count()>0){
			foreach(TimeConstraint* tc, toBeRemovedTime){
				if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
				bool tmp=gt.rules.removeTimeConstraint(tc);
				assert(tmp);
			}
		}
		//////

		//space
		QList<SpaceConstraint*> toBeRemovedSpace;
		foreach(SpaceConstraint* sc, gt.rules.spaceConstraintsList){
			if(sc->hasWrongDayOrHour(gt.rules)){
				bool tmp=sc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=sc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedSpace.append(sc);
				}
			}
		}
		
		bool recomputeSpace=false;

		if(toBeRemovedSpace.count()>0){
			foreach(SpaceConstraint* sc, toBeRemovedSpace){
				if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
				bool tmp=gt.rules.removeSpaceConstraint(sc);
				assert(tmp);
			}
		}
		//////

		gt.rules.nDaysPerWeek=_oldDays;

		if(recomputeTime){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		}
		if(recomputeSpace){
			assert(0);
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		}
		if(recomputeTime || recomputeSpace){
			LockUnlock::increaseCommunicationSpinBox();
		}
	}
	////////////

	if(gt.rules.nDaysPerWeek!=nDays){
		students_schedule_ready=false;
		teachers_schedule_ready=false;
		rooms_schedule_ready=false;
	}
		
	//remove old names
	for(int i=nDays; i<gt.rules.nDaysPerWeek; i++)
		gt.rules.daysOfTheWeek[i]="";

	gt.rules.nDaysPerWeek=nDays;
	for(int i=0; i<nDays; i++)
		gt.rules.daysOfTheWeek[i]=daysNames[i]->text();
		
	gt.rules.nHoursPerWeek=gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay; //not needed
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void DaysForm::cancel()
{
	this->close();
}
