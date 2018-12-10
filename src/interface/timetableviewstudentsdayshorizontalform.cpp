/***************************************************************************
                          timetableviewstudentsdayshorizontalform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QtGlobal>

#include "tablewidgetupdatebug.h"

#include "longtextmessagebox.h"
#include "centerwidgetonscreen.h"

#include "fetmainform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetable_defs.h"
#include "fetguisettings.h"

#include "timetable.h"
#include "fet.h"
#include "solution.h"

#include "timetableexport.h"

#include "matrix.h"

#include "lockunlock.h"

#include "errorrenderer.h"

#include <QList>
#include <QSet>

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QCoreApplication>
#include <QApplication>

#include <QString>
#include <QStringList>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

//begin by Marco Vassura
#include <QBrush>
#include <QColor>
//end by Marco Vassura

extern bool simulation_running;

extern Matrix3D<bool> subgroupNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;

TimetableViewStudentsDaysHorizontalForm::TimetableViewStudentsDaysHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	verticalStudentsTableDetailsSplitter->setStretchFactor(0, 4);
	verticalStudentsTableDetailsSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);
	
	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	subgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	groupsListWidget->clear();
	subgroupsListWidget->clear();
	
	//This connect should be lower in the code
	//connect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
	connect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));
	connect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(studentsTimetableTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIGNAL(clicked()), this, SLOT(lockTime()));
	connect(lockSpacePushButton, SIGNAL(clicked()), this, SLOT(lockSpace()));
	connect(lockTimeSpacePushButton, SIGNAL(clicked()), this, SLOT(lockTimeSpace()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical students list splitter state
	QSettings settings;
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")))
		verticalStudentsListSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")).toByteArray());

	//restore vertical students table details splitter state
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")))
		verticalStudentsTableDetailsSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")).toByteArray());

	//restore horizontal splitter state
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());

	LockUnlock::assertIsUpdated(&gt.rules);

	LockUnlock::increaseCommunicationSpinBox();

	studentsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
	studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		studentsTimetableTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		studentsTimetableTable->setVerticalHeaderItem(i, item);
	}
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			studentsTimetableTable->setItem(j, k, item);

			//if(j==0 && k==0)
			//	teachersTimetableTable->setCurrentItem(item);
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
	studentsTimetableTable->horizontalHeader()->setMinimumSectionSize(studentsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= 0x050000
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	///////////////
	
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		yearsListWidget->addItem(sty->name);
	}
	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
	connect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
		
	shownComboBox->addItem(tr("Years"));
	shownComboBox->addItem(tr("Groups"));
	shownComboBox->addItem(tr("Subgroups"));

	shownComboBox->setCurrentIndex(-1);

	if(settings.contains(this->metaObject()->className()+QString("/shown-categories")))
		shownComboBox->setCurrentIndex(settings.value(this->metaObject()->className()+QString("/shown-categories")).toInt());
	else
		shownComboBox->setCurrentIndex(0);

	connect(shownComboBox, SIGNAL(activated(QString)), this, SLOT(shownComboBoxChanged(QString)));

	//added by Volker Dirr
	connect(&LockUnlock::communicationSpinBox, SIGNAL(valueChanged()), this, SLOT(updateStudentsTimetableTable()));

	shownComboBoxChanged(shownComboBox->currentText());
	studentsTimetableTable->setSolution(&gt.rules, CachedSchedule::getCachedSolution());
	connect(studentsTimetableTable, SIGNAL(solution_changed()), &LockUnlock::communicationSpinBox, SLOT(increaseValue()));

	connect(studentsTimetableTable, SIGNAL(activityRemoved(int)), this, SLOT(newActivityNotPlaced(int)));
}

TimetableViewStudentsDaysHorizontalForm::~TimetableViewStudentsDaysHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical students list splitter state
	QSettings settings;
	settings.setValue(this->metaObject()->className()+QString("/vertical-students-list-splitter-state"), verticalStudentsListSplitter->saveState());

	//save vertical students table details splitter state
	settings.setValue(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state"), verticalStudentsTableDetailsSplitter->saveState());

	//save horizontal splitter state
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());
	
	settings.setValue(this->metaObject()->className()+QString("/shown-categories"), shownComboBox->currentIndex());
}

void TimetableViewStudentsDaysHorizontalForm::resizeRowsAfterShow()
{
	studentsTimetableTable->resizeRowsToContents();
//	tableWidgetUpdateBug(studentsTimetableTable);
}

void TimetableViewStudentsDaysHorizontalForm::shownComboBoxChanged(QString shownCategory)
{
	Q_UNUSED(shownCategory);

	if(shownComboBox->currentIndex()==0){
		//years, groups, and subgroups shown
		yearsListWidget->show();
		groupsListWidget->show();
		subgroupsListWidget->show();
		
		disconnect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
		yearsListWidget->clear();
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			yearsListWidget->addItem(sty->name);
		}
		if(yearsListWidget->count()>0)
			yearsListWidget->setCurrentRow(0);
		connect(yearsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(yearChanged(const QString&)));
		if(yearsListWidget->count()>0)
			yearChanged(yearsListWidget->item(0)->text());
	}
	else if(shownComboBox->currentIndex()==1){
		//only groups and subgroups shown
		yearsListWidget->hide();
		groupsListWidget->show();
		subgroupsListWidget->show();

		disconnect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));
		groupsListWidget->clear();
		QSet<QString> groupsSet;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(StudentsGroup* stg : qAsConst(sty->groupsList))
				if(!groupsSet.contains(stg->name)){
					groupsListWidget->addItem(stg->name);
					groupsSet.insert(stg->name);
				}
		}
		if(groupsListWidget->count()>0)
			groupsListWidget->setCurrentRow(0);
		connect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));
		if(groupsListWidget->count()>0)
			groupChanged(groupsListWidget->item(0)->text());
	}
	else if(shownComboBox->currentIndex()==2){
		//only subgroups shown
		yearsListWidget->hide();
		groupsListWidget->hide();
		subgroupsListWidget->show();

		disconnect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));
		subgroupsListWidget->clear();
		QSet<QString> subgroupsSet;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(StudentsGroup* stg : qAsConst(sty->groupsList))
				for(StudentsSubgroup* sts : qAsConst(stg->subgroupsList))
					if(!subgroupsSet.contains(sts->name)){
						subgroupsListWidget->addItem(sts->name);
						subgroupsSet.insert(sts->name);
					}
		}
		if(subgroupsListWidget->count()>0)
			subgroupsListWidget->setCurrentRow(0);
		connect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));
		if(subgroupsListWidget->count()>0)
			subgroupChanged(subgroupsListWidget->item(0)->text());
	}
	else{
		assert(0);
	}
}

void TimetableViewStudentsDaysHorizontalForm::yearChanged(const QString &yearName)
{
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}

	if(yearName==QString())
		return;
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;
	}

	disconnect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));

	groupsListWidget->clear();
	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	connect(groupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(groupChanged(const QString&)));

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
}

void TimetableViewStudentsDaysHorizontalForm::groupChanged(const QString &groupName)
{
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}

	if(groupName==QString())
		return;
	
	StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(groupName);
	if(ss==NULL){
		QMessageBox::warning(this, tr("FET warning"), tr("Inexistent group - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_GROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect group settings - please reload this dialog"));
		return;
	}
	
	StudentsGroup* stg=(StudentsGroup*)ss;
	
	/*QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;
	}

	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	int groupIndex=gt.rules.searchAugmentedGroup(yearName, groupName);
	if(groupIndex<0){
		QMessageBox::warning(this, tr("FET warning"),
		 tr("Invalid group in the selected year, or the groups of the current year are not updated")+
		 "\n\n"+
		 tr("Solution: please try to select a different year and after that select the current year again, "
		 "to refresh the groups list, or close this dialog and open again the students view timetable dialog"));
		return;
	}*/
	
	disconnect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));

	subgroupsListWidget->clear();
	
	//StudentsGroup* stg=sty->groupsList.at(groupIndex);
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListWidget->addItem(sts->name);
	}

	connect(subgroupsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(subgroupChanged(const QString&)));

	if(subgroupsListWidget->count()>0)
		subgroupsListWidget->setCurrentRow(0);
}

void TimetableViewStudentsDaysHorizontalForm::subgroupChanged(const QString &subgroupName)
{
	Q_UNUSED(subgroupName);
	
	updateStudentsTimetableTable();
}

void TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable(){
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}
	
	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	//QString yearname;
	//QString groupname;
	QString subgroupname;

	/*if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count())
		return;
	if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count())
		return;
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count())
		return;*/

	//yearname = yearsListWidget->currentItem()->text();
	//groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(ss==NULL){
		QMessageBox::information(this, tr("FET warning"), tr("Inexistent subgroup - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_SUBGROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect subgroup settings - please reload this dialog"));
		return;
	}
	StudentsSubgroup* sts=(StudentsSubgroup*)ss;

	s="";
	s += subgroupname;

	classNameTextLabel->setText(s);

	assert(gt.rules.initialized);

	int i = sts->indexInInternalSubgroupsList;
	assert(sts == gt.rules.internalSubgroupsList[i]);
	const Solution& best_solution=CachedSchedule::getCachedSolution();

	for(int k=0; k<studentsTimetableTable->columnCount(); k++){
		for(int j=0; j<studentsTimetableTable->rowCount(); j++){
			if (studentsTimetableTable->rowSpan(j,k) != 1 || studentsTimetableTable->columnSpan(j,k) != 1)
				studentsTimetableTable->setSpan(j, k, 1, 1);
			studentsTimetableTable->item(j,k)->setData(Qt::UserRole, UNALLOCATED_ACTIVITY);
		}
	}

	for(int k=0; k<gt.rules.nDaysPerWeek && k<studentsTimetableTable->columnCount(); k++){
		for(int j=0; j<gt.rules.nHoursPerDay && j<studentsTimetableTable->rowCount(); ){
			int nextJ = j+1;

			//begin by Marco Vassura
			// add colors (start)
			//if(USE_GUI_COLORS) {
				studentsTimetableTable->item(j, k)->setBackground(studentsTimetableTable->palette().color(QPalette::Base));
				studentsTimetableTable->item(j, k)->setForeground(studentsTimetableTable->palette().color(QPalette::Text));
			//}
			// add colors (end)
			//end by Marco Vassura
			s="";
			int ai=CachedSchedule::students_timetable_weekly[i][k][j]; //activity index
			if(ai!=UNALLOCATED_ACTIVITY){
				const Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				
				assert(act->studentsNames.count()>=1);
				if((act->studentsNames.count()==1 && act->studentsNames.at(0)!=subgroupname) || act->studentsNames.count()>=2){
					s+=act->studentsNames.join(", ");
					s+="\n";
				}
				
				if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
					QString ats=act->activityTagsNames.join(", ");
					s+=act->subjectName +" "+ ats;
				}
				else{
					s+=act->subjectName;
				}
				if(act->teachersNames.count()>0){
					s+="\n";
					s+=act->teachersNames.join(", ");
				}
				
				int r=best_solution.room(ai);
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					//s+=" ";
					//s+=tr("R:%1", "Room").arg(gt.rules.internalRoomsList[r]->name);
					s+="\n";
					s+=gt.rules.internalRoomsList[r]->name;
				}

				s+=LockUnlock::getActivityLockTipString(act->id);
				
				//begin by Marco Vassura
				// add colors (start)
				if(USE_GUI_COLORS) {
					QBrush bg(stringToColor(act->subjectName));
					studentsTimetableTable->item(j, k)->setBackground(bg);
					double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
					if (brightness<0.5)
						studentsTimetableTable->item(j, k)->setForeground(QBrush(Qt::white));
					else
						studentsTimetableTable->item(j, k)->setForeground(QBrush(Qt::black));
				}
				// add colors (end)
				//end by Marco Vassura

				while (nextJ < gt.rules.nHoursPerDay && ai == CachedSchedule::students_timetable_weekly[i][k][nextJ])
					nextJ++;
			}
			else{
				if(subgroupNotAvailableDayHour[i][k][j]) {
					if (PRINT_NOT_AVAILABLE_TIME_SLOTS)
						s+="-x-";
					while (nextJ < gt.rules.nHoursPerDay && subgroupNotAvailableDayHour[i][k][nextJ])
						nextJ++;
				}
				else if(breakDayHour[k][j]) {
					if (PRINT_BREAK_TIME_SLOTS)
						s+="-X-";
					while (nextJ < gt.rules.nHoursPerDay && breakDayHour[k][nextJ])
						nextJ++;
				}
			}
			studentsTimetableTable->item(j, k)->setText(s);
			studentsTimetableTable->item(j,k)->setData(Qt::UserRole, ai);
			for (int fakeJ = j+1; fakeJ < nextJ; fakeJ++)
				studentsTimetableTable->item(j,k)->setData(Qt::UserRole, ai);

			int rowspan = nextJ - j;
			if (rowspan != studentsTimetableTable->rowSpan(j,k))
				studentsTimetableTable->setSpan(j, k, rowspan, 1);

			j = nextJ;
		}
	}

	studentsTimetableTable->resizeRowsToContents();
	
	tableWidgetUpdateBug(studentsTimetableTable);
	
	detailActivity(studentsTimetableTable->currentItem());

	updateNotPlacedActivities();
	updateBrokenConstraints();
}

void TimetableViewStudentsDaysHorizontalForm::newActivityNotPlaced(int ai)
{
	//notPlacedActivitiesListWidget->addItem(gt.rules.internalActivitiesList[ai].getDescription());
	updateNotPlacedActivities();
}

//begin by Marco Vassura
QColor TimetableViewStudentsDaysHorizontalForm::stringToColor(QString s)
{
	// CRC-24 Based on RFC 2440 Section 6.1
	unsigned long crc = 0xB704CEL;
	int i;
	QChar *data = s.data();
	while (!data->isNull()) {
		crc ^= (data->unicode() & 0xFF) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000)
				crc ^= 0x1864CFBL;
		}
		data++;
	}
	return QColor::fromRgb((int)(crc>>16), (int)((crc>>8) & 0xFF), (int)(crc & 0xFF));
}

//end by Marco Vassura

void TimetableViewStudentsDaysHorizontalForm::updateNotPlacedActivities()
{
	StudentsSubgroup *subgroup = static_cast<StudentsSubgroup*>(subgroupsListWidget->currentItem() ? gt.rules.searchAugmentedStudentsSet(subgroupsListWidget->currentItem()->text()) : nullptr);
	StudentsGroup *group = static_cast<StudentsGroup*>(groupsListWidget->currentItem() ? gt.rules.searchAugmentedStudentsSet(groupsListWidget->currentItem()->text()) : nullptr);
	StudentsYear *year = static_cast<StudentsYear*>(yearsListWidget->currentItem() ? gt.rules.searchStudentsSet(yearsListWidget->currentItem()->text()) : nullptr);
	if (!group || !group->subgroupsList.contains(subgroup) || !year || !year->groupsList.contains(group)) {
		for(StudentsYear* tmp_year : qAsConst(gt.rules.augmentedYearsList)){
			for(StudentsGroup* tmp_group : qAsConst(tmp_year->groupsList)){
				if (tmp_group->subgroupsList.contains(subgroup)) {
					group = tmp_group;
					year = tmp_year;
				}
			}
		}
	}

	notPlacedActivitiesListWidget->clear();
	for(int ai : CachedSchedule::getCachedSolution().getUnallocatedActivities(gt.rules)) {
		const Activity& activity = gt.rules.internalActivitiesList[ai];
		if ((year && activity.studentsNames.contains(year->name))
			|| (group && activity.studentsNames.contains(group->name))
			|| (subgroup && activity.studentsNames.contains(subgroup->name)))
			notPlacedActivitiesListWidget->addItem(activity.getDescription());
//		studentsTimetableTable->tempRemovedActivities.insert(ai);
	}
}

void TimetableViewStudentsDaysHorizontalForm::updateBrokenConstraints()
{
	brokenConstraintsListWidget->clear();
	const Solution& solution = CachedSchedule::getCachedSolution();
	int i = 0;
	for(QString item : qAsConst(solution.conflictsDescriptionList)) {
		QListWidgetItem * list_item = new QListWidgetItem(item);
		if (solution.severeConflictList.contains(item)) {
			QString unallocatedActivityConflictText = tr("Time constraint basic compulsory broken: unallocated activity with id=%1 (%2)").arg("_-ID-_").arg("");
			unallocatedActivityConflictText = unallocatedActivityConflictText.left(unallocatedActivityConflictText.indexOf("_-ID-_"));
			if (!item.startsWith(unallocatedActivityConflictText))
				list_item ->setIcon(QIcon(":/images/error.png"));
		}
		brokenConstraintsListWidget->addItem(list_item);
		i++;
	}
}

void TimetableViewStudentsDaysHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsDaysHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewStudentsDaysHorizontalForm::detailActivity(QTableWidgetItem* item)
{
	if(item==NULL){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}
	
	if(item->row()>=gt.rules.nHoursPerDay || item->column()>=gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}

	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count())
		return;
	if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count())
		return;
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count())
		return;

	yearname = yearsListWidget->currentItem()->text();
	groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
/*	if(!(i<gt.rules.nInternalSubgroups)){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students dialog"));
		return;
	}*/
	assert(i<gt.rules.nInternalSubgroups);
	int j=item->row();
	int k=item->column();
	s="";
	if(j>=0 && k>=0){
		int ai=CachedSchedule::students_timetable_weekly[i][k][j]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			//s+=act->getDetailedDescriptionWithConstraints(gt.rules);
			s+=act->getDetailedDescription();

			//int r=rooms_timetable_weekly[i][k][j];
			int r=CachedSchedule::getCachedSolution().room(ai);
			if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
				s+="\n";
				s+=tr("Room: %1").arg(gt.rules.internalRoomsList[r]->name);
				if(gt.rules.internalRoomsList[r]->building!=""){
					s+="\n";
					s+=tr("Building=%1").arg(gt.rules.internalRoomsList[r]->building);
				}
				s+="\n";
				s+=tr("Capacity=%1").arg(gt.rules.internalRoomsList[r]->capacity);
			}

			s+=LockUnlock::getActivityLockDetailsString(act->id);
		}
		else{
			if(subgroupNotAvailableDayHour[i][k][j]){
				s+=tr("Students subgroup is not available 100% in this slot");
				s+="\n";
			}
			if(breakDayHour[k][j]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setPlainText(s);
}

void TimetableViewStudentsDaysHorizontalForm::lockTime()
{
	lock(true, false);
}

void TimetableViewStudentsDaysHorizontalForm::lockSpace()
{
	lock(false, true);
}

void TimetableViewStudentsDaysHorizontalForm::lockTimeSpace()
{
	lock(true, true);
}

void TimetableViewStudentsDaysHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	//find subgroup index
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString yearname;
	QString groupname;
	QString subgroupname;
	
	if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a year"));
		return;
	}
	if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a group"));
		return;
	}
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a subgroup"));
		return;
	}

	yearname = yearsListWidget->currentItem()->text();
	groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	const Solution* c=&CachedSchedule::getCachedSolution();

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);

	bool report=false; //the messages are annoying
	ErrorList errors;

	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	//lock selected activities
	for (const QTableWidgetItem *item : studentsTimetableTable->selectedItems()) {
		int j = item->row();
		int k = item->column();
				int ai=CachedSchedule::students_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY){
					int hour=c->hour(ai, gt.rules);
					int day=c->day(ai, gt.rules);

					const Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(lockTime){
						TimeConstraint* ctr = LockUnlock::lockTime(&gt.rules, act->id, day, hour);
						if (ctr != NULL) {
							errors << ErrorCode(ErrorCode::Info, tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules));
							addedT++;
						}
						else{
							int nUnlocked = 0;
							errors << LockUnlock::unlockTime(&gt.rules, act->id, nUnlocked);
							unlockedT += nUnlocked;
						}
						if (errors.hasFatal())
							break;
					}

					int ri=c->room(ai);
					if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
						SpaceConstraint* ctr = LockUnlock::lockSpace(&gt.rules, act->id, gt.rules.internalRoomsList[ri]->name);
						if (ctr != NULL) {
							errors << ErrorCode(ErrorCode::Info, tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules));
							addedS++;
						}
						else{
							int nUnlocked = 0;
							errors << LockUnlock::unlockSpace(&gt.rules, act->id, nUnlocked);
							unlockedS += nUnlocked;
						}
						if (errors.hasFatal())
							break;
					}
				}
	}
	ErrorRenderer::renderErrorList(this, errors, report ? ErrorCode::Verbose : ErrorCode::Warning);

	QStringList added;
	QStringList removed;
	if(addedT>0){
		if(FET_LANGUAGE=="en_US"){
			if(addedT==1)
				added << QString("Added 1 locking time constraint.");
			else
				added << QString("Added %1 locking time constraints.").arg(addedT);
		} else {
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 "(open these files with Qt Linguist and see the translation of this field).",
#if QT_VERSION < 0x050000
			 QCoreApplication::CodecForTr,
#endif
			 addedT);
		}
	}
	if(addedS>0){
		if(FET_LANGUAGE=="en_US"){
			if(addedS==1)
				added << QString("Added 1 locking space constraint.");
			else
				added << QString("Added %1 locking space constraints.").arg(addedS);
		} else {
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 "(open these files with Qt Linguist and see the translation of this field).",
#if QT_VERSION < 0x050000
			 QCoreApplication::CodecForTr,
#endif
			 addedS);
		}
	}
	if(unlockedT>0){
		if(FET_LANGUAGE=="en_US"){
			if(unlockedT==1)
				removed << QString("Removed 1 locking time constraint.");
			else
				removed << QString("Removed %1 locking time constraints.").arg(unlockedT);
		} else {
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 "(open these files with Qt Linguist and see the translation of this field).",
#if QT_VERSION < 0x050000
			 QCoreApplication::CodecForTr,
#endif
			 unlockedT);
		}
	}
	if(unlockedS>0){
		if(FET_LANGUAGE=="en_US"){
			if(unlockedS==1)
				removed << QString("Removed 1 locking space constraint.");
			else
				removed << QString("Removed %1 locking space constraints.").arg(unlockedS);
		} else {
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 "(open these files with Qt Linguist and see the translation of this field).",
#if QT_VERSION < 0x050000
			 QCoreApplication::CodecForTr,
#endif
			 unlockedS);
		}
	}
	QStringList all;
	if(!added.isEmpty())
		all<<added.join("\n");
	if(!removed.isEmpty())
		all<<removed.join("\n");
	QString s=all.join("\n\n");
	if(s.isEmpty())
		s=QCoreApplication::translate("TimetableViewForm", "No locking constraints added or removed.");
	QMessageBox::information(this, tr("FET information"), s);

	LockUnlock::assertIsUpdated(&gt.rules);

	LockUnlock::increaseCommunicationSpinBox(); //this is needed
}

void TimetableViewStudentsDaysHorizontalForm::help()
{
	QString s="";
	//s+=QCoreApplication::translate("TimetableViewForm", "You can drag sections to increase/decrease them.");
	//s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Lock/unlock: you can select one or more activities in the table and toggle lock/unlock in time, space or both.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "There will be added or removed locking constraints for the selected activities (they can be unlocked only if they are not permanently locked).");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Locking time constraints are constraints of type activity preferred starting time. Locking space constraints are constraints of type"
		" activity preferred room. You can see these constraints in the corresponding constraints dialogs. New locking constraints are added at the end of the list of constraints.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "If a cell is (permanently) locked in time or space, it contains abbreviations to show that: PLT (permanently locked time), LT (locked time), "
		"PLS (permanently locked space) or LS (locked space).", "Translate the abbreviations also. Make sure the abbreviations in your language are different between themselves "
		"and the user can differentiate easily between them. These abbreviations may appear also in other places, please use the same abbreviations.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "There is a combo box in this dialog. You can choose each year, group, and subgroup if you select 'Years',"
		" each group and subgroup if you select 'Groups', or each subgroup if you select 'Subgroups'.");
	s+="\n\n";

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
