/***************************************************************************
                          timetableviewteachersdayshorizontalform.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
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
#include "timetableviewteachersdayshorizontalform.h"
#include "timetable_defs.h"
#include "fetguisettings.h"

#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "timetableexport.h"

#include "matrix.h"

#include "lockunlock.h"

#include "errorrenderer.h"

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QListWidget>

#include <QList>

#include <QCoreApplication>

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

extern Matrix3D<bool> teacherNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;

TimetableViewTeachersDaysHorizontalForm::TimetableViewTeachersDaysHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	//verticalSplitter->setStretchFactor(0, 1);	//unneeded, because both have the same value
	//verticalSplitter->setStretchFactor(1, 1);	//unneeded, because both have the same value
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);

	teachersTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	teachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(teachersListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(teacherChanged(const QString&)));
	connect(teachersTimetableTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIGNAL(clicked()), this, SLOT(lockTime()));
	connect(lockSpacePushButton, SIGNAL(clicked()), this, SLOT(lockSpace()));
	connect(lockTimeSpacePushButton, SIGNAL(clicked()), this, SLOT(lockTimeSpace()));

	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings;
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

	//restore horizontal splitter state
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());

	LockUnlock::assertIsUpdated(&gt.rules);

	LockUnlock::increaseCommunicationSpinBox();
	
	teachersTimetableTable->setRowCount(gt.rules.nHoursPerDay);
	teachersTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		teachersTimetableTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		teachersTimetableTable->setVerticalHeaderItem(i, item);
	}

	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			teachersTimetableTable->setItem(j, k, item);
			
			//if(j==0 && k==0)
				//teachersTimetableTable->setCurrentItem(item);
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= 0x050000
	teachersTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	teachersTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	////////////////
	
	teachersListWidget->clear();

	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
	}
	else{
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			teachersListWidget->addItem(gt.rules.internalTeachersList[i]->name);
	}

	if(teachersListWidget->count()>0)
		teachersListWidget->setCurrentRow(0);

	//added by Volker Dirr
	connect(&LockUnlock::communicationSpinBox, SIGNAL(valueChanged()), this, SLOT(updateTeachersTimetableTable()));
}

TimetableViewTeachersDaysHorizontalForm::~TimetableViewTeachersDaysHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical splitter state
	QSettings settings;
	settings.setValue(this->metaObject()->className()+QString("/vertical-splitter-state"), verticalSplitter->saveState());

	//save horizontal splitter state
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());
}

void TimetableViewTeachersDaysHorizontalForm::resizeRowsAfterShow()
{
	teachersTimetableTable->resizeRowsToContents();
//	tableWidgetUpdateBug(teachersTimetableTable);
}

void TimetableViewTeachersDaysHorizontalForm::teacherChanged(const QString &teacherName)
{
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}

	if(teacherName==QString())
		return;

	int teacherId=gt.rules.searchTeacher(teacherName);
	if(teacherId<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid teacher - please close this dialog and open a new view teachers dialog"));
		return;
	}

	updateTeachersTimetableTable();
}

void TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable(){
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable "
		"or close the timetable view teachers dialog"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count())
		return;

	teachername = teachersListWidget->currentItem()->text();
	
	const int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("You have an old timetable view teachers dialog opened - please close it"));
		return;
	}

	s = teachername;
	teacherNameTextLabel->setText(s);

	assert(gt.rules.initialized);
	const Solution& best_solution=CachedSchedule::getCachedSolution();

	for(int k=0; k<teachersTimetableTable->columnCount(); k++){
		for(int j=0; j<teachersTimetableTable->rowCount(); j++){
			if (teachersTimetableTable->rowSpan(j,k) != 1 || teachersTimetableTable->columnSpan(j,k) != 1)
				teachersTimetableTable->setSpan(j, k, 1, 1);
		}
	}

	for(int k=0; k<gt.rules.nDaysPerWeek && k<teachersTimetableTable->columnCount(); k++){
		for(int j=0; j<gt.rules.nHoursPerDay && j<teachersTimetableTable->rowCount(); ){
			int nextJ = j+1;

			//begin by Marco Vassura
			// add colors (start)
			//if(USE_GUI_COLORS) {
				teachersTimetableTable->item(j, k)->setBackground(teachersTimetableTable->palette().color(QPalette::Base));
				teachersTimetableTable->item(j, k)->setForeground(teachersTimetableTable->palette().color(QPalette::Text));
			//}
			// add colors (end)
			//end by Marco Vassura
			s = "";
			int ai=CachedSchedule::teachers_timetable_weekly[teacher][k][j]; //activity index
			if(ai!=UNALLOCATED_ACTIVITY){
				const Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				
				if(act->teachersNames.count()==1){
					//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
					//without generating again (as reported by Yush Yuen).
					//assert(act->teachersNames.at(0)==teachername);
				}
				else{
					assert(act->teachersNames.count()>=2);
					//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
					//without generating again (as reported by Yush Yuen).
					//assert(act->teachersNames.contains(teachername));
					s+=act->teachersNames.join(", ");
					s+="\n";
				}
				
				if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
					QString ats=act->activityTagsNames.join(", ");
					s += act->subjectName+" "+ats;
				}
				else{
					s += act->subjectName;
				}
				
				//students
				if(act->studentsNames.count()>0){
					s+="\n";
					s+=act->studentsNames.join(", ");
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
				if(USE_GUI_COLORS /*&& act->studentsNames.count()>0*/){
					QBrush bg(stringToColor(act->subjectName+" "+act->studentsNames.join(", ")));
					teachersTimetableTable->item(j, k)->setBackground(bg);
					double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
					if (brightness<0.5)
						teachersTimetableTable->item(j, k)->setForeground(QBrush(Qt::white));
					else
						teachersTimetableTable->item(j, k)->setForeground(QBrush(Qt::black));
				}
				// add colors (end)
				//end by Marco Vassura

				while (nextJ < gt.rules.nHoursPerDay && ai == CachedSchedule::teachers_timetable_weekly[teacher][k][nextJ])
					nextJ++;
			}
			else{
				if(teacherNotAvailableDayHour[teacher][k][j]) {
					if (PRINT_NOT_AVAILABLE_TIME_SLOTS)
						s+="-x-";
					while (nextJ < gt.rules.nHoursPerDay && teacherNotAvailableDayHour[teacher][k][nextJ])
						nextJ++;
				}
				else if(breakDayHour[k][j]) {
					if (PRINT_BREAK_TIME_SLOTS)
						s+="-X-";
					while (nextJ < gt.rules.nHoursPerDay && breakDayHour[k][nextJ])
						nextJ++;
				}
			}
			teachersTimetableTable->item(j, k)->setText(s);

			int rowspan = nextJ - j;
			if (rowspan != teachersTimetableTable->rowSpan(j,k))
				teachersTimetableTable->setSpan(j, k, rowspan, 1);

			j = nextJ;
		}
	}
	//	for(int i=0; i<gt.rules.nHoursPerDay; i++) //added in version 3_9_16, on 16 Oct. 2004
	//		teachersTimetableTable->adjustRow(i);

	teachersTimetableTable->resizeRowsToContents();
	
	tableWidgetUpdateBug(teachersTimetableTable);
	
	detailActivity(teachersTimetableTable->currentItem());
}

void TimetableViewTeachersDaysHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

	teachersTimetableTable->resizeRowsToContents();
}

//begin by Marco Vassura
QColor TimetableViewTeachersDaysHorizontalForm::stringToColor(QString s)
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

void TimetableViewTeachersDaysHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewTeachersDaysHorizontalForm::detailActivity(QTableWidgetItem* item){
	if(item==NULL){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}

	if(item->row()>=gt.rules.nHoursPerDay || item->column()>=gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable "
		"or close the timetable view teachers dialog"));
		return;
	}

	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count())
		return;

	teachername = teachersListWidget->currentItem()->text();

	s = teachername;

	teacherNameTextLabel->setText(s);

	int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("The teacher is invalid - please close this dialog and open a new view teachers timetable"));
		return;
	}
	else{
		int j=item->row();
		int k=item->column();
		s = "";
		if(j>=0 && k>=0){
			int ai=CachedSchedule::teachers_timetable_weekly[teacher][k][j]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				//s += act->getDetailedDescriptionWithConstraints(gt.rules);
				s += act->getDetailedDescription();

				//int r=rooms_timetable_weekly[teacher][k][j];
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
				if(teacherNotAvailableDayHour[teacher][k][j]){
					s+=tr("Teacher is not available 100% in this slot");
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
}

void TimetableViewTeachersDaysHorizontalForm::lockTime()
{
	this->lock(true, false);
}
	
void TimetableViewTeachersDaysHorizontalForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewTeachersDaysHorizontalForm::lockTimeSpace()
{
	this->lock(true, true);
}
			
void TimetableViewTeachersDaysHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	//find teacher index
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a teacher"));
		return;
	}

	teachername = teachersListWidget->currentItem()->text();
	int i=gt.rules.searchTeacher(teachername);
	
	if(i<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid teacher - please close this dialog and open a new view teachers dialog"));
		return;
	}

	const Solution* c=&CachedSchedule::getCachedSolution();
	
	bool report=false; //the messages are annoying
	ErrorList errors;
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	//lock selected activities
	for (const QTableWidgetItem *item : teachersTimetableTable->selectedItems()) {
		int j = item->row();
		int k = item->column();
				int ai=CachedSchedule::teachers_timetable_weekly[i][k][j];
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

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewTeachersDaysHorizontalForm::help()
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

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
