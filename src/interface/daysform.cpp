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

#include "timetable_defs.h"
#include "fet.h"

#include "lockunlock.h"

#include "daysform.h"

#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

static QLineEdit* daysNames[MAX_DAYS_PER_WEEK];
static int nDays;

DaysForm::DaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(daysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(daysChanged()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	nDays=gt.rules.nDaysPerWeek;
	
    for(int i=0; i<MAX_DAYS_PER_WEEK; i++) {
        int row = i % 7;
        int col = i / 7;
        QLabel * dayLabel = new QLabel();
        dayLabel->setText(tr("Day %1").arg(i+1));
        dayTable->addWidget(dayLabel, row*2, col);
        QLineEdit * dayLineEdit = new QLineEdit();
        dayTable->addWidget(dayLineEdit, row*2+1, col);
        daysNames[i] = dayLineEdit;
    }

	daysSpinBox->setMinimum(1);
    daysSpinBox->setMaximum(MAX_DAYS_PER_WEEK);
	daysSpinBox->setValue(gt.rules.nDaysPerWeek);

    for(int i=0; i<MAX_DAYS_PER_WEEK; i++)
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
    for(int i=0; i<MAX_DAYS_PER_WEEK; i++)
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

	bool userChangedDays = false;
	for(int i=0; i < nDays; i++) {
		if (daysNames[i]->text() != gt.rules.daysOfTheWeek[i]) {
			userChangedDays = true;
			break;
		}
	}
	if (!userChangedDays)
		return;
	
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
		QString s;
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

	QStringList dayList;
	for(int i=0; i<nDays; i++)
		dayList << daysNames[i]->text();

	gt.rules.setDays(dayList);

	this->close();
}

