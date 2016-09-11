/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      activityplanning.cpp  -  description
                             -------------------
    begin                : Dec 2009
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

// Code contributed by Volker Dirr ( http://www.timetabling.de/ )
// Many thanks to Liviu Lalescu. He told me some speed optimizations.

#include <QtGlobal>

#include "activityplanningform.h"
#include "statisticsexport.h"

#include "timetable.h"
#include "timetable_defs.h"

// BE CAREFUL: DON'T USE INTERNAL VARIABLES HERE, because maybe computeInternalStructure() is not done!

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QInputDialog>

#include <QToolButton>

#include <QTableWidget>
#include <QHeaderView>

#include <QKeySequence>

#include "activitiesform.h"
#include "addactivityform.h"
#include "subactivitiesform.h"
#include "modifyactivityform.h"
#include "modifysubactivityform.h"
#include "longtextmessagebox.h"

#include "sparseitemmodel.h"

#include <QString>
#include <QStringList>
#include <QHash>
#include <QMultiHash>
#include <QMap>
#include <QSet>

#include <QRadioButton>
#include <QCheckBox>

#include <QList>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

const QString RBActivityState="/activity-radio-button-state";
const QString RBSubactivityState="/subactivity-radio-button-state";
const QString RBAddState="/add-activity-radio-button-state";
const QString RBModifyState="/modify-activity-radio-button-state";
const QString RBDeleteState="/delete-activities-radio-button-state";
////
const QString CBActiveState="/active-combo-box-state";
////
const QString showYearsState="/show-years-check-box-state";
const QString showGroupsState="/show-groups-check-box-state";
const QString showSubgroupsState="/show-subgroups-check-box-state";
const QString showTeachersState="/show-teachers-check-box-state";
const QString showActivityTagsState="/show-activity-tags-check-box-state";
const QString showDuplicatesState="/show-duplicates-check-box-state";
const QString hideEmptyLinesState="/hide-empty-lines-check-box-state";
const QString swapAxisState="/swap-axes-check-box-state";
const QString hideFullTeachersState="/hide-full-teachers-state";

//maybe put following in timetable_defs.h? (start)
const int IS_YEAR = 1;
const int IS_GROUP = 2;
const int IS_SUBGROUP = 3;

const int ACTIVE_ONLY = 0;
const int INACTIVE_ONLY = 1;
const int ACTIVE_OR_INACTIVE = 2;
//maybe put following in timetable_defs.h? (end)

extern Timetable gt;

static FetStatistics statisticValues;			//maybe TODO: do it more local
static QList<bool> studentsDuplicates;		//maybe TODO: do it more local
static QList<int> yearORgroupORsubgroup;	//maybe TODO: do it more local

//TODO: need to setDefaultValue for the QHash-s ? (sum/number of hours) (also in statisticsexport?) looks like it is unneeded.
//TODO: check with toggled
//TODO: retry mouseTracking (still in source. search "mouseTracking"). check: move mouse one last visibile line. is header always highlighted, under all operating systems?!
//TODO: update if a new teacher, subject or year/group/subgroup is added - or better: just disalow that?!
//      write a "updateBasic"-function with source from constructor if you want to update/care about teacher, subject and students
//TODO: add a new row in teachers table with "comments" - of course saving data is needed to be useful
//MAYBE TODO: display "related" activities. so display all activities of a subgroup also in its related year and group. Problem: Memory and speed!
//            ! I don't like this feature, because using this activities table will be much more difficult, because it mean each cell contains (normaly) more than 1 activity!
//            ! So all shortcuts (especialy delete, add and modify) will be useless!
//            ! The table will be very full, so you will lose clear view!

static QList<int> teachersTargetNumberOfHours;
static QList<Teacher*> teachersList;

PlanningCommunicationSpinBox planningCommunicationSpinBox;

StartActivityPlanning::StartActivityPlanning()
{
}

StartActivityPlanning::~StartActivityPlanning()
{
}

void StartActivityPlanning::startActivityPlanning(QWidget* parent){
	assert(gt.rules.initialized);
	teachersTargetNumberOfHours.clear();
	teachersList.clear();
	
	statisticValues.allStudentsNames.clear();
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();
	
	QSet<QString> allStudentsNamesSet;
	
	foreach(StudentsYear* sty, gt.rules.yearsList){
		if(allStudentsNamesSet.contains(sty->name)){
			studentsDuplicates<<true;
		} else {
			studentsDuplicates<<false;
		}
		statisticValues.allStudentsNames<<sty->name;
		allStudentsNamesSet.insert(sty->name);
		yearORgroupORsubgroup<<IS_YEAR;
		foreach(StudentsGroup* stg, sty->groupsList){
			if(allStudentsNamesSet.contains(stg->name)){
				studentsDuplicates<<true;
			} else {
				studentsDuplicates<<false;
			}
			statisticValues.allStudentsNames<<stg->name;
			allStudentsNamesSet.insert(stg->name);
			yearORgroupORsubgroup<<IS_GROUP;
			if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING) foreach(StudentsSubgroup* sts, stg->subgroupsList){
				if(allStudentsNamesSet.contains(sts->name)){
					studentsDuplicates<<true;
				} else {
					studentsDuplicates<<false;
				}
				statisticValues.allStudentsNames<<sts->name;
				allStudentsNamesSet.insert(sts->name);
				yearORgroupORsubgroup<<IS_SUBGROUP;
			}
		}
	}
	
	statisticValues.allTeachersNames.clear();				// just needed, because i don't need to care about correct iTeacherList if i do it this way.
	foreach(Teacher* t, gt.rules.teachersList){		//  So i don't need gt.rules.computeInternalStructure();
		statisticValues.allTeachersNames << t->name;
		teachersTargetNumberOfHours << t->targetNumberOfHours;
		teachersList<<t;
	}

	statisticValues.allSubjectsNames.clear();				// just done, because i always want to do it the same way + it is faster
	foreach(Subject* s, gt.rules.subjectsList){		// Also don't display empty subjects is easier
		statisticValues.allSubjectsNames<<s->name;
	}
	
	ActivityPlanningForm apfd(parent);
	setParentAndOtherThings(&apfd, parent);
	apfd.exec();
	
	statisticValues.allStudentsNames.clear();
	statisticValues.allTeachersNames.clear();
	statisticValues.allSubjectsNames.clear();
	
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();

	statisticValues.teachersTotalNumberOfHours.clear();
	statisticValues.teachersTotalNumberOfHours2.clear();
	statisticValues.studentsTotalNumberOfHours.clear();
	statisticValues.studentsTotalNumberOfHours2.clear();
	statisticValues.subjectsTotalNumberOfHours.clear();
	statisticValues.subjectsTotalNumberOfHours4.clear();
}

// this is very similar to statisticsexport.cpp. so please also check there if you change something here!
ActivityPlanningForm::ActivityPlanningForm(QWidget *parent): QDialog(parent)
{
	this->setWindowTitle(tr("Activity Planning Dialog"));
	
	QHBoxLayout* wholeDialog=new QHBoxLayout(this);
	
	leftSplitter=new QSplitter(Qt::Vertical);
	//leftSplitter->setChildrenCollapsible(false);
	
	activitiesTableView= new SparseTableView;

	teachersTableView= new SparseTableView;

	leftSplitter->addWidget(activitiesTableView);
	leftSplitter->addWidget(teachersTableView);

	leftSplitter->setStretchFactor(0, 10);
	leftSplitter->setStretchFactor(1, 4);
	leftSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* rightDialog=new QVBoxLayout();
	
	/*QGroupBox**/ actionsBox=new QGroupBox(tr("Action", "It means the kind of action the user selects"));
	QVBoxLayout* actionsBoxVertical=new QVBoxLayout();
	RBActivity= new QRadioButton(tr("Activities", "Please keep translation short"));
	RBSubactivity= new QRadioButton(tr("Subactivities", "Please keep translation short"));
	RBAdd= new QRadioButton(tr("Add activity", "Please keep translation short"));
	RBModify= new QRadioButton(tr("Modify activity", "Please keep translation short"));
	RBDelete= new QRadioButton(tr("Delete activities", "Please keep translation short"));

	actionsBoxVertical->addWidget(RBActivity);
	actionsBoxVertical->addWidget(RBSubactivity);
	actionsBoxVertical->addWidget(RBAdd);
	actionsBoxVertical->addWidget(RBModify);
	actionsBoxVertical->addWidget(RBDelete);
	RBActivity->setChecked(true);
	actionsBox->setLayout(actionsBoxVertical);
	
	/*QGroupBox**/ optionsBox=new QGroupBox(tr("Options"));
	QVBoxLayout* optionsBoxVertical=new QVBoxLayout();
	QStringList activeStrings;
	//please do not modify order for these below, as the current combobox index is saved in settings and restored
	activeStrings<<tr("Active", "It refers to active activities. Please keep translation short")
		<<tr("Inactive", "It refers to inactive activities. Please keep translation short")
		<<tr("All", "It refers to all activities. Please keep translation short");
	CBActive=new QComboBox();
	CBActive->addItems(activeStrings);
	CBActive->setCurrentIndex(ACTIVE_ONLY);
	showYears=new QCheckBox(tr("Show years", "Please keep translation short"));
	showYears->setChecked(true);
	showGroups=new QCheckBox(tr("Show groups", "Please keep translation short"));
	showGroups->setChecked(true);
	showSubgroups=new QCheckBox(tr("Show subgroups", "Please keep translation short"));
	showSubgroups->setChecked(false);
	showSubgroups->setEnabled(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	showTeachers=new QCheckBox(tr("Show teachers", "Please keep translation short"));
	showTeachers->setChecked(true);
	//showActivityTags=new QCheckBox(tr("Show activity tags", "Please keep translation short"));
	showActivityTags=new QCheckBox(tr("Show tags", "Please keep translation short. It refers to activity tags, but I want to keep it shorter, because there is not enough space."));
	showActivityTags->setChecked(false);
	showDuplicates=new QCheckBox(tr("Show duplicates", "Please keep translation short"));
	showDuplicates->setChecked(false);
	hideEmptyLines=new QCheckBox(tr("Hide empty lines", "Please keep translation short"));
	hideEmptyLines->setChecked(false);
	hideUsedTeachers=new QCheckBox(tr("Hide full teachers", "It refers to teachers who have their number of hours fulfilled. Please keep translation short"));
	hideUsedTeachers->setChecked(false); //important to not hide teachers without activities, if target number of hours is deactivated
	swapAxis=new QCheckBox(tr("Swap axes", "Please keep translation short"));
	swapAxis->setChecked(false);
	pbPseudoActivities=new QPushButton(tr("Pseudo activities", "Please keep translation short. It means activities without teachers and/or students sets"));
	//set auto default all buttons, so that the user can press Enter on a cell and have the action for the cell, not the auto default button
	pbPseudoActivities->setAutoDefault(false);
	pbDeleteAll=new QPushButton(tr("Delete all", "Refers to activities. Please keep translation short"));
	pbDeleteAll->setAutoDefault(false);
	//pbHelp=new QPushButton(tr("Help", "Please keep translation short"));
	//pbHelp->setAutoDefault(false);
	
	showHideButton=new QToolButton;
	showHideButton->setText(tr("Hide buttons", "Please keep translation short. This is an option to hide some buttons in the planning activity dialog, so that the tables are more visible."));
	showHideButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
	origShowHideSizePolicy=showHideButton->sizePolicy();
	/*cout<<"hpol=="<<showHidePushButton->sizePolicy().horizontalPolicy()<<" ";
	cout<<"vpol=="<<showHidePushButton->sizePolicy().verticalPolicy()<<endl;*/
	showHideButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	
	pbClose=new QPushButton(tr("Close", "Please keep translation short"));
	pbClose->setAutoDefault(false);
	
	optionsBoxVertical->addWidget(CBActive);
	optionsBoxVertical->addWidget(showYears);
	optionsBoxVertical->addWidget(showGroups);
	optionsBoxVertical->addWidget(showSubgroups);
	optionsBoxVertical->addWidget(showTeachers);
	optionsBoxVertical->addWidget(showActivityTags);
	optionsBoxVertical->addWidget(showDuplicates);
	optionsBoxVertical->addWidget(hideEmptyLines);
	optionsBoxVertical->addWidget(hideUsedTeachers);
	optionsBoxVertical->addWidget(swapAxis);
	//optionsBoxVertical->addWidget(pbPseudoActivities);
	optionsBox->setLayout(optionsBoxVertical);
	
	rightDialog->addWidget(actionsBox);
	rightDialog->addWidget(optionsBox);
	rightDialog->addWidget(pbPseudoActivities);
	rightDialog->addStretch();
	rightDialog->addWidget(pbDeleteAll);
	rightDialog->addStretch();
	//rightDialog->addWidget(pbHelp);
	rightDialog->addWidget(showHideButton);
	rightDialog->addStretch();
	rightDialog->addWidget(pbClose);
	
	wholeDialog->addWidget(leftSplitter);
	wholeDialog->addLayout(rightDialog);
	
	teachersTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	activitiesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	//maybe disable AlternatingColors as soon as mouseTracking work correct?!
	activitiesTableView->setAlternatingRowColors(true);

	teachersTableView->setAlternatingRowColors(true); //by Liviu
	//mouseTracking (start 1/3)
	/*
	activitiesTable->setMouseTracking(true);
	teachersTable->setMouseTracking(true);
	*/
	//mouseTracking (end 1/3)
	
	int hh=560;
	if(hh<this->minimumSizeHint().height())
		hh=this->minimumSizeHint().height();
	this->resize(840, hh);
	
	buttonsVisible=true;

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		leftSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	////////
	if(settings.contains(this->metaObject()->className()+QString("/buttons-visible"))){
		bool bv=settings.value(this->metaObject()->className()+QString("/buttons-visible")).toBool();
		if(bv==false){
			showHide();
		}
	}
	//restore other settings
	if(settings.contains(this->metaObject()->className()+RBActivityState))
		RBActivity->setChecked(settings.value(this->metaObject()->className()+RBActivityState).toBool());
	if(settings.contains(this->metaObject()->className()+RBSubactivityState))
		RBSubactivity->setChecked(settings.value(this->metaObject()->className()+RBSubactivityState).toBool());
	if(settings.contains(this->metaObject()->className()+RBAddState))
		RBAdd->setChecked(settings.value(this->metaObject()->className()+RBAddState).toBool());
	if(settings.contains(this->metaObject()->className()+RBModifyState))
		RBModify->setChecked(settings.value(this->metaObject()->className()+RBModifyState).toBool());
	if(settings.contains(this->metaObject()->className()+RBDeleteState))
		RBDelete->setChecked(settings.value(this->metaObject()->className()+RBDeleteState).toBool());
	//
	if(settings.contains(this->metaObject()->className()+CBActiveState))
		CBActive->setCurrentIndex(settings.value(this->metaObject()->className()+CBActiveState).toInt());
	//
	if(settings.contains(this->metaObject()->className()+showYearsState))
		showYears->setChecked(settings.value(this->metaObject()->className()+showYearsState).toBool());
	if(settings.contains(this->metaObject()->className()+showGroupsState))
		showGroups->setChecked(settings.value(this->metaObject()->className()+showGroupsState).toBool());
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		if(settings.contains(this->metaObject()->className()+showSubgroupsState))
			showSubgroups->setChecked(settings.value(this->metaObject()->className()+showSubgroupsState).toBool());
	if(settings.contains(this->metaObject()->className()+showTeachersState))
		showTeachers->setChecked(settings.value(this->metaObject()->className()+showTeachersState).toBool());
	if(settings.contains(this->metaObject()->className()+showActivityTagsState))
		showActivityTags->setChecked(settings.value(this->metaObject()->className()+showActivityTagsState).toBool());
	if(settings.contains(this->metaObject()->className()+showDuplicatesState))
		showDuplicates->setChecked(settings.value(this->metaObject()->className()+showDuplicatesState).toBool());
	if(settings.contains(this->metaObject()->className()+hideEmptyLinesState))
		hideEmptyLines->setChecked(settings.value(this->metaObject()->className()+hideEmptyLinesState).toBool());
	if(settings.contains(this->metaObject()->className()+swapAxisState))
		swapAxis->setChecked(settings.value(this->metaObject()->className()+swapAxisState).toBool());
	if(settings.contains(this->metaObject()->className()+hideFullTeachersState))
		hideUsedTeachers->setChecked(settings.value(this->metaObject()->className()+hideFullTeachersState).toBool());
	
	//connect(activitiesTableView, SIGNAL(cellClicked(int, int)), this, SLOT(activitiesCellSelected(int, int)));
	connect(activitiesTableView, SIGNAL(activated(const QModelIndex&)), this, SLOT(activitiesCellSelected(const QModelIndex&)));
	
	//connect(activitiesTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(ActivtiesCellSelected(int, int)));

	//connect(teachersTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(teachersCellSelected(QTableWidgetItem*)));
	connect(teachersTableView, SIGNAL(activated(const QModelIndex&)), this, SLOT(teachersCellSelected(const QModelIndex&)));

	//mouseTracking (start 2/3)
	/*
	connect(activitiesTable, SIGNAL(cellEntered(int, int)), this, SLOT(ActivitiesCellEntered(int, int)));
	connect(teachersTable, SIGNAL(cellEntered(int, int)), this, SLOT(TeachersCellEntered(int, int)));
	*/
	//mouseTracking (end 2/3)
	
	connect(activitiesTableView->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(activitiesTableHorizontalHeaderClicked(int)));
	connect(activitiesTableView->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(activitiesTableVerticalHeaderClicked(int)));

	connect(teachersTableView->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(teachersTableHorizontalHeaderClicked(int)));
	
	connect(CBActive, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTables(int)));
	connect(showDuplicates, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showYears, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showGroups, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		connect(showSubgroups, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showTeachers, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(showActivityTags, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(hideEmptyLines, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(hideUsedTeachers, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(swapAxis, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(pbDeleteAll, SIGNAL(clicked()), this, SLOT(deleteAll()));
	connect(pbPseudoActivities, SIGNAL(clicked()), this, SLOT(pseudoActivities()));
	//connect(pbHelp, SIGNAL(clicked()), this, SLOT(help()));
	connect(showHideButton, SIGNAL(clicked()), this, SLOT(showHide()));
	connect(pbClose, SIGNAL(clicked()), this, SLOT(close()));
	
	connect(&planningCommunicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTables(int)));

	updateTables(0);
}

ActivityPlanningForm::~ActivityPlanningForm()
{
	saveFETDialogGeometry(this);

	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), leftSplitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/buttons-visible"), buttonsVisible);

	//save other settings
	settings.setValue(this->metaObject()->className()+RBActivityState, RBActivity->isChecked());
	settings.setValue(this->metaObject()->className()+RBSubactivityState, RBSubactivity->isChecked());
	settings.setValue(this->metaObject()->className()+RBAddState, RBAdd->isChecked());
	settings.setValue(this->metaObject()->className()+RBModifyState, RBModify->isChecked());
	settings.setValue(this->metaObject()->className()+RBDeleteState, RBDelete->isChecked());
	//
	settings.setValue(this->metaObject()->className()+CBActiveState, CBActive->currentIndex());
	//
	settings.setValue(this->metaObject()->className()+showYearsState, showYears->isChecked());
	settings.setValue(this->metaObject()->className()+showGroupsState, showGroups->isChecked());
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		settings.setValue(this->metaObject()->className()+showSubgroupsState, showSubgroups->isChecked());
	settings.setValue(this->metaObject()->className()+showTeachersState, showTeachers->isChecked());
	settings.setValue(this->metaObject()->className()+showActivityTagsState, showActivityTags->isChecked());
	settings.setValue(this->metaObject()->className()+showDuplicatesState, showDuplicates->isChecked());
	settings.setValue(this->metaObject()->className()+hideEmptyLinesState, hideEmptyLines->isChecked());
	settings.setValue(this->metaObject()->className()+swapAxisState, swapAxis->isChecked());
	settings.setValue(this->metaObject()->className()+hideFullTeachersState, hideUsedTeachers->isChecked());
}

void ActivityPlanningForm::showHide()
{
	/*cout<<"hpol=="<<showHidePushButton->sizePolicy().horizontalPolicy()<<" ";
	cout<<"vpol=="<<showHidePushButton->sizePolicy().verticalPolicy()<<endl;*/

	if(buttonsVisible){
		pbPseudoActivities->hide();
		pbDeleteAll->hide();
		showHideButton->setText(tr("B", "Abbreviation for 'Buttons'. Please keep translation very short (probably only one letter)."
			" There is an option to hide the buttons in the planning activity dialog, so that the tables can have larger visible parts."
			" This button (B) will remain there after the user hides the other buttons. The user then can press this button to show again all the buttons."
			" We need this 'B' button to be very small, so keep translation abbreviated to the initial only"));
		pbClose->hide();
		
		optionsBox->hide();
		actionsBox->hide();

		showHideButton->setSizePolicy(origShowHideSizePolicy);
		
		buttonsVisible=false;
	}
	else{
		pbPseudoActivities->show();
		pbDeleteAll->show();
		showHideButton->setText(tr("Hide buttons", "Please keep translation short. This is an option to hide some buttons in the planning activity dialog, so that the tables are more visible."));
		pbClose->show();

		optionsBox->show();
		actionsBox->show();

		showHideButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

		buttonsVisible=true;
	}
}

void ActivityPlanningForm::computeActivitiesForDeletion(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
	const QList<int>& tmpID, const QList<int>& tmpGroupID,
	int& nTotalActsDeleted,
	QList<int>& _idsToBeRemoved, QList<int>& _agidsToBeRemoved,
	QList<bool>& _affectOtherTeachersToBeRemoved, bool& _affectOtherTeachersOverall,
	QList<bool>& _affectOtherStudentsSetsToBeRemoved, bool& _affectOtherStudentsSetsOverall,
	QList<bool>& _affectOtherSubjectsToBeRemoved, bool& _affectOtherSubjectsOverall)
{
	QHash<int, int> representantForId;
	QHash<int, int> numberOfSubactivitiesForRepresentant;
	QSet<int> affectStatusForRepresentantTeacher; //if in set, it affects
	QSet<int> affectStatusForRepresentantStudent; //if in set, it affects
	QSet<int> affectStatusForRepresentantSubject; //if in set, it affects
	foreach(Activity* act, gt.rules.activitiesList){
		int id=act->id;
		
		int agid=act->activityGroupId;
		if(agid==0)
			agid=id;
		
		assert(!representantForId.contains(id));
		representantForId.insert(id, agid);
		
		int c=numberOfSubactivitiesForRepresentant.value(agid, 0);
		c++;
		numberOfSubactivitiesForRepresentant.insert(agid, c);
		
		if(teacherName!=""){
			bool af=true;
			if(act->teachersNames.count()==1)
				if(act->teachersNames.at(0)==teacherName)
					af=false;
					
			if( af && !affectStatusForRepresentantTeacher.contains(agid) )
				affectStatusForRepresentantTeacher.insert(agid);
		}
		if(studentsSetName!=""){
			bool af=true;
			if(act->studentsNames.count()==1)
				if(act->studentsNames.at(0)==studentsSetName)
					af=false;
					
			if( af && !affectStatusForRepresentantStudent.contains(agid) )
				affectStatusForRepresentantStudent.insert(agid);
		}
		if(subjectName!=""){
			bool af=true;
			if(act->subjectName==subjectName)
				af=false;
				
			if( af && !affectStatusForRepresentantSubject.contains(agid) )
				affectStatusForRepresentantSubject.insert(agid);
		}
	}
	
	QSet<int> representantCounted;

	nTotalActsDeleted=0;
	
	_idsToBeRemoved.clear();
	_agidsToBeRemoved.clear();
	
	_affectOtherTeachersToBeRemoved.clear();
	_affectOtherTeachersOverall=false;
	
	_affectOtherStudentsSetsToBeRemoved.clear();
	_affectOtherStudentsSetsOverall=false;
	
	_affectOtherSubjectsToBeRemoved.clear();
	_affectOtherSubjectsOverall=false;

	assert(tmpID.count()==tmpGroupID.count());
	
	for(int i=0; i<tmpID.count(); i++){
		int id=tmpID.at(i);
		assert(representantForId.contains(id));
		int repr=representantForId.value(id);
		
		if(!representantCounted.contains(repr)){
			representantCounted.insert(repr);
			
			assert(numberOfSubactivitiesForRepresentant.contains(repr));
			int n=numberOfSubactivitiesForRepresentant.value(repr);
			assert(n>0);
			nTotalActsDeleted+=n;
			
			_idsToBeRemoved.append(id);
			_agidsToBeRemoved.append(tmpGroupID.at(i));
			
			if(affectStatusForRepresentantTeacher.contains(repr)){
				_affectOtherTeachersToBeRemoved.append(true);
				_affectOtherTeachersOverall=true;
			}
			else{
				_affectOtherTeachersToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentantStudent.contains(repr)){
				_affectOtherStudentsSetsToBeRemoved.append(true);
				_affectOtherStudentsSetsOverall=true;
			}
			else{
				_affectOtherStudentsSetsToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentantSubject.contains(repr)){
				_affectOtherSubjectsToBeRemoved.append(true);
				_affectOtherSubjectsOverall=true;
			}
			else{
				_affectOtherSubjectsToBeRemoved.append(false);
			}
		}
	}

	assert(_idsToBeRemoved.count()==_agidsToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherTeachersToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherStudentsSetsToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherSubjectsToBeRemoved.count());
}

void ActivityPlanningForm::activitiesTableHorizontalHeaderClicked(int column){
	if(swapAxis->checkState()==Qt::Checked){
		if(column<0 || column>=statisticValues.allStudentsNames.count()){
			return;
		}
	}
	else{
		if(column<0 || column>=statisticValues.allSubjectsNames.count()){
			return;
		}
	}
	if(RBActivity->isChecked()){
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			SubactivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			SubactivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are to many activities. So just entering the activity form
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=-1;
		int subject=column;
		if(swapAxis->checkState()==Qt::Checked){
			students=column;
			subject=-1;
		}
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE) ){
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					foreach(QString st, act->studentsNames){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}
		
		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());
		
		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[column], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[column]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", "", statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[column]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::activitiesTableVerticalHeaderClicked(int row){
	if(swapAxis->checkState()==Qt::Checked){
		if(row<0 || row>=statisticValues.allSubjectsNames.count()){
			return;
		}
	}
	else{
		if(row<0 || row>=statisticValues.allStudentsNames.count()){
			return;
		}
	}
	if(RBActivity->isChecked()){
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			SubactivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			SubactivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			AddActivityForm addActivityForm(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();	
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are to many activities. So just entering the activity form
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=row;
		int subject=-1;
		if(swapAxis->checkState()==Qt::Checked){
			students=-1;
			subject=row;
		}
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					foreach(QString st, act->studentsNames){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());

		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", "", statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[row]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[row], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[row]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::activitiesCellSelected(const QModelIndex& index){
	if(!index.isValid())
		return;

	int row=index.row();
	int column=index.column();

	int students=row;
	int subject=column;
	if(swapAxis->checkState()==Qt::Checked){
		students=column;
		subject=row;
	}
	if(RBActivity->isChecked()){
		ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&addActivityForm, this);
		addActivityForm.exec();
	} else if(RBModify->isChecked()) {
		QList<int> tmpActivities;
		QList<int> tmpSubactivities;
		QList<int> tmpAGIds;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->subjectName==statisticValues.allSubjectsNames[subject]){
					foreach(QString st, act->studentsNames){
						if(st==statisticValues.allStudentsNames[students]){
							tmpSubactivities<<ai;
							if(act->activityGroupId==0 || act->activityGroupId==act->id)
								tmpActivities<<ai;
							
							tmpAGIds<<act->activityGroupId;
							
							break;
						}
					}
				}
			}
		}
		if(tmpActivities.size()<=1 && tmpSubactivities.count()>=1){
			int agid=-1;
			bool sameAgid=true;
			foreach(int k, tmpAGIds){
				if(agid==-1)
					agid=k;
				else if(agid!=k){
					sameAgid=false;
					break;
				}
			}
			
			if(!sameAgid){
				ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
				setParentAndOtherThings(&form, this);
				form.exec();
				
				return;
			}
			
			if(tmpActivities.count()==0){ //maybe representant is inactive
				assert(tmpAGIds.count()>=1);
			
				for(int i=0; i<gt.rules.activitiesList.count(); i++){
					Activity* act=gt.rules.activitiesList.at(i);
					if(act->id==act->activityGroupId && act->activityGroupId==tmpAGIds.at(0)){
						tmpActivities.append(i);
						//tmpSubactivities.prepend(i);
						//tmpAGIds.prepend(act->activityGroupId);
						break;
					}
				}
			}
			
			assert(tmpActivities.count()==1);
			
			foreach(int tmpAct, tmpActivities){
				Activity* act=gt.rules.activitiesList[tmpAct];
				if(act->isSplit()){
					//similar to activitiesform.cpp by Liviu Lalescu(start)
					//maybe TODO: write a function int activityCheckedManualy in activity.cpp, because we use this already 3 times (me even 5 times)
					//            here, in activitiesform.cpp, in csv export and also in willi2 export and winplan export.
					QStringList _teachers=act->teachersNames;
					bool _diffTeachers=false;
				
					QString _subject=act->subjectName;
					bool _diffSubject=false;
					
					QStringList _activityTags=act->activityTagsNames;
					bool _diffActivityTags=false;
				
					QStringList _students=act->studentsNames;
					bool _diffStudents=false;
					
					int _nTotalStudents=act->nTotalStudents;
					bool _diffNTotalStudents=false;
				
					bool _computeNTotalStudents=act->computeNTotalStudents;
					bool _diffComputeNTotalStudents=false;

					int nSplit=0;
					for(int i=0; i<gt.rules.activitiesList.size(); i++){
						Activity* act2=gt.rules.activitiesList[i];
						if(act2->activityGroupId==act->activityGroupId){
							nSplit++;
							
							if(_teachers!=act2->teachersNames)
								_diffTeachers=true;
							if(_subject!=act2->subjectName)
								_diffSubject=true;
							if(_activityTags!=act2->activityTagsNames)
								_diffActivityTags=true;
							if(_students!=act2->studentsNames)
								_diffStudents=true;
							if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ _nTotalStudents!=act2->nTotalStudents )
								_diffNTotalStudents=true;
							if(_computeNTotalStudents!=act2->computeNTotalStudents)
								_diffComputeNTotalStudents=true;
						}
						if(nSplit>MAX_SPLIT_OF_AN_ACTIVITY){
							QMessageBox::warning(this, tr("FET information"),
							 tr("Cannot modify this large activity, because it contains more than %1 activities.")
							 .arg(MAX_SPLIT_OF_AN_ACTIVITY));
							return;
						}
					}
					if(_diffTeachers || _diffSubject || _diffActivityTags || _diffStudents || _diffNTotalStudents || _diffComputeNTotalStudents){
						/*QStringList s;
						if(diffTeachers)
							s.append(tr("different teachers"));
						if(diffSubject)
							s.append(tr("different subject"));
						if(diffActivityTags)
							s.append(tr("different activity tags"));
						if(diffStudents)
							s.append(tr("different students"));
						if(diffComputeNTotalStudents)
							s.append(tr("different boolean variable 'must compute n total students'"));
						if(diffNTotalStudents)
							s.append(tr("different number of students"));
							
						QString s2;
						s2+=tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
						 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
						 " from this larger split activity.");
						s2+="\n\n";
						s2+=tr("The fields which are different are: %1").arg(s.join(", "));
							
						int t=QMessageBox::warning(this, tr("FET warning"), s2, tr("Abort"), tr("Continue"), QString(), 1, 0);
						
						if(t==0)
							return;*/
							
						ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
						setParentAndOtherThings(&form, this);
						form.exec();
				
						return;
					}
				}
				ModifyActivityForm modifyActivityForm(this, act->id, act->activityGroupId);
				//int t;
				setParentAndOtherThings(&modifyActivityForm, this);
				modifyActivityForm.exec();
				//similar to activitiesform.cpp (end)
				return;
			}
		}
		//else if(tmpSubactivities.count()>=1){
		else{
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->subjectName==statisticValues.allSubjectsNames[subject]){
					foreach(QString st, act->studentsNames){
						if(st==statisticValues.allStudentsNames[students]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());
		
		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			QString s;
			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[column], statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[row]);
				s+=", ";
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[column]);
				s+=")";

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}
			else{
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[row], statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);


				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[row]);
				s+=", ";
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[column]);
				s+=")";

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}

			int ret=QMessageBox::question(this, tr("Delete activities?"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int retst=QMessageBox::No;
				if(_affectOtherStudentsOverall){
					retst=QMessageBox::question(this, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other students. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				int retsubj=QMessageBox::No;
				if(_affectOtherSubjectsOverall){
					retsubj=QMessageBox::question(this, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if( ((_affectOtherStudentsToBeRemoved.at(i)==false) || (retst==QMessageBox::Yes)) && 
						((_affectOtherSubjectsToBeRemoved.at(i)==false) || (retsubj==QMessageBox::Yes)) ){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else assert(0==1);
}

//mouseTracking (start 3/3)
/*
void ActivityPlanningForm::ActivtiesCellEntered(int row, int column){
	activitiesTable->setCurrentCell(row, column);
}

void ActivityPlanningForm::TeachersCellEntered(int row, int column){
	teachersTable->setCurrentCell(row, column);
}*/
//mouseTracking (end 3/3)

void ActivityPlanningForm::teachersTableHorizontalHeaderClicked(int column){
	if(column<0 || column>=statisticValues.allTeachersNames.count()){
		return;
	}
	if(RBActivity->isChecked()){
		ActivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&addActivityForm, this);
		addActivityForm.exec();	
	} else if(RBModify->isChecked()) {
		//Just enter the activity dialog, because in normal case there are to many activities related to a teacher.
		ActivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBDelete->isChecked()) {
		//bool affectOtherTeachers=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherTeachers;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
					foreach(QString t, act->teachersNames){
						if(t==statisticValues.allTeachersNames[column]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->teachersNames.size()>1){
									tmpAffectOtherTeachers<<true;
									affectOtherTeachers=true;
								} else {
									tmpAffectOtherTeachers<<false;
								}*/
							}
							break;
						}
					}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherTeachers.count()==tmpIdentifySet.size());

		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			computeActivitiesForDeletion(statisticValues.allTeachersNames[column], "", "",
				tmpID, tmpGroupID,
				nTotalActsDeleted,
				_idsToBeRemoved, _agidsToBeRemoved,
				_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
				_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
				_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

			QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[column]);
			assert(nTotalActsDeleted>=tmpIdentifySet.size());
			if(nTotalActsDeleted>tmpIdentifySet.size()){
				s+="\n\n";
				s+=tr("Because you have individually modified the components, there will be removed an additional number "
					"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
			}
			int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int ret2=QMessageBox::No;
				if(_affectOtherTeachersOverall){
					ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}
				
				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::teachersCellSelected(const QModelIndex& index){
	/*if(item==NULL){
		return;
	}*/
	if(!index.isValid())
		return;
	int itrow=index.row();
	int itcol=index.column();
	if(itrow==0){
		bool ok=false;
		int targetHours=QInputDialog::getInt(this, tr("FET question"), tr("Please enter the target number of hours:")+" ",
		 teachersTargetNumberOfHours.at(itcol), 0, gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek, 1, &ok);
		if(ok){
			teachersTargetNumberOfHours[itcol]=targetHours;
			
			teachersList.at(itcol)->targetNumberOfHours=targetHours;
			/*useless, because i also need to remove the table head item and i don't know how, so i redo the whole table
			teachersTable->removeCellWidget(0, itcol);
			QTableWidgetItem* newItem2=new QTableWidgetItem(CustomFETString::number(teachersTargetNumberOfHours.at(itcol)));
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);*/
			
			/*doesn't work, but idea is maybe better/faster!
			QTableWidgetItem* newItem2= teachersTable->takeItem(0, itcol);
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);
			QString tmpString=statisticValues.allTeachersNames.at(itcol)+"\n"+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol));
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(itcol))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(itcol)))
				tmpString+=" ("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol))+")";
			QTableWidgetItem* newItem3= teachersTable->takeVerticalHeaderItem(itcol);
			newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem3);*/
			
			//currently using not fast, but safe calculation
			updateTables_Teachers();
			updateTablesVisual(0);
			
			gt.rules.internalStructureComputed=false;
			setRulesModifiedAndOtherThings(&gt.rules);
		}
	} else {
		if(RBActivity->isChecked()){
			ActivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else if(RBSubactivity->isChecked()) {
			SubactivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else if(RBAdd->isChecked()) {
			AddActivityForm addActivityForm(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();	
		} else if(RBModify->isChecked()) {
			//Just enter the activity dialog, because in normal case there are to many activities related to a teacher.
			ActivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else if(RBDelete->isChecked()) {
			//bool affectOtherTeachers=false;
			QList<int> tmpID;
			QList<int> tmpGroupID;
			//QStringList tmpIdentify;
			QSet<int> tmpIdentifySet;
			//QList<bool> tmpAffectOtherTeachers;
			Activity* act;
			for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
				act=gt.rules.activitiesList[ai];
				int tmpCurrentIndex=CBActive->currentIndex();
				if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
					|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
					|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
						foreach(QString t, act->teachersNames){
							if(t==statisticValues.allTeachersNames[itcol]){
								//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								//if(!tmpIdentify.contains(tmpIdent)){
								if(!tmpIdentifySet.contains(act->id)){
									tmpID<<act->id;
									tmpGroupID<<act->activityGroupId;
									//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
									tmpIdentifySet.insert(act->id);
									/*if(act->teachersNames.size()>1){
										tmpAffectOtherTeachers<<true;
										affectOtherTeachers=true;
									} else {
										tmpAffectOtherTeachers<<false;
									}*/
								}
								break;
							}
						}
				}
			}
			assert(tmpID.count()==tmpIdentifySet.size());
			assert(tmpGroupID.count()==tmpIdentifySet.size());
			//assert(tmpAffectOtherTeachers.count()==tmpIdentifySet.size());
	
			if(!tmpIdentifySet.isEmpty()){
				int nTotalActsDeleted;
				QList<int> _idsToBeRemoved;
				QList<int> _agidsToBeRemoved;

				QList<bool> _affectOtherTeachersToBeRemoved;
				bool _affectOtherTeachersOverall;
				QList<bool> _affectOtherStudentsToBeRemoved;
				bool _affectOtherStudentsOverall;
				QList<bool> _affectOtherSubjectsToBeRemoved;
				bool _affectOtherSubjectsOverall;
	
				computeActivitiesForDeletion(statisticValues.allTeachersNames[itcol], "", "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[itcol]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherTeachersOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		} else assert(0==1);
	}
}

void ActivityPlanningForm::updateTables(int unneeded){
	Q_UNUSED(unneeded);

	updateTables_Students_Subjects();
	updateTables_Teachers();

	updateTablesVisual(0);
}

void ActivityPlanningForm::updateTables_Students_Subjects(){	//similar to statisticsexport.cpp
	QMultiHash <QString, int> studentsActivities;

	statisticValues.studentsTotalNumberOfHours.clear();
	statisticValues.studentsTotalNumberOfHours2.clear();
	statisticValues.subjectsTotalNumberOfHours.clear();
	statisticValues.subjectsTotalNumberOfHours4.clear();
	
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				//subjectsActivities.insert(act->subjectName, ai);
				int tmp=statisticValues.subjectsTotalNumberOfHours.value(act->subjectName)+act->duration;
				statisticValues.subjectsTotalNumberOfHours.insert(act->subjectName, tmp);						// (1) so teamlearning-teaching is not counted twice!
				foreach(QString st, act->studentsNames){
					studentsActivities.insert(st, ai);
					tmp=statisticValues.studentsTotalNumberOfHours.value(st)+act->duration;
					statisticValues.studentsTotalNumberOfHours.insert(st, tmp);							// (2)
					//subjectstTotalNumberOfHours3[act->subjectIndex]+=duration;				// (1) so teamlearning is counted twice!
				}
				foreach(QString st, act->studentsNames){
					tmp=statisticValues.studentsTotalNumberOfHours2.value(st);
					tmp += act->duration * act->teachersNames.count();
					statisticValues.studentsTotalNumberOfHours2.insert(st, tmp);					// (2)
				}
				tmp=statisticValues.subjectsTotalNumberOfHours4.value(act->subjectName);
				tmp += act->duration * act->studentsNames.count() * act->teachersNames.count();
				statisticValues.subjectsTotalNumberOfHours4.insert(act->subjectName, tmp);			// (1) so teamlearning-teaching is counted twice!
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());
	QStringList studentsLabels;
	QStringList subjectsLabels;
	studentsLabels.clear();
	subjectsLabels.clear();
	
	activitiesTableView->model.clearDataAndHeaders();

	if(swapAxis->checkState()==Qt::Checked){
		/*activitiesTable->setRowCount(statisticValues.allSubjectsNames.count());
		activitiesTable->setColumnCount(statisticValues.allStudentsNames.count());*/
		activitiesTableView->model.resize(statisticValues.allSubjectsNames.count(), statisticValues.allStudentsNames.count());
	} else {
		/*activitiesTable->setRowCount(statisticValues.allStudentsNames.count());
		activitiesTable->setColumnCount(statisticValues.allSubjectsNames.count());*/
		activitiesTableView->model.resize(statisticValues.allStudentsNames.count(), statisticValues.allSubjectsNames.count());
	}

	for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
		//progress2.setValue(students);
		QList<int> tmpStudents;
		QMultiHash<QString, int> tmpSubjects;
		tmpStudents.clear();
		tmpSubjects.clear();
		tmpStudents=studentsActivities.values(statisticValues.allStudentsNames.at(students));
		foreach(int aidx, tmpStudents){
			Activity* act=gt.rules.activitiesList.at(aidx);
			tmpSubjects.insert(act->subjectName, aidx);
		}
		
		QString tmpStudentsLabel="";
		tmpStudentsLabel=statisticValues.allStudentsNames.at(students);
		tmpStudentsLabel+="\n"+CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students)));
		if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))!=statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))
			tmpStudentsLabel+=" ("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))+")";
		studentsLabels<<tmpStudentsLabel;
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			QList<int> tmpActivities;
			tmpActivities.clear();
			tmpActivities=tmpSubjects.values(statisticValues.allSubjectsNames.at(subject));
			QString tmpString="";
			if(!tmpActivities.isEmpty()){
				QMap<QString, int> tmpIdentDuration;	//not QHash, because i need the correct order of the activities
				foreach(int tmpAct, tmpActivities){
					Activity* act=gt.rules.activitiesList[tmpAct];
					int tmpD=act->duration;
					QString tmpIdent;
					if(showTeachers->checkState()==Qt::Checked){
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tmpIdent+=*it;
								if(it!=act->teachersNames.end()-1)
									tmpIdent+=", ";
							}
						}
					}
					tmpIdent+=" ";
					if(showActivityTags->checkState()==Qt::Checked){
						if(act->activityTagsNames.size()>0){
							for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
								tmpIdent+=*it;
								if(it!=act->activityTagsNames.end()-1)
									tmpIdent+=", ";
							}
						}
					}
					tmpD+=tmpIdentDuration.value(tmpIdent);
					tmpIdentDuration.insert(tmpIdent, tmpD);
				}
				QMapIterator<QString, int> it(tmpIdentDuration);
				while(it.hasNext()){
					it.next();
					tmpString+=CustomFETString::number(it.value());
					tmpString+=" "+it.key();
					if(it.hasNext())
						tmpString+="\n";
				}
			}
			if(!tmpActivities.isEmpty()){
				/*QTableWidgetItem* newItem=new QTableWidgetItem(tmpString);
				newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/
				QPair<int, int> pair;
				if(swapAxis->checkState()==Qt::Checked){
					pair.first=subject;
					pair.second=students;
					//activitiesTable->setItem(subject, students, newItem);
				} else {
					pair.first=students;
					pair.second=subject;
					//activitiesTable->setItem(students, subject, newItem);
				}
				activitiesTableView->model.items.insert(pair, tmpString);
			}
		}
	}
	//progress2.setValue(statisticValues.allStudentsNames.count());
	foreach(QString subjects, statisticValues.allSubjectsNames){
		QString tmpSubjectLabel="";
		tmpSubjectLabel=subjects+"\n"+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(subjects));
		if(statisticValues.subjectsTotalNumberOfHours.value(subjects)!=statisticValues.subjectsTotalNumberOfHours4.value(subjects))
			tmpSubjectLabel+=" ("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(subjects))+")";
		subjectsLabels<<tmpSubjectLabel;
	}
	if(swapAxis->checkState()==Qt::Checked){
		/*activitiesTableView->setHorizontalHeaderLabels(studentsLabels);
		activitiesTableView->setVerticalHeaderLabels(subjectsLabels);*/
		activitiesTableView->model.horizontalHeaderItems=studentsLabels;
		activitiesTableView->model.verticalHeaderItems=subjectsLabels;
	} else {
		/*activitiesTableView->setHorizontalHeaderLabels(subjectsLabels);
		activitiesTableView->setVerticalHeaderLabels(studentsLabels);*/
		activitiesTableView->model.horizontalHeaderItems=subjectsLabels;
		activitiesTableView->model.verticalHeaderItems=studentsLabels;
	}
	
	activitiesTableView->allTableChanged();
}

void ActivityPlanningForm::updateTables_Teachers(){	//similar to statisticsexport.cpp
	QMultiHash <QString, int> teachersActivities;
	
	statisticValues.teachersTotalNumberOfHours.clear();
	statisticValues.teachersTotalNumberOfHours2.clear();
	
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				int tmp;
				foreach(QString t, act->teachersNames){
					teachersActivities.insert(t, ai);
					tmp=statisticValues.teachersTotalNumberOfHours.value(t)+act->duration;
					statisticValues.teachersTotalNumberOfHours.insert(t, tmp);							// (3)
				}
				foreach(QString t, act->teachersNames){
					tmp=statisticValues.teachersTotalNumberOfHours2.value(t);
					tmp += act->duration * act->studentsNames.count();
					statisticValues.teachersTotalNumberOfHours2.insert(t, tmp);						// (3)
				}
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());

	QStringList teachersLabels;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		QString prefixString1="";
		QString prefixString2="";
		if(teachersTargetNumberOfHours.at(teacher)!=0){
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString1="+";
			}else if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))==teachersTargetNumberOfHours.at(teacher)){
				prefixString1="=";
			}
			if(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString2="+";
			}else if(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))==teachersTargetNumberOfHours.at(teacher)){
				prefixString2="=";
			}
		}
		QString tmpString=statisticValues.allTeachersNames.at(teacher)+"\n"+prefixString1+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher));
		if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))
			tmpString+=" ("+prefixString2+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher))+")";
		teachersLabels<<tmpString;
	}
	
	teachersTableView->model.clearDataAndHeaders();

	teachersTableView->model.resize(3, statisticValues.allTeachersNames.count());
	
	QStringList vLabels;
	vLabels<<tr("Target", "Please keep translation short.")
		<<tr("Subjects", "Please keep translation short")
		<<tr("Students", "Please keep translation short");

	teachersTableView->model.verticalHeaderItems=vLabels;
	teachersTableView->model.horizontalHeaderItems=teachersLabels;
	
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		//QTableWidgetItem* newItem=new QTableWidgetItem(statisticValues.allTeachersNames[teacher]);
		//newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		/*QTableWidgetItem* newItem2=new QTableWidgetItem(CustomFETString::number(teachersTargetNumberOfHours.at(teacher)));
		newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(0, teacher, newItem2);*/
		QString str=CustomFETString::number(teachersTargetNumberOfHours.at(teacher));
		QPair<int, int> pair;
		pair.first=0;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, str);
		
		//new (start)
		QList<int> tmpActivities;
		QMap<QString, int> tmpSubjectsNumberOfHours;		//using map, because it sorts alphabetically
		QMap<QString, int> tmpStudentsNumberOfHours;		//using map, because it sorts alphabetically
		tmpActivities.clear();
		tmpActivities=teachersActivities.values(statisticValues.allTeachersNames.at(teacher));
		foreach(int aidx, tmpActivities){
			Activity* act=gt.rules.activitiesList.at(aidx);
			//students start
			int tmpD=act->duration;
			QString tmpIdent;
			if(act->studentsNames.size()>0){
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++){
					tmpIdent+=*it;
					if(it!=act->studentsNames.end()-1)
						tmpIdent+=", ";
				}
			}
			tmpIdent+=" ";
			if(showActivityTags->checkState()==Qt::Checked){
				if(act->activityTagsNames.size()>0){
					for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.end()-1)
							tmpIdent+=", ";
					}
				}
			}
			tmpD+=tmpStudentsNumberOfHours.value(tmpIdent);
			tmpStudentsNumberOfHours.insert(tmpIdent, tmpD);
			//students end
			//subject start
			tmpD=act->duration;
			tmpIdent.clear();
			tmpIdent=act->subjectName;
			tmpIdent+=" ";
			if(showActivityTags->checkState()==Qt::Checked){
				if(act->activityTagsNames.size()>0){
					for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.end()-1)
							tmpIdent+=", ";
					}
				}
			}
			tmpD+=tmpSubjectsNumberOfHours.value(tmpIdent);
			tmpSubjectsNumberOfHours.insert(tmpIdent, tmpD);
			//subject end
		}
		
		QString tmpItem;
		tmpItem.clear();
		QMapIterator<QString, int> it(tmpSubjectsNumberOfHours);
		while(it.hasNext()){
			it.next();
			tmpItem+=CustomFETString::number(it.value())+" "+it.key()+"\n";
		}
		//
		/*QTableWidgetItem* newItem3=new QTableWidgetItem(tmpItem);
		newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(1, teacher, newItem3);*/
		//
		pair.first=1;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, tmpItem);
		
		tmpItem.clear();
		QMapIterator<QString, int> it2(tmpStudentsNumberOfHours);
		while(it2.hasNext()){
			it2.next();
			tmpItem+=CustomFETString::number(it2.value())+" "+it2.key()+"\n";
		}
		//
		/*QTableWidgetItem* newItem4=new QTableWidgetItem(tmpItem);
		newItem4->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(2, teacher, newItem4);*/
		//
		pair.first=2;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, tmpItem);
		//new(end)
	}
	
	//teachersTargetNumberOfHours; Delete this lines! (start)
	//teachersTableView->hideRow(0);
	//hideUsedTeachers->hide();
	//teachersTargetNumberOfHours; Delete this lines! (end)
	
	teachersTableView->allTableChanged();
}

void ActivityPlanningForm::updateTablesVisual(int unneeded){
	Q_UNUSED(unneeded);

	assert(studentsDuplicates.count()==statisticValues.allStudentsNames.count());
	assert(studentsDuplicates.count()==yearORgroupORsubgroup.count());
	
	
	activitiesTableView->setUpdatesEnabled(false);
	teachersTableView->setUpdatesEnabled(false);

	//(I hope) no performance slowdown, because of implicit sharing
	/*QHash<QPair<int, int>, QString> tItems=activitiesTableView->model.items;
	QStringList tHorizontalHeaderItems=activitiesTableView->model.horizontalHeaderItems;
	QStringList tVerticalHeaderItems=activitiesTableView->model.verticalHeaderItems;
	int trc=activitiesTableView->model.rowCount();
	int tcc=activitiesTableView->model.columnCount();
	activitiesTableView->model.clear();
	activitiesTableView->model.resize(trc, tcc);*/

	if(swapAxis->checkState()==Qt::Checked){
		for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			if(!SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
				assert(tmpInt!=IS_SUBGROUP);
			switch(tmpInt){
				case IS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case IS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case IS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))==0 && statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showColumn(students);
			else
				activitiesTableView->hideColumn(students);
		}
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			bool show=true;
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))==0 && statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showRow(subject);
			else
				activitiesTableView->hideRow(subject);
		}
	} else {
		for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			if(!SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
				assert(tmpInt!=IS_SUBGROUP);
			switch(tmpInt){
				case IS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case IS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case IS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))==0 && statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showRow(students);
			else
				activitiesTableView->hideRow(students);
		}
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			bool show=true;
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))==0 && statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showColumn(subject);
			else
				activitiesTableView->hideColumn(subject);
		}
	}

	/*activitiesTableView->model.items=tItems;
	activitiesTableView->model.horizontalHeaderItems=tHorizontalHeaderItems;
	activitiesTableView->model.verticalHeaderItems=tVerticalHeaderItems;
	activitiesTableView->allTableChanged();*/
	
	
	//(I hope) no performance slowdown, because of implicit sharing
	/*tItems=teachersTableView->model.items;
	tHorizontalHeaderItems=teachersTableView->model.horizontalHeaderItems;
	tVerticalHeaderItems=teachersTableView->model.verticalHeaderItems;
	trc=teachersTableView->model.rowCount();
	tcc=teachersTableView->model.columnCount();
	teachersTableView->model.clear();
	teachersTableView->model.resize(trc, tcc);*/
	
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		bool show=true;
		if((teachersTargetNumberOfHours.at(teacher)-statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher)))==0
		 /*&& (teachersTargetNumberOfHours.at(teacher)-statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))==0*/
		 && hideUsedTeachers->checkState()==Qt::Checked){
			show=false;
		}
		if(show)
			teachersTableView->showColumn(teacher);
		else
			teachersTableView->hideColumn(teacher);
	}

	/*teachersTableView->model.items=tItems;
	teachersTableView->model.horizontalHeaderItems=tHorizontalHeaderItems;
	teachersTableView->model.verticalHeaderItems=tVerticalHeaderItems;
	teachersTableView->allTableChanged();*/

	
	/*activitiesTable->resizeColumnsToContents();
	activitiesTable->resizeRowsToContents();*/
	activitiesTableView->resizeToFit();

	/*teachersTable->resizeColumnsToContents();
	teachersTable->resizeRowsToContents();*/
	teachersTableView->resizeToFit();

	activitiesTableView->setUpdatesEnabled(true);
	teachersTableView->setUpdatesEnabled(true);
}

void ActivityPlanningForm::deleteAll(){
	int ret=QMessageBox::question(this, tr("Delete all?", "It refers to activities"), tr("Are you sure you want to remove ALL the %1 activities and related constraints?", "%1 is the number of total activities")
		.arg(gt.rules.activitiesList.count()), QMessageBox::Yes | QMessageBox::No);
	if(ret==QMessageBox::Yes){
		ret=QMessageBox::question(this, tr("Delete all?", "It refers to activities"), tr("Are you absolutely sure you want to remove ALL activities and related constraints from your data?"), QMessageBox::Yes | QMessageBox::No);
		if(ret==QMessageBox::Yes){
			QList<int> idsToBeRemoved;
			foreach(Activity* act, gt.rules.activitiesList)
				idsToBeRemoved.append(act->id);
			gt.rules.removeActivities(idsToBeRemoved, true);
		
			/*while(!gt.rules.activitiesList.isEmpty()){
				Activity* act=gt.rules.activitiesList.at(0);
				gt.rules.removeActivity(act->id, act->activityGroupId);
			}*/
			
			PlanningChanged::increasePlanningCommunicationSpinBox();
		}
	}
}

void ActivityPlanningForm::pseudoActivities(){
	QString noTeachersAndStudents;
	QString noTeachers;
	QString noStudents;
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		act=gt.rules.activitiesList[ai];
		if(	((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->teachersNames.isEmpty() && act->studentsNames.isEmpty()){
					noTeachersAndStudents+=act->getDescription(gt.rules)+"\n";
				} else if(act->teachersNames.isEmpty()){
					noTeachers+=act->getDescription(gt.rules)+"\n";
				} else if(act->studentsNames.isEmpty()){
					noStudents+=act->getDescription(gt.rules)+"\n";
				}
			}
	}
	if(noTeachersAndStudents.isEmpty()){
		noTeachersAndStudents=tr("There are no activities without teachers and without students.")+"\n";
	} else {
		noTeachersAndStudents=tr("Activities without teachers and without students:")+"\n"+noTeachersAndStudents;
	}
	if(noTeachers.isEmpty()){
		noTeachers="\n"+tr("There are no activities without teachers.")+"\n";
	} else {
		noTeachers="\n"+tr("Activities without teachers:")+"\n"+noTeachers;
	}
	if(noStudents.isEmpty()){
		noStudents="\n"+tr("There are no activities without students.")+"\n";
	} else {
		noStudents="\n"+tr("Activities without students:")+"\n"+noStudents;
	}
	LongTextMessageBox::mediumInformation(this, tr("Information about pseudo activities", "Pseudo activities means activities without teachers and/or students sets"), noTeachersAndStudents+noTeachers+noStudents);
}

//communication box by Liviu Lalescu

PlanningCommunicationSpinBox::PlanningCommunicationSpinBox()
{
	minValue=0;
	maxValue=9;
	value=0;
}

PlanningCommunicationSpinBox::~PlanningCommunicationSpinBox()
{
}

void PlanningCommunicationSpinBox::increaseValue()
{
	assert(maxValue>minValue);
	assert(value>=minValue && value<=maxValue);
	value++;
	if(value>maxValue)
		value=minValue;
		
	emit(valueChanged(value));
}

void PlanningChanged::increasePlanningCommunicationSpinBox()
{
	planningCommunicationSpinBox.increaseValue();
}
