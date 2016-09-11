/***************************************************************************
                          studentsstatisticform.cpp  -  description
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

#include "studentsstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include "messageboxes.h"

#include <QString>
#include <QStringList>

#include <QHash>

#include <QProgressDialog>

#include "longtextmessagebox.h"

#include <QMessageBox>
#include <QApplication>

#include <QHeaderView>
#include <QTableWidget>

extern QApplication* pqapplication;

StudentsStatisticsForm::StudentsStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));

	connect(showCompleteStructureCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	
	//2014-12-18
	QSet<StudentsYear*> allYears;
	QSet<StudentsGroup*> allGroups;
	QSet<StudentsSubgroup*> allSubgroups;
	
	QHash<QString, StudentsYear*> yearsHash;
	QHash<QString, StudentsGroup*> groupsHash;
	QHash<QString, StudentsSubgroup*> subgroupsHash;
	
	QHash<StudentsYear*, QSet<Activity*> > activitiesForYear;
	QHash<StudentsGroup*, QSet<Activity*> > activitiesForGroup;
	QHash<StudentsSubgroup*, QSet<Activity*> > activitiesForSubgroup;
	
	foreach(StudentsYear* year, gt.rules.yearsList){
		yearsHash.insert(year->name, year);
		
		allYears.insert(year);
		
		foreach(StudentsGroup* group, year->groupsList){
			groupsHash.insert(group->name, group);
			
			allGroups.insert(group);
			
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				subgroupsHash.insert(subgroup->name, subgroup);

				allSubgroups.insert(subgroup);
			}
		}
	}
	
	QString warnings=QString("");
	foreach(Activity* act, gt.rules.activitiesList)
		if(act->active){
			foreach(QString sts, act->studentsNames){
				if(yearsHash.contains(sts)){
					StudentsYear* year=yearsHash.value(sts);
	
					QSet<Activity*> acts=activitiesForYear.value(year, QSet<Activity*>());
					acts.insert(act);
					activitiesForYear.insert(year, acts);
				}
				else if(groupsHash.contains(sts)){
					StudentsGroup* group=groupsHash.value(sts);

					QSet<Activity*> acts=activitiesForGroup.value(group, QSet<Activity*>());
					acts.insert(act);
					activitiesForGroup.insert(group, acts);
				}
				else if(subgroupsHash.contains(sts)){
					StudentsSubgroup* subgroup=subgroupsHash.value(sts);

					QSet<Activity*> acts=activitiesForSubgroup.value(subgroup, QSet<Activity*>());
					acts.insert(act);
					activitiesForSubgroup.insert(subgroup, acts);
				}
				else
					warnings+=tr("Students set %1 from activity with id %2 is inexistent in the students list. Please correct this.").arg(sts).arg(act->id)+QString("\n");
			}
		}
	if(!warnings.isEmpty())
		FetMessage::warning(this, tr("FET warning"), warnings);
	
	//phase 1a
	foreach(StudentsYear* year, gt.rules.yearsList){
		QSet<Activity*> actsYear=activitiesForYear.value(year, QSet<Activity*>());
		foreach(StudentsGroup* group, year->groupsList){
			QSet<Activity*> actsGroup=activitiesForGroup.value(group, QSet<Activity*>());
			actsGroup.unite(actsYear);
			activitiesForGroup.insert(group, actsGroup);
		}
	}
	//phase 1b
	foreach(StudentsYear* year, gt.rules.yearsList){
		foreach(StudentsGroup* group, year->groupsList){
			QSet<Activity*> actsGroup=activitiesForGroup.value(group, QSet<Activity*>());
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				QSet<Activity*> actsSubgroup=activitiesForSubgroup.value(subgroup, QSet<Activity*>());
				actsSubgroup.unite(actsGroup);
				activitiesForSubgroup.insert(subgroup, actsSubgroup);
			}
		}
	}
	//phase 2a
	foreach(StudentsYear* year, gt.rules.yearsList){
		foreach(StudentsGroup* group, year->groupsList){
			QSet<Activity*> actsGroup=activitiesForGroup.value(group, QSet<Activity*>());
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				QSet<Activity*> actsSubgroup=activitiesForSubgroup.value(subgroup, QSet<Activity*>());
				actsGroup.unite(actsSubgroup);
			}
			activitiesForGroup.insert(group, actsGroup);
		}
	}
	//phase 2b
	foreach(StudentsYear* year, gt.rules.yearsList){
		QSet<Activity*> actsYear=activitiesForYear.value(year, QSet<Activity*>());
		foreach(StudentsGroup* group, year->groupsList){
			QSet<Activity*> actsGroup=activitiesForGroup.value(group, QSet<Activity*>());
			actsYear.unite(actsGroup);
		}
		activitiesForYear.insert(year, actsYear);
	}
	
	allActivities.clear();
	allHours.clear();
	
	foreach(StudentsYear* year, allYears){
		QSet<Activity*> acts=activitiesForYear.value(year, QSet<Activity*>());
		int n=0, d=0;
		foreach(Activity* act, acts){
			n++;
			d+=act->duration;
		}
		assert(!allActivities.contains(year->name));
		assert(!allHours.contains(year->name));
		allActivities.insert(year->name, n);
		allHours.insert(year->name, d);
	}
	
	foreach(StudentsGroup* group, allGroups){
		QSet<Activity*> acts=activitiesForGroup.value(group, QSet<Activity*>());
		int n=0, d=0;
		foreach(Activity* act, acts){
			n++;
			d+=act->duration;
		}
		assert(!allActivities.contains(group->name));
		assert(!allHours.contains(group->name));
		allActivities.insert(group->name, n);
		allHours.insert(group->name, d);
	}

	foreach(StudentsSubgroup* subgroup, allSubgroups){
		QSet<Activity*> acts=activitiesForSubgroup.value(subgroup, QSet<Activity*>());
		int n=0, d=0;
		foreach(Activity* act, acts){
			n++;
			d+=act->duration;
		}
		assert(!allActivities.contains(subgroup->name));
		assert(!allHours.contains(subgroup->name));
		allActivities.insert(subgroup->name, n);
		allHours.insert(subgroup->name, d);
	}
	
/*
	QSet<QString> allStudentsSets;

	allStudentsSets.clear();
	foreach(StudentsYear* year, gt.rules.yearsList){
		allStudentsSets.insert(year->name);
		foreach(StudentsGroup* group, year->groupsList){
			allStudentsSets.insert(group->name);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				allStudentsSets.insert(subgroup->name);
			}
		}
	}
	///////////
	
	///////////
	allHours.clear();
	allActivities.clear();
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Computing students statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Computing ... please wait"));
	progress.setRange(0, allStudentsSets.count());
	progress.setModal(true);
						
	int ttt=0;
							
	foreach(QString set, allStudentsSets){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(this, tr("FET information"), tr("Canceled"));
			showYearsCheckBox->setDisabled(true);
			showGroupsCheckBox->setDisabled(true);
			showSubgroupsCheckBox->setDisabled(true);
			showCompleteStructureCheckBox->setDisabled(true);
			return;
		}
		ttt++;
	
		relatedSubgroups.clear();
		
		relatedGroups.clear();
		
		relatedYears.clear();
		
		related.clear();
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			bool y=false;
			if(year->name==set)
				y=true;
			if(y)
				relatedYears.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				bool g=false;
				if(group->name==set)
					g=true;
				if(y || g)
					relatedGroups.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(y || g || subgroup->name==set)
						relatedSubgroups.insert(subgroup->name);
				}
			}
		}
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			if(relatedYears.contains(year->name))
				related.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				if(relatedGroups.contains(group->name)){
					related.insert(year->name);
					related.insert(group->name);
				}
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(relatedSubgroups.contains(subgroup->name)){
						related.insert(year->name);
						related.insert(group->name);
						related.insert(subgroup->name);
					}
				}
			}
		}
		
		int nh=0;
		int na=0;
		
		foreach(Activity* act, gt.rules.activitiesList) if(act->active){
			foreach(QString _students, act->studentsNames){
				if(related.contains(_students)){
					nh += act->duration;
					na ++;
					
					break;
				}
			}
		}
		
		allHours.insert(set, nh);
		allActivities.insert(set, na);
	}
	////////////

	progress.setValue(allStudentsSets.count());
*/
	
	checkBoxesModified();
}

StudentsStatisticsForm::~StudentsStatisticsForm()
{
	saveFETDialogGeometry(this);
}

void StudentsStatisticsForm::checkBoxesModified()
{
	bool complete=showCompleteStructureCheckBox->isChecked();
	
	QSet<QString> setOfStudents;

	setOfStudents.clear();
	int nStudentsSets=0;
	foreach(StudentsYear* year, gt.rules.yearsList){
		bool sy=true;
		if(!complete){
			if(setOfStudents.contains(year->name))
				sy=false;
			else
				setOfStudents.insert(year->name);
		}
		if(showYearsCheckBox->isChecked() && sy)
			nStudentsSets++;
		foreach(StudentsGroup* group, year->groupsList){
			bool sg=true;
			if(!complete){
				if(setOfStudents.contains(group->name))
					sg=false;
				else
					setOfStudents.insert(group->name);
			}
			if(showGroupsCheckBox->isChecked() && sg)
				nStudentsSets++;
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				bool ss=true;
				if(!complete){
					if(setOfStudents.contains(subgroup->name))
						ss=false;
					else
						setOfStudents.insert(subgroup->name);
				}
				if(showSubgroupsCheckBox->isChecked() && ss)
					nStudentsSets++;
			}
		}
	}
	
	tableWidget->clear();
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(nStudentsSets);
	
	QStringList columns;
	columns<<tr("Students set");
	columns<<tr("No. of activities");
	columns<<tr("Duration");
	
	tableWidget->setHorizontalHeaderLabels(columns);
	
	setOfStudents.clear();
	
	int currentStudentsSet=-1;
	foreach(StudentsYear* year, gt.rules.yearsList){
		bool sy=true;
		if(!complete){
			if(setOfStudents.contains(year->name))
				sy=false;
			else
				setOfStudents.insert(year->name);
		}

		if(showYearsCheckBox->isChecked() && sy){
			currentStudentsSet++;
			insertStudentsSet(year, currentStudentsSet);
		}
				
		foreach(StudentsGroup* group, year->groupsList){
			bool sg=true;
			if(!complete){
				if(setOfStudents.contains(group->name))
					sg=false;
				else
					setOfStudents.insert(group->name);
			}

			if(showGroupsCheckBox->isChecked() && sg){
				currentStudentsSet++;
				insertStudentsSet(group, currentStudentsSet);
			}
			
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				bool ss=true;
				if(!complete){
					if(setOfStudents.contains(subgroup->name))
						ss=false;
					else
						setOfStudents.insert(subgroup->name);
				}

				if(showSubgroupsCheckBox->isChecked() && ss){
					currentStudentsSet++;
					insertStudentsSet(subgroup, currentStudentsSet);
				}
			}	
		}
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

void StudentsStatisticsForm::insertStudentsSet(StudentsSet* set, int row)
{
	QTableWidgetItem* newItem=new QTableWidgetItem(set->name);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 0, newItem);

	int nSubActivities=0;
	int nHours=0;
	
	if(allHours.contains(set->name))
		nHours=allHours.value(set->name);
	else
		assert(0);
		
	if(allActivities.contains(set->name))
		nSubActivities=allActivities.value(set->name);
	else
		assert(0);
		
	newItem=new QTableWidgetItem(CustomFETString::number(nSubActivities));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 1, newItem);

	newItem=new QTableWidgetItem(CustomFETString::number(nHours));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 2, newItem);
}

void StudentsStatisticsForm::on_helpPushButton_clicked()
{
	QString s;
	
	s+=tr("The check boxes '%1', '%2' and '%3': they permit you to show/hide information related to years, groups or subgroups")
	 .arg(tr("Show years"))
	 .arg(tr("Show groups"))
	 .arg(tr("Show subgroups"));
	
	s+="\n\n";
	
	s+=tr("The check box '%1': it has effect only if you have overlapping groups/years, and means that FET will show the complete tree structure"
	 ", even if that means that some subgroups/groups will appear twice or more in the table, with the same information."
	 " For instance, if you have year Y1, groups G1 and G2, subgroups S1, S2, S3, with structure: Y1 (G1 (S1, S2), G2 (S1, S3)),"
	 " S1 will appear twice in the table with the same information attached").arg(tr("Show duplicates"));
	
	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
