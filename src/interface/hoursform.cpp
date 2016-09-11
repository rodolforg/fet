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

#include <QMessageBox>

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "hoursform.h"

#include <QLineEdit>

extern Timetable gt;

static QLineEdit* hoursNames[60];
static int nHours;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

HoursForm::HoursForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(hoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(hoursChanged()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	nHours=gt.rules.nHoursPerDay;

	hoursNames[0]=hour1LineEdit;
	hoursNames[1]=hour2LineEdit;
	hoursNames[2]=hour3LineEdit;
	hoursNames[3]=hour4LineEdit;
	hoursNames[4]=hour5LineEdit;
	hoursNames[5]=hour6LineEdit;
	hoursNames[6]=hour7LineEdit;
	hoursNames[7]=hour8LineEdit;
	hoursNames[8]=hour9LineEdit;
	hoursNames[9]=hour10LineEdit;

	hoursNames[10]=hour11LineEdit;
	hoursNames[11]=hour12LineEdit;
	hoursNames[12]=hour13LineEdit;
	hoursNames[13]=hour14LineEdit;
	hoursNames[14]=hour15LineEdit;
	hoursNames[15]=hour16LineEdit;
	hoursNames[16]=hour17LineEdit;
	hoursNames[17]=hour18LineEdit;
	hoursNames[18]=hour19LineEdit;
	hoursNames[19]=hour20LineEdit;

	hoursNames[20]=hour21LineEdit;
	hoursNames[21]=hour22LineEdit;
	hoursNames[22]=hour23LineEdit;
	hoursNames[23]=hour24LineEdit;
	hoursNames[24]=hour25LineEdit;
	hoursNames[25]=hour26LineEdit;
	hoursNames[26]=hour27LineEdit;
	hoursNames[27]=hour28LineEdit;
	hoursNames[28]=hour29LineEdit;
	hoursNames[29]=hour30LineEdit;

	hoursNames[30]=hour31LineEdit;
	hoursNames[31]=hour32LineEdit;
	hoursNames[32]=hour33LineEdit;
	hoursNames[33]=hour34LineEdit;
	hoursNames[34]=hour35LineEdit;
	hoursNames[35]=hour36LineEdit;
	hoursNames[36]=hour37LineEdit;
	hoursNames[37]=hour38LineEdit;
	hoursNames[38]=hour39LineEdit;
	hoursNames[39]=hour40LineEdit;

	hoursNames[40]=hour41LineEdit;
	hoursNames[41]=hour42LineEdit;
	hoursNames[42]=hour43LineEdit;
	hoursNames[43]=hour44LineEdit;
	hoursNames[44]=hour45LineEdit;
	hoursNames[45]=hour46LineEdit;
	hoursNames[46]=hour47LineEdit;
	hoursNames[47]=hour48LineEdit;
	hoursNames[48]=hour49LineEdit;
	hoursNames[49]=hour50LineEdit;

	hoursNames[50]=hour51LineEdit;
	hoursNames[51]=hour52LineEdit;
	hoursNames[52]=hour53LineEdit;
	hoursNames[53]=hour54LineEdit;
	hoursNames[54]=hour55LineEdit;
	hoursNames[55]=hour56LineEdit;
	hoursNames[56]=hour57LineEdit;
	hoursNames[57]=hour58LineEdit;
	hoursNames[58]=hour59LineEdit;
	hoursNames[59]=hour60LineEdit;

	hoursSpinBox->setMinimum(1);
	hoursSpinBox->setMaximum(60);
	hoursSpinBox->setValue(gt.rules.nHoursPerDay);

	for(int i=0; i<60; i++)
		if(i<nHours){
			hoursNames[i]->setEnabled(true);
			hoursNames[i]->setText(gt.rules.hoursOfTheDay[i]);
		}
		else
			hoursNames[i]->setDisabled(true);
}

HoursForm::~HoursForm()
{
	saveFETDialogGeometry(this);
}

void HoursForm::hoursChanged()
{
	nHours=hoursSpinBox->value();
	assert(nHours <= MAX_HOURS_PER_DAY);
	for(int i=0; i<60; i++)
		if(i<nHours)
			hoursNames[i]->setEnabled(true);
		else
			hoursNames[i]->setDisabled(true);
}

void HoursForm::ok()
{
	for(int i=0; i<nHours; i++)
		if(hoursNames[i]->text()==""){
			QMessageBox::warning(this, tr("FET information"),
				tr("Empty names not allowed"));
			return;
		}
	for(int i=0; i<nHours-1; i++)
		for(int j=i+1; j<nHours; j++)
			if(hoursNames[i]->text()==hoursNames[j]->text()){
				QMessageBox::warning(this, tr("FET information"),
					tr("Duplicates not allowed"));
				return;
			}
			
	//2011-10-18
	int cnt_mod=0;
	int cnt_rem=0;
	int oldHours=gt.rules.nHoursPerDay;
	gt.rules.nHoursPerDay=nHours;

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
	
	gt.rules.nHoursPerDay=oldHours;
			
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
			
		int _oldHours=gt.rules.nHoursPerDay;
		gt.rules.nHoursPerDay=nHours;
		
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

		gt.rules.nHoursPerDay=_oldHours;

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

	if(gt.rules.nHoursPerDay!=nHours){
		students_schedule_ready=false;
		teachers_schedule_ready=false;
		rooms_schedule_ready=false;
	}

	//remove old names
	for(int i=nHours; i<gt.rules.nHoursPerDay; i++)
		gt.rules.hoursOfTheDay[i]="";
		
	gt.rules.nHoursPerDay=nHours;
	for(int i=0; i<nHours; i++)
		gt.rules.hoursOfTheDay[i]=hoursNames[i]->text();
		
	gt.rules.nHoursPerWeek=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek; //not needed
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	assert(gt.rules.nHoursPerDay<=MAX_HOURS_PER_DAY);
		
	this->close();
}

void HoursForm::cancel()
{
	this->close();
}
