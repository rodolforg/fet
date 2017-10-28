//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
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

static QLineEdit* hoursNames[MAX_HOURS_PER_DAY];
static int nHours;

HoursForm::HoursForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(hoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(hoursChanged()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	for(int i=0; i<MAX_HOURS_PER_DAY; i++) {
		int row = i % 8;
		int col = i / 8;
		QLabel * hourLabel = new QLabel();
		hourLabel->setText(tr("Hour %1").arg(i+1));
		hourTable->addWidget(hourLabel, row*2, col);
		QLineEdit * hourLineEdit = new QLineEdit();
		hourTable->addWidget(hourLineEdit, row*2+1, col);
		hoursNames[i] = hourLineEdit;
	}

	nHours=gt.rules.nHoursPerDay;

	hoursSpinBox->setMinimum(1);
	hoursSpinBox->setMaximum(MAX_HOURS_PER_DAY);
	hoursSpinBox->setValue(gt.rules.nHoursPerDay);

	for(int i=0; i<MAX_HOURS_PER_DAY; i++) {
		if(i<nHours){
			hoursNames[i]->setEnabled(true);
			hoursNames[i]->setText(gt.rules.hoursOfTheDay[i]);
		}
		else
			hoursNames[i]->setDisabled(true);
	}
}

HoursForm::~HoursForm()
{
	saveFETDialogGeometry(this);
}

void HoursForm::hoursChanged()
{
	nHours=hoursSpinBox->value();
	assert(nHours <= MAX_HOURS_PER_DAY);
    for(int i=0; i<MAX_HOURS_PER_DAY; i++)
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


	QStringList hourList;
	for(int i=0; i<nHours; i++)
		hourList << hoursNames[i]->text();

	gt.rules.setHours(hourList);

	assert(gt.rules.nHoursPerDay<=MAX_HOURS_PER_DAY);
		
	this->close();
}
