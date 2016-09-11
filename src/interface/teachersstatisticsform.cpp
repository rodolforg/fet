/***************************************************************************
                          teacherstatisticform.cpp  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#include "teachersstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <QString>
#include <QStringList>

#include <QTableWidget>
#include <QHeaderView>

#include <QSet>
#include <QHash>

TeachersStatisticsForm::TeachersStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	connect(hideFullTeachersCheckBox, SIGNAL(toggled(bool)), this, SLOT(hideFullTeachersCheckBoxModified()));

	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QHash<QString, QSet<Activity*> > activitiesForTeacher;
	
	foreach(Activity* act, gt.rules.activitiesList)
		if(act->active)
			foreach(QString teacherName, act->teachersNames){
				QSet<Activity*> acts=activitiesForTeacher.value(teacherName, QSet<Activity*>());
				acts.insert(act);
				activitiesForTeacher.insert(teacherName, acts);
			}
	
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		
		int	nSubActivities=0;
		int nHours=0;
		
		QSet<Activity*> acts=activitiesForTeacher.value(t->name, QSet<Activity*>());
		
		foreach(Activity* act, acts){
			if(act->active){
				nSubActivities++;
				nHours+=act->duration;
			}
			else{
				assert(0);
			}
		}
		
		names.append(t->name);
		subactivities.append(nSubActivities);
		durations.append(nHours);
		targets.append(t->targetNumberOfHours);

		if(nHours==t->targetNumberOfHours)
			hideFullTeacher.append(true);
		else
			hideFullTeacher.append(false);
	}

	hideFullTeachersCheckBoxModified();
}

TeachersStatisticsForm::~TeachersStatisticsForm()
{
	saveFETDialogGeometry(this);
}

void TeachersStatisticsForm::hideFullTeachersCheckBoxModified()
{
	tableWidget->clear();
	
	int n_rows=0;
	foreach(bool b, hideFullTeacher)
		if(!(hideFullTeachersCheckBox->isChecked() && b))
			n_rows++;

	tableWidget->setColumnCount(4);
	tableWidget->setRowCount(n_rows);
	
	QStringList columns;
	columns<<tr("Teacher");
	columns<<tr("No. of activities");
	columns<<tr("Duration");
	columns<<tr("Target duration", "It means the target duration of activities for each teacher");
	
	tableWidget->setHorizontalHeaderLabels(columns);

	int j=0;
	for(int i=0; i<gt.rules.teachersList.count(); i++){
		if(!(hideFullTeachersCheckBox->isChecked() && hideFullTeacher.at(i))){
			QTableWidgetItem* newItem=new QTableWidgetItem(names.at(i));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 0, newItem);

			newItem=new QTableWidgetItem(CustomFETString::number(subactivities.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 1, newItem);
	
			newItem=new QTableWidgetItem(CustomFETString::number(durations.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 2, newItem);
		
			newItem=new QTableWidgetItem(CustomFETString::number(targets.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 3, newItem);
			
			j++;
		}
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}
