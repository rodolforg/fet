/***************************************************************************
                          timetableviewstudentstimehorizontalform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#include <QtGlobal>

#include "tablewidgetupdatebug.h"

#include "longtextmessagebox.h"

#include "fetmainform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "matrix.h"

#include "lockunlock.h"

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QListWidget>

#include <QList>

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

#include <QHash>
#include <QSet>
#include <QPair>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;

extern Solution best_solution;

extern bool simulation_running;

//extern Matrix3D<bool> subgroupNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;
extern QHash<QString, QSet<QPair<int, int> > > studentsSetNotAvailableDayHour;

extern QSet<int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet<int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

void TimetableViewStudentsTimeHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	//int day=index.column()/gt.rules.nHoursPerDay;
	int hour=index.column()%gt.rules.nHoursPerDay;

	/*if(day>=0 && day<gt.rules.nDaysPerWeek-1 && hour==gt.rules.nHoursPerDay-1){
		QPen pen(painter->pen());
		pen.setWidth(2);
		painter->setPen(pen);
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
	}*/
	
	/*assert(table!=NULL);
	QBrush bg(table->item(index.row(), index.column())->background());
	QPen pen(painter->pen());

	double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
	if (brightness<0.5)
		pen.setColor(Qt::white);
	else
		pen.setColor(Qt::black);

	painter->setPen(pen);*/
	
	if(hour==0)
		painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
	if(hour==nColumns-1)
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

	if(index.row()==0)
		painter->drawLine(option.rect.topLeft(), option.rect.topRight());
	if(index.row()==nRows-1)
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
}


TimetableViewStudentsTimeHorizontalForm::TimetableViewStudentsTimeHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	verticalSplitter->setStretchFactor(0, 20);
	verticalSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 5);
	horizontalSplitter->setStretchFactor(1, 1);

	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(studentsTimetableTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIGNAL(clicked()), this, SLOT(lockTime()));
	connect(lockSpacePushButton, SIGNAL(clicked()), this, SLOT(lockSpace()));
	connect(lockTimeSpacePushButton, SIGNAL(clicked()), this, SLOT(lockTimeSpace()));

	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	
	lockRadioButton->setChecked(true);
	unlockRadioButton->setChecked(false);
	toggleRadioButton->setChecked(false);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());

	if(settings.contains(this->metaObject()->className()+QString("/lock-radio-button")))
		lockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/lock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/unlock-radio-button")))
		unlockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/unlock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/toggle-radio-button")))
		toggleRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/toggle-radio-button")).toBool());

///////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	//added by Volker Dirr
	//these 2 lines are not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	/*if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}*/

	LockUnlock::increaseCommunicationSpinBox();
	
	usedStudentsSet.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		foreach(QString students, act->studentsNames){
			if(!usedStudentsSet.contains(students))
				usedStudentsSet.insert(students);
			
			/*QList<int> list=activitiesForStudentsSet.value(students, QList<int>());
			list.append(i);
			activitiesForStudentsSet.insert(students, list);*/
		}
	}
	
	QHash<QString, QSet<QPair<int, int> > >::const_iterator it=studentsSetNotAvailableDayHour.constBegin();
	while(it!=studentsSetNotAvailableDayHour.constEnd()){
		if(!usedStudentsSet.contains(it.key()))
			usedStudentsSet.insert(it.key());
			
		it++;
	}
	
	//Students not available constraints
	/*QSet<QString> studentsSet3;
	foreach(StudentsYear* year, gt.rules.augmentedYearsList){
		if(!studentsSet3.contains(year->name)){
			studentsSet3.insert(year->name);
		}
		foreach(StudentsGroup* group, year->groupsList){
			if(!studentsSet3.contains(group->name)){
				studentsSet3.insert(group->name);
			}
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				if(!studentsSet3.contains(subgroup->name)){
					studentsSet3.insert(subgroup->name);
				}
			}
		}
	}
	
	//notAvailableHash.clear();
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* ctr=gt.rules.internalTimeConstraintsList[i];
		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* csna=(ConstraintStudentsSetNotAvailableTimes*)ctr;
			assert(csna->active);
			if(studentsSet3.contains(csna->students) && !usedStudentsSet.contains(csna->students))
				usedStudentsSet.insert(csna->students);
			if(studentsSet3.contains(csna->students)){
				assert(!notAvailableHash.contains(csna->students));
				notAvailableHash.insert(csna->students, csna);
			}
		}
	}*/
	
	QSet<QString> studentsSet2;
	usedStudentsList.clear();
	foreach(StudentsYear* year, gt.rules.augmentedYearsList){
		if(usedStudentsSet.contains(year->name) && !studentsSet2.contains(year->name)){
			usedStudentsList.append(year->name);
			studentsSet2.insert(year->name);
		}
		foreach(StudentsGroup* group, year->groupsList){
			if(usedStudentsSet.contains(group->name) && !studentsSet2.contains(group->name)){
				usedStudentsList.append(group->name);
				studentsSet2.insert(group->name);
			}
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				if(usedStudentsSet.contains(subgroup->name) && !studentsSet2.contains(subgroup->name)){
					usedStudentsList.append(subgroup->name);
					studentsSet2.insert(subgroup->name);
				}
			}
		}
	}

	studentsTimetableTable->setRowCount(usedStudentsList.count());
	studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	
	oldItemDelegate=studentsTimetableTable->itemDelegate();
	newItemDelegate=new TimetableViewStudentsTimeHorizontalDelegate(NULL, studentsTimetableTable->rowCount(), gt.rules.nHoursPerDay);
	studentsTimetableTable->setItemDelegate(newItemDelegate);
	
	bool min2letters=false;
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		if(gt.rules.daysOfTheWeek[d].size()>gt.rules.nHoursPerDay){
			min2letters=true;
			break;
		}
	}
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		QString dayName=gt.rules.daysOfTheWeek[d];
		int t=dayName.size();
		int q=t/gt.rules.nHoursPerDay;
		int r=t%gt.rules.nHoursPerDay;
		QStringList list;
		
		if(q==0)
			q=1;
		
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			if(!min2letters){
				list.append(dayName.left(1));
				dayName.remove(0, 1);
			}
			else if(i<r || q<=1){
				assert(q>=1);
				list.append(dayName.left(q+1));
				dayName.remove(0, q+1);
			}
			else{
				list.append(dayName.left(q));
				dayName.remove(0, q);
			}
		}
	
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.hoursOfTheDay[h]);
			item->setToolTip(gt.rules.daysOfTheWeek[d]+"\n"+gt.rules.hoursOfTheDay[h]);
			studentsTimetableTable->setHorizontalHeaderItem(d*gt.rules.nHoursPerDay+h, item);
		}
	}
	for(int t=0; t<usedStudentsList.count(); t++){
		QTableWidgetItem* item=new QTableWidgetItem(usedStudentsList.at(t));
		item->setToolTip(usedStudentsList.at(t));
		studentsTimetableTable->setVerticalHeaderItem(t, item);
	}

	for(int t=0; t<usedStudentsList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item= new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(t, d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
//	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
	
	initialRecommendedHeight=studentsTimetableTable->verticalHeader()->sectionSizeHint(0);
	
	int h;
	int w;

	if(settings.contains(this->metaObject()->className()+QString("/vertical-header-size"))){
		h=settings.value(this->metaObject()->className()+QString("/vertical-header-size")).toInt();
		if(h==0)
			h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
	else{
		h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
//	if(h==0)
//		h=initialRecommendedHeight;

	if(settings.contains(this->metaObject()->className()+QString("/horizontal-header-size"))){
		w=settings.value(this->metaObject()->className()+QString("/horizontal-header-size")).toInt();
		if(w==0)
			w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
	else{
		w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
//	if(w==0)
//		w=2*initialRecommendedHeight;
		
	widthSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	widthSpinBox->setMinimum(MINIMUM_WIDTH_SPIN_BOX_VALUE);
#if QT_VERSION >= 0x050200
	widthSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	widthSpinBox->setMaximum(maxScreenWidth(this));
#endif
	widthSpinBox->setValue(w);
	widthSpinBox->setSpecialValueText(tr("Automatic"));
	
	heightSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	heightSpinBox->setMinimum(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
#if QT_VERSION >= 0x050200
	heightSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	heightSpinBox->setMaximum(maxScreenWidth(this));
#endif
	heightSpinBox->setValue(h);
	heightSpinBox->setSpecialValueText(tr("Automatic"));
	
	widthSpinBoxValueChanged();
	heightSpinBoxValueChanged();
	
	connect(widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(widthSpinBoxValueChanged()));
	connect(heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(heightSpinBoxValueChanged()));
	
//	teachersTimetableTable->verticalHeader()->setDefaultSectionSize(h);
//	teachersTimetableTable->horizontalHeader()->setDefaultSectionSize(w);

#if QT_VERSION >= 0x050000
	studentsTimetableTable->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
	studentsTimetableTable->verticalHeader()->setResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif
	//}
	////////////////
	
	subjectsCheckBox->setChecked(true);
	teachersCheckBox->setChecked(false);
	
	if(settings.contains(this->metaObject()->className()+QString("/subjects-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/subjects-check-box-state")).toBool();
		subjectsCheckBox->setChecked(state);
	}
	if(settings.contains(this->metaObject()->className()+QString("/teachers-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/teachers-check-box-state")).toBool();
		teachersCheckBox->setChecked(state);
	}
	
	connect(subjectsCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateStudentsTimetableTable()));
	connect(teachersCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateStudentsTimetableTable()));
	
	//added by Volker Dirr
	connect(&communicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateStudentsTimetableTable()));
	
	updateStudentsTimetableTable();
}

TimetableViewStudentsTimeHorizontalForm::~TimetableViewStudentsTimeHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-splitter-state"), verticalSplitter->saveState());

	//save horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/lock-radio-button"), lockRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/unlock-radio-button"), unlockRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/toggle-radio-button"), toggleRadioButton->isChecked());

	if(heightSpinBox->value()<=MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), heightSpinBox->value());
		
	if(widthSpinBox->value()<=MINIMUM_WIDTH_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), widthSpinBox->value());
		
	settings.setValue(this->metaObject()->className()+QString("/subjects-check-box-state"), subjectsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/teachers-check-box-state"), teachersCheckBox->isChecked());
		
	studentsTimetableTable->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
	
	usedStudentsList.clear();
	usedStudentsSet.clear();
	//activitiesForStudentsSet.clear();
	
	//notAvailableHash.clear();
}

void TimetableViewStudentsTimeHorizontalForm::resizeRowsAfterShow()
{
//	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	assert(gt.rules.initialized);
	
	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());
		
		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), NULL);
		assert(ss!=NULL);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
		
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);
		
		QSet<QPair<int, int> > notAvailableDayHour=studentsSetNotAvailableDayHour.value(usedStudentsList.at(t), QSet<QPair<int, int> >());
		
		/*ConstraintStudentsSetNotAvailableTimes* ctr=notAvailableHash.value(usedStudentsList.at(t), NULL);
		if(ctr!=NULL){
			for(int i=0; i<ctr->days.count(); i++){
				int d=ctr->days.at(i);
				int h=ctr->hours.at(i);
				notAvailableDayHour.insert(QPair<int,int>(d,h));
			}
		}*/
		
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());

				//begin by Marco Vassura
				// add colors (start)
				//if(USE_GUI_COLORS) {
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setBackground(studentsTimetableTable->palette().color(QPalette::Base));
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(studentsTimetableTable->palette().color(QPalette::Text));
				//}
				// add colors (end)
				//end by Marco Vassura

				QString s = "";
				
				int ai=students_timetable_weekly[sbg][d][h]; //activity index
				
				bool ok=true;
				if(ai==UNALLOCATED_ACTIVITY){
					ok=false;
				}
				else{
					if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
						ok=false;
					}
				}
				
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ok){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					assert(act!=NULL);
					
					if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
						QString ats=act->activityTagsNames.join(", ");
						s += act->subjectName+" "+ats;
					}
					else{
						s += act->subjectName;
					}
					
					//teachers
					if(act->teachersNames.count()>0){
						s+=" ";
						s+="\n";
						s+=act->teachersNames.join(", ");
					}
					
					if(act->studentsNames.count()==1){
						//Comment taken from the teachers view timetable time horizontal
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.at(0)==teachername);
					}
					else{
						assert(act->studentsNames.count()>=2);
						//Comment taken from the teachers view timetable time horizontal
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.contains(teachername));
						s+=" ";
						s+="\n";
						s+=act->studentsNames.join(", ");
					}
					
					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						//s+=" ";
						//s+=tr("R:%1", "Room").arg(gt.rules.internalRoomsList[r]->name);
						s+=" ";
						s+="\n";
						s+=gt.rules.internalRoomsList[r]->name;
					}
					
					//added by Volker Dirr (start)
					QString descr="";
					QString tt="";
					if(idsOfPermanentlyLockedTime.contains(act->id)){
						descr+=QCoreApplication::translate("TimetableViewForm", "PLT", "Abbreviation for permanently locked time. There are 4 string: permanently locked time, permanently locked space, "
							"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
							" These abbreviations may appear also in other places, please use the same abbreviations.");
						tt=", ";
					}
					else if(idsOfLockedTime.contains(act->id)){
						descr+=QCoreApplication::translate("TimetableViewForm", "LT", "Abbreviation for locked time. There are 4 string: permanently locked time, permanently locked space, "
						"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
							" These abbreviations may appear also in other places, please use the same abbreviations.");
						tt=", ";
					}
					if(idsOfPermanentlyLockedSpace.contains(act->id)){
						descr+=tt+QCoreApplication::translate("TimetableViewForm", "PLS", "Abbreviation for permanently locked space. There are 4 string: permanently locked time, permanently locked space, "
							"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
							" These abbreviations may appear also in other places, please use the same abbreviations.");
					}
					else if(idsOfLockedSpace.contains(act->id)){
						descr+=tt+QCoreApplication::translate("TimetableViewForm", "LS", "Abbreviation for locked space. There are 4 string: permanently locked time, permanently locked space, "
							"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
							" These abbreviations may appear also in other places, please use the same abbreviations.");
					}
					if(descr!=""){
						descr.prepend("\n(");
						descr.prepend(" ");
						descr.append(")");
					}

					if(idsOfPermanentlyLockedTime.contains(act->id) || idsOfLockedTime.contains(act->id)){
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setBold(true);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					else{
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setBold(false);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					
					s+=descr;
					//added by Volker Dirr (end)
				
					//begin by Marco Vassura
					// add colors (start)
					if(USE_GUI_COLORS /*&& act->studentsNames.count()>0*/){
						QBrush bg(stringToColor(act->subjectName));
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setBackground(bg);
						double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
						if (brightness<0.5)
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(Qt::white));
						else
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(Qt::black));
					}
					// add colors (end)
					//end by Marco Vassura
				}
				else{
//					if(subgroupNotAvailableDayHour[sbg][d][h] && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					if(notAvailableDayHour.contains(QPair<int,int>(d,h)) && PRINT_NOT_AVAILABLE_TIME_SLOTS)
						s+="-x-";
					else if(breakDayHour[d][h] && PRINT_BREAK_TIME_SLOTS)
						s+="-X-";
				}
				if(ok){
					QString s2;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(teachersCheckBox->isChecked() && !act->teachersNames.isEmpty()){
						s2+=act->teachersNames.join(", ");
					}
					if(subjectsCheckBox->isChecked()){
						if(!s2.isEmpty()){
							s2+=" ";
							s2+="\n";
						}
						s2+=act->subjectName;
					}
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setText(s2);
				}
				else
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setText(s);
				studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setToolTip(s);
			}
		}
	}
	//	for(int i=0; i<gt.rules.nHoursPerDay; i++) //added in version 3_9_16, on 16 Oct. 2004
	//		teachersTimetableTable->adjustRow(i);

//	teachersTimetableTable->resizeRowsToContents();
	
	tableWidgetUpdateBug(studentsTimetableTable);
	
	detailActivity(studentsTimetableTable->currentItem());
}

/*void TimetableViewTeachersTimeHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

//	teachersTimetableTable->resizeRowsToContents();
}*/

//begin by Marco Vassura
QColor TimetableViewStudentsTimeHorizontalForm::stringToColor(QString s)
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

void TimetableViewStudentsTimeHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewStudentsTimeHorizontalForm::detailActivity(QTableWidgetItem* item){
	if(item==NULL){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(item->row()>=usedStudentsList.count() || item->column()>=gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	int t=item->row();
	
	/*if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}*/

	int d=item->column()/gt.rules.nHoursPerDay;
	int h=item->column()%gt.rules.nHoursPerDay;

	int sbg=-1;
	
	if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
		QMessageBox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
		return;
	}
	assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
	StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), NULL);
	assert(ss!=NULL);
	if(ss->type==STUDENTS_YEAR){
		StudentsYear* year=(StudentsYear*)ss;
		sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* group=(StudentsGroup*)ss;
		sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
	}
	else if(ss->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
		sbg=subgroup->indexInInternalSubgroupsList;
	}
	else{
		assert(0);
	}
	
	assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);
	
	QSet<QPair<int, int> > notAvailableDayHour=studentsSetNotAvailableDayHour.value(usedStudentsList.at(t), QSet<QPair<int, int> >());

	/*QSet<QPair<int, int> > notAvailableDayHour;
	QSet<ConstraintStudentsSetNotAvailableTimes*> cs=gt.rules.ssnatHash.value(usedStudentsList.at(t), QSet<ConstraintStudentsSetNotAvailableTimes*>());
	if(!cs.isEmpty()){
		assert(cs.count()==1);
		ConstraintStudentsSetNotAvailableTimes* ctr=*(cs.begin());
		
		for(int i=0; i<ctr->days.count(); i++){
			int d=ctr->days.at(i);
			int h=ctr->hours.at(i);
			notAvailableDayHour.insert(QPair<int,int>(d,h));
		}
	}*/

	/*int teacher=gt.rules.searchTeacher(gt.rules.internalTeachersList[t]->name);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("The teacher is invalid - please close this dialog and open a new view teachers timetable"));
		return;
	}*/
	QString s = "";
	if(d>=0 && d<gt.rules.nDaysPerWeek && h>=0 && h<gt.rules.nHoursPerDay){
		int ai=students_timetable_weekly[sbg][d][h]; //activity index

		bool ok=true;
		if(ai==UNALLOCATED_ACTIVITY){
			ok=false;
		}
		else{
			if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
				ok=false;
			}
		}
		
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ok){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			//s += act->getDetailedDescriptionWithConstraints(gt.rules);
			s += act->getDetailedDescription(gt.rules);

			//int r=rooms_timetable_weekly[teacher][k][j];
			int r=best_solution.rooms[ai];
			if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
				s+="\n";
				s+=tr("Room: %1").arg(gt.rules.internalRoomsList[r]->name);
			}
			//added by Volker Dirr (start)
			QString descr="";
			QString tt="";
			if(idsOfPermanentlyLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "permanently locked time", "refers to activity");
				tt=", ";
			}
			else if(idsOfLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "locked time", "refers to activity");
				tt=", ";
			}
			if(idsOfPermanentlyLockedSpace.contains(act->id)){
				descr+=tt+QCoreApplication::translate("TimetableViewForm", "permanently locked space", "refers to activity");
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=tt+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
		}
		else{
			//if(subgroupNotAvailableDayHour[sbg][d][h]){
			if(notAvailableDayHour.contains(QPair<int,int>(d,h))){
				s+=tr("Students set is not available 100% in this slot");
				s+="\n";
			}
			if(breakDayHour[d][h]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setPlainText(s);
}

void TimetableViewStudentsTimeHorizontalForm::lockTime()
{
	this->lock(true, false);
}
	
void TimetableViewStudentsTimeHorizontalForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewStudentsTimeHorizontalForm::lockTimeSpace()
{
	this->lock(true, true);
}
			
void TimetableViewStudentsTimeHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	//cout<<"teachers begin, isc="<<gt.rules.internalStructureComputed<<endl;

	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	Solution* tc=&best_solution;
	
	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;
	
	QSet<int> dummyActivitiesColumn; //Dummy activities (no students) column to be considered, because the whole column is selected.
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			assert(d*gt.rules.nHoursPerDay+h < studentsTimetableTable->columnCount());
			bool wholeColumn=true;
			for(int t=0; t<usedStudentsList.count(); t++){
				assert(t<studentsTimetableTable->rowCount());

				if(!studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){
					wholeColumn=false;
					break;
				}
			}
			if(wholeColumn)
				dummyActivitiesColumn.insert(d+h*gt.rules.nDaysPerWeek);
		}
	}
	
	QSet<int> dummyActivities;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		if(gt.rules.internalActivitiesList[ai].iSubgroupsList.count()==0){
			if(tc->times[ai]!=UNALLOCATED_TIME){
				int da=tc->times[ai]%gt.rules.nDaysPerWeek;
				int ha=tc->times[ai]/gt.rules.nDaysPerWeek;
				for(int ha2=ha; ha2<ha+gt.rules.internalActivitiesList[ai].duration; ha2++)
					if(dummyActivitiesColumn.contains(da+ha2*gt.rules.nDaysPerWeek))
						if(!dummyActivities.contains(ai))
							dummyActivities.insert(ai);
			}
		}
	}

	QSet<int> realActivities;
	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());

		/*if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
			QMessagebox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
			return;
		}*/

		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), NULL);
		assert(ss!=NULL);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
	
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());
				if(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){

					int ai=students_timetable_weekly[sbg][d][h];
					
					bool ok=true;
					if(ai==UNALLOCATED_ACTIVITY){
						ok=false;
					}
					else{
						if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
							ok=false;
						}
					}
		
					//Activity* act=gt.rules.activitiesList.at(ai);
					if(ok)
						if(!realActivities.contains(ai))
							realActivities.insert(ai);
				}
			}
		}
	}

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert( ! (realActivities.contains(ai) && dummyActivities.contains(ai)) );
		if(realActivities.contains(ai) || dummyActivities.contains(ai)){
			assert(tc->times[ai]!=UNALLOCATED_TIME);
			int day=tc->times[ai]%gt.rules.nDaysPerWeek;
			int hour=tc->times[ai]/gt.rules.nDaysPerWeek;

			Activity* act=&gt.rules.internalActivitiesList[ai];
			
			if(lockTime){
				QString s;
			
				QList<TimeConstraint*> tmptc;
				int count=0;

				foreach(ConstraintActivityPreferredStartingTime* c, gt.rules.apstHash.value(act->id, QSet<ConstraintActivityPreferredStartingTime*>())){
					assert(c->activityId==act->id);
					if(c->activityId==act->id && c->weightPercentage==100.0 && c->active && c->day>=0 && c->hour>=0){
						count++;
						if(c->permanentlyLocked){
							if(idsOfLockedTime.contains(c->activityId) || !idsOfPermanentlyLockedTime.contains(c->activityId)){
								QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
									+"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
									+"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
									+"\n\n"+tr("Please report possible bug")
									);
							}
							else{
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
								}
							}
						}
						else{
							if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
								QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
									+"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
									+"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
									+"\n\n"+tr("Please report possible bug")
									);
							}
							else{
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									tmptc.append((TimeConstraint*)c);
								}
							}
						}
					}
				}
				if(count==0 && (lockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false);
					bool t=gt.rules.addTimeConstraint(ctr);
					QString s;
					if(t){
						addedT++;
						idsOfLockedTime.insert(act->id);
						s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
					}
					else{
						delete ctr;
						
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to add 1 constraint, but this is not possible. "
						 "Please report possible bug"));
					}
				}
				else if(count>=1 && (unlockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					if(count>=2)
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

					foreach(TimeConstraint* deltc, tmptc){
						s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
						gt.rules.removeTimeConstraint(deltc);
						idsOfLockedTime.remove(act->id);
						unlockedT++;
						//delete deltc; - done by rules.removeTim...
					}
				}
				tmptc.clear();
				//gt.rules.internalStructureComputed=false;

				if(report){
					int k;
					k=QMessageBox::information(this, tr("FET information"), s,
						tr("Skip information"), tr("See next"), QString(), 1, 0 );

					if(k==0)
						report=false;
				}
			}
			
			int ri=tc->rooms[ai];
			if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
				QString s;
				
				QList<SpaceConstraint*> tmpsc;
				int count=0;

				foreach(ConstraintActivityPreferredRoom* c, gt.rules.aprHash.value(act->id, QSet<ConstraintActivityPreferredRoom*>())){
					assert(c->activityId==act->id);

					if(c->activityId==act->id && c->weightPercentage==100.0 && c->active){
						count++;
						if(c->permanentlyLocked){
							if(idsOfLockedSpace.contains(c->activityId) || !idsOfPermanentlyLockedSpace.contains(c->activityId)){
								QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
									+"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
									+"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
									+"\n\n"+tr("Please report possible bug")
									);
							}
							else{
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
								}
							}
						}
						else{
							if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
								QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
									+"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
									+"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
									+"\n\n"+tr("Please report possible bug")
									);
							}
							else{
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									tmpsc.append((SpaceConstraint*)c);
								}
							}
						}
					}
				}
				if(count==0 && (lockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, false);
					bool t=gt.rules.addSpaceConstraint(ctr);
	
					QString s;
					
					if(t){
						addedS++;
						idsOfLockedSpace.insert(act->id);
						s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
					}
					else{
						delete ctr;
						
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to add 1 constraint, but this is not possible. "
						 "Please report possible bug"));
					}
				}
				else if(count>=1 && (unlockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					if(count>=2)
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

					foreach(SpaceConstraint* delsc, tmpsc){
						s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
						gt.rules.removeSpaceConstraint(delsc);
						idsOfLockedSpace.remove(act->id);
						unlockedS++;
						//delete delsc; done by rules.removeSpa...
					}
				}
				tmpsc.clear();
				//gt.rules.internalStructureComputed=false;
			
				if(report){
					int k;
					k=QMessageBox::information(this, tr("FET information"), s,
						tr("Skip information"), tr("See next"), QString(), 1, 0 );
						
					if(k==0)
						report=false;
				}
			}
		}
	}

	QStringList added;
	QStringList removed;
	if(addedT>0){
		if(addedT==1)
			added << QCoreApplication::translate("TimetableViewForm", "Added 1 locking time constraint.", "constraint is singular (only 1 constraint)");
		else
			added << QCoreApplication::translate("TimetableViewForm", "Added %1 locking time constraints.", "%1 is >= 2, so constraints is plural").arg(addedT);
	}
	if(addedS>0){
		if(addedS==1)
			added << QCoreApplication::translate("TimetableViewForm", "Added 1 locking space constraint.", "constraint is singular (only 1 constraint)");
		else
			added << QCoreApplication::translate("TimetableViewForm", "Added %1 locking space constraints.", "%1 is >= 2, so constraints is plural").arg(addedS);
	}
	if(unlockedT>0){
		if(unlockedT==1)
			removed << QCoreApplication::translate("TimetableViewForm", "Removed 1 locking time constraint.", "constraint is singular (only 1 constraint)");
		else
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %1 locking time constraints.", "%1 is >= 2, so constraints is plural").arg(unlockedT);
	}
	if(unlockedS>0){
		if(unlockedS==1)
			removed << QCoreApplication::translate("TimetableViewForm", "Removed 1 locking space constraint.", "constraint is singular (only 1 constraint)");
		else
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %1 locking space constraints.", "%1 is >= 2, so constraints is plural").arg(unlockedS);
	}
	QString ad=added.join("\n");
	QString re=removed.join("\n");
	QStringList all;
	if(!ad.isEmpty())
		all<<ad;
	if(!re.isEmpty())
		all<<re;
	QString s=all.join("\n\n");
	if(s.isEmpty())
		s=QCoreApplication::translate("TimetableViewForm", "No locking constraints added or removed.");
	QMessageBox::information(this, tr("FET information"), s);

////////// just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //not needed, just for testing
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	LockUnlock::increaseCommunicationSpinBox();
	
	//cout<<"teachers end, isc="<<gt.rules.internalStructureComputed<<endl;
	//cout<<endl;
}

void TimetableViewStudentsTimeHorizontalForm::widthSpinBoxValueChanged()
{
	if(widthSpinBox->value()==MINIMUM_WIDTH_SPIN_BOX_VALUE)
		studentsTimetableTable->horizontalHeader()->setDefaultSectionSize(2*initialRecommendedHeight);
	else
		studentsTimetableTable->horizontalHeader()->setDefaultSectionSize(widthSpinBox->value());
}

void TimetableViewStudentsTimeHorizontalForm::heightSpinBoxValueChanged()
{
	if(heightSpinBox->value()==MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		studentsTimetableTable->verticalHeader()->setDefaultSectionSize(initialRecommendedHeight);
	else
		studentsTimetableTable->verticalHeader()->setDefaultSectionSize(heightSpinBox->value());
}

void TimetableViewStudentsTimeHorizontalForm::help()
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
	s+=tr("If a whole column (day+hour) is selected, there will be locked/unlocked also the dummy activities (activities with no students sets) from that column.");
	
	s+="\n\n";
	s+=tr("A bold font cell means that the activity is locked in time, either permanently or not.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
