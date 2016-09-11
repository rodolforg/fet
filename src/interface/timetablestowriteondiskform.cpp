//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2016 Liviu Lalescu <http://lalescu.ro/liviu/>
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

#include "timetablestowriteondiskform.h"

TimetablesToWriteOnDiskForm::TimetablesToWriteOnDiskForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(wasAccepted()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(wasCanceled()));
	
	softConflictsCheckBox->setChecked(WRITE_TIMETABLE_CONFLICTS);

	statisticsCheckBox->setChecked(WRITE_TIMETABLES_STATISTICS);
	xmlCheckBox->setChecked(WRITE_TIMETABLES_XML);
	daysHorizontalCheckBox->setChecked(WRITE_TIMETABLES_DAYS_HORIZONTAL);
	daysVerticalCheckBox->setChecked(WRITE_TIMETABLES_DAYS_VERTICAL);
	timeHorizontalCheckBox->setChecked(WRITE_TIMETABLES_TIME_HORIZONTAL);
	timeVerticalCheckBox->setChecked(WRITE_TIMETABLES_TIME_VERTICAL);

	subgroupsCheckBox->setChecked(WRITE_TIMETABLES_SUBGROUPS);
	groupsCheckBox->setChecked(WRITE_TIMETABLES_GROUPS);
	yearsCheckBox->setChecked(WRITE_TIMETABLES_YEARS);
	teachersCheckBox->setChecked(WRITE_TIMETABLES_TEACHERS);
	teachersFreePeriodsCheckBox->setChecked(WRITE_TIMETABLES_TEACHERS_FREE_PERIODS);
	roomsCheckBox->setChecked(WRITE_TIMETABLES_ROOMS);
	subjectsCheckBox->setChecked(WRITE_TIMETABLES_SUBJECTS);
	activityTagsCheckBox->setChecked(WRITE_TIMETABLES_ACTIVITY_TAGS);
	activitiesCheckBox->setChecked(WRITE_TIMETABLES_ACTIVITIES);
}

TimetablesToWriteOnDiskForm::~TimetablesToWriteOnDiskForm()
{
	saveFETDialogGeometry(this);
}

void TimetablesToWriteOnDiskForm::wasAccepted()
{
	WRITE_TIMETABLE_CONFLICTS=softConflictsCheckBox->isChecked();

	WRITE_TIMETABLES_STATISTICS=statisticsCheckBox->isChecked();
	WRITE_TIMETABLES_XML=xmlCheckBox->isChecked();
	WRITE_TIMETABLES_DAYS_HORIZONTAL=daysHorizontalCheckBox->isChecked();
	WRITE_TIMETABLES_DAYS_VERTICAL=daysVerticalCheckBox->isChecked();
	WRITE_TIMETABLES_TIME_HORIZONTAL=timeHorizontalCheckBox->isChecked();
	WRITE_TIMETABLES_TIME_VERTICAL=timeVerticalCheckBox->isChecked();

	WRITE_TIMETABLES_SUBGROUPS=subgroupsCheckBox->isChecked();
	WRITE_TIMETABLES_GROUPS=groupsCheckBox->isChecked();
	WRITE_TIMETABLES_YEARS=yearsCheckBox->isChecked();
	WRITE_TIMETABLES_TEACHERS=teachersCheckBox->isChecked();
	WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=teachersFreePeriodsCheckBox->isChecked();
	WRITE_TIMETABLES_ROOMS=roomsCheckBox->isChecked();
	WRITE_TIMETABLES_SUBJECTS=subjectsCheckBox->isChecked();
	WRITE_TIMETABLES_ACTIVITY_TAGS=activityTagsCheckBox->isChecked();
	WRITE_TIMETABLES_ACTIVITIES=activitiesCheckBox->isChecked();

	this->accept();
}

void TimetablesToWriteOnDiskForm::wasCanceled()
{
	this->reject();
}
