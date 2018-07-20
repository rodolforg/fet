/*
File centerwidgetonscreen.cpp
*/

/***************************************************************************
                          centerwidgetonscreen.cpp  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "rules.h"
#include "timetable.h"
#include "studentsset.h"

#ifndef FET_COMMAND_LINE
#include "fetmainform.h"

#include "timetableshowconflictsform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomstimehorizontalform.h"

#include <QWidget>
#include <QApplication>
#include <QWidgetList>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QScreen>
#else
#include <QDesktopWidget>
#endif

#include <QRect>
#include <QSize>
#include <QPoint>

#include <QCoreApplication>
#include <QMessageBox>

#include <QSettings>
#endif

#include <QObject>
#include <QMetaObject>

#include <QString>

#ifndef FET_COMMAND_LINE
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>

#include <QComboBox>

#include <QSet>

extern const QString COMPANY;
extern const QString PROGRAM;

extern FetMainForm* pFetMainForm;

//extern QApplication* pqapplication;

extern Timetable gt;

void centerWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);

	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
/*	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());*/
}

void forceCenterWidgetOnScreen(QWidget* widget)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());
}

/*void centerWidgetOnParent(QWidget* widget, QWidget* parent)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	assert(parent!=NULL);
	
	QRect frect=widget->geometry();
	frect.moveCenter(parent->geometry().center());
	widget->move(frect.topLeft());
}*/

int maxScreenWidth(QWidget* widget)
{
	QRect rect = QApplication::desktop()->availableGeometry(widget);

	return rect.width();
}

int maxRecommendedWidth(QWidget* widget)
{
	int d=maxScreenWidth(widget);
	
	if(d<800)
		d=800;
		
	//if(d>1000)
	//	d=1000;
	
	return d;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	
	QRect rect=widget->geometry();
	settings.setValue(name+QString("/geometry"), rect);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	if(settings.contains(name+QString("/geometry"))){
		QRect rect=settings.value(name+QString("/geometry")).toRect();
		if(rect.isValid()){
			bool ok=false;
#if QT_VERSION >= 0x050000
			for(QScreen* screen : QGuiApplication::screens()){
				if(screen->availableGeometry().intersects(rect)){
#else
			for(int i=0; i<QApplication::desktop()->screenCount(); i++){
				if(QApplication::desktop()->availableGeometry(i).intersects(rect)){
#endif
					ok=true;
					break;
				}
			}
		
			if(ok){
				widget->setGeometry(rect);
			}
		}
	}
}

void setParentAndOtherThings(QWidget* widget, QWidget* parent)
{
	Q_UNUSED(widget);
	Q_UNUSED(parent);

/*	if(!widget->parentWidget()){
		widget->setParent(parent, Qt::Dialog);
	
		QRect rect=widget->geometry();
		rect.translate(widget->geometry().x() - widget->frameGeometry().x(), widget->geometry().y() - widget->frameGeometry().y());
		widget->setGeometry(rect);
	}*/
}

void setStretchAvailabilityTableNicely(QTableWidget* notAllowedTimesTable)
{
#if QT_VERSION >= 0x050000
	notAllowedTimesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	notAllowedTimesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	int q=notAllowedTimesTable->horizontalHeader()->defaultSectionSize();
	notAllowedTimesTable->horizontalHeader()->setMinimumSectionSize(q);

#if QT_VERSION >= 0x050000
	notAllowedTimesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	notAllowedTimesTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	q=-1;
	for(int i=0; i<notAllowedTimesTable->verticalHeader()->count(); i++)
		if(q<notAllowedTimesTable->verticalHeader()->sectionSizeHint(i))
			q=notAllowedTimesTable->verticalHeader()->sectionSizeHint(i);
	notAllowedTimesTable->verticalHeader()->setMinimumSectionSize(q);
	
	//2011-09-23
	for(int i=0; i<notAllowedTimesTable->rowCount(); i++){
		for(int j=0; j<notAllowedTimesTable->columnCount(); j++){
			QFont font=notAllowedTimesTable->item(i,j)->font();
			font.setBold(true);
			notAllowedTimesTable->item(i,j)->setFont(font);
		}
	}
	notAllowedTimesTable->setCornerButtonEnabled(false);
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	if(!rules->modified){
		rules->modified=true;
	
		if(rules==&gt.rules && pFetMainForm!=NULL)
			if(!pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(true);
	}
}

void setRulesUnmodifiedAndOtherThings(Rules* rules)
{
	if(rules->modified){
		rules->modified=false;
	
		if(rules==&gt.rules && pFetMainForm!=NULL)
			if(pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(false);
	}
}

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current constraint is referring to subgroup %1, so the constraint is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");

	QMessageBox::warning(parent, title, message);
}

void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " You are trying to modify a constraint, leaving it to refer to the original subgroup %1, which is invisible in the combo box - but this is impossible.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");
	
	QMessageBox::warning(parent, title, message);
}

void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current activity is referring to subgroup %1, so the activity is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");

	QMessageBox::warning(parent, title, message);
}

int populateStudentsComboBox(QComboBox* studentsComboBox, const QString& selectedStudentsSet, bool addEmptyAtBeginning)
{
	studentsComboBox->clear();
	
	int currentIndex=0;
	int selectedIndex=-1;
	
	if(addEmptyAtBeginning){
		studentsComboBox->addItem(QString(""));
		if(selectedStudentsSet==QString(""))
			selectedIndex=currentIndex;
		currentIndex++;
		if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED && gt.rules.yearsList.count()>0){
#if QT_VERSION >= 0x040400
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
		}
	}

	if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_SIMPLE){
		for(StudentsYear* sty : qAsConst(gt.rules.yearsList)){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			for(StudentsGroup* stg : qAsConst(sty->groupsList)){
				studentsComboBox->addItem(stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					for(StudentsSubgroup* sts : qAsConst(stg->subgroupsList)){
						studentsComboBox->addItem(sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_ICONS){
		for(StudentsYear* sty : qAsConst(gt.rules.yearsList)){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			for(StudentsGroup* stg : qAsConst(sty->groupsList)){
				studentsComboBox->addItem(QIcon(":/images/group.png"), stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					for(StudentsSubgroup* sts : qAsConst(stg->subgroupsList)){
						studentsComboBox->addItem(QIcon(":/images/subgroup.png"), sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED){
		QSet<QString> years;
		
		bool haveGroups=false;
	
		for(StudentsYear* sty : qAsConst(gt.rules.yearsList)){
			assert(!years.contains(sty->name));
			years.insert(sty->name);
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			
			if(!haveGroups && sty->groupsList.count()>0)
				haveGroups=true;
		}
		
		if(haveGroups){
#if QT_VERSION >= 0x040400
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
			QSet<QString> groups;
		
			bool haveSubgroups=false;
	
			for(StudentsYear* sty : qAsConst(gt.rules.yearsList)){
				for(StudentsGroup* stg : qAsConst(sty->groupsList)){
					if(!groups.contains(stg->name)){
						groups.insert(stg->name);
						studentsComboBox->addItem(stg->name);
						if(stg->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
						
						if(!haveSubgroups && stg->subgroupsList.count()>0)
							haveSubgroups=true;
					}
				}
			}

			if(SHOW_SUBGROUPS_IN_COMBO_BOXES && haveSubgroups){
#if QT_VERSION >= 0x040400
				studentsComboBox->insertSeparator(studentsComboBox->count());
				currentIndex++;
#endif

				QSet<QString> subgroups;

				for(StudentsYear* sty : qAsConst(gt.rules.yearsList)){
					for(StudentsGroup* stg : qAsConst(sty->groupsList)){
						for(StudentsSubgroup* sts : qAsConst(stg->subgroupsList)){
							if(!subgroups.contains(sts->name)){
								subgroups.insert(sts->name);
								studentsComboBox->addItem(sts->name);
								if(sts->name==selectedStudentsSet)
									selectedIndex=currentIndex;
								currentIndex++;
							}
						}
					}
				}
			}
		}
	}
	else{
		assert(0);
	}
	
	return selectedIndex;
}

/*void closeAllTimetableViewDialogs()
{
	//QList<QWidget*> tlwl=pqapplication->topLevelWidgets();
	QWidgetList tlwl=QApplication::topLevelWidgets();

	for(QWidget* wi : qAsConst(tlwl))
		//if(wi->isVisible()){
		if(1){
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdf!=NULL){
				vsdf->close();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vstf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vstf!=NULL){
				vstf->close();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdf!=NULL){
				vtchdf->close();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchtf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchtf!=NULL){
				vtchtf->close();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdf!=NULL){
				vrdf->close();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrtf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrtf!=NULL){
				vrtf->close();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=NULL){
				scf->close();
				continue;
			}
		}
}*/

void updateAllTimetableViewDialogs()
{
	//QList<QWidget*> tlwl=pqapplication->topLevelWidgets();
	QWidgetList tlwl=QApplication::topLevelWidgets();

	for(QWidget* wi : qAsConst(tlwl))
		if(1 /*wi->isVisible()*/){
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdf!=NULL){
				vsdf->newTimetableGenerated();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vstf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vstf!=NULL){
				vstf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdf!=NULL){
				vtchdf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchtf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchtf!=NULL){
				vtchtf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdf!=NULL){
				vrdf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrtf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrtf!=NULL){
				vrtf->newTimetableGenerated();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=NULL){
				scf->newTimetableGenerated();
				continue;
			}
		}
}

#else

void centerWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);
}

void forceCenterWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);
}

int maxScreenWidth(QWidget* widget)
{
	Q_UNUSED(widget);
	
	return 0;
}

int maxRecommendedWidth(QWidget* widget)
{
	Q_UNUSED(widget);

	return 0;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	Q_UNUSED(widget);
	Q_UNUSED(alternativeName);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	Q_UNUSED(widget);
	Q_UNUSED(alternativeName);
}

void setParentAndOtherThings(QWidget* widget, QWidget* parent)
{
	Q_UNUSED(widget);
	Q_UNUSED(parent);
}

void setStretchAvailabilityTableNicely(QTableWidget* notAllowedTimesTable)
{
	Q_UNUSED(notAllowedTimesTable);
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

void setRulesUnmodifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void populateStudentsComboBox(QComboBox* studentsComboBox)
{
	Q_UNUSED(studentsComboBox);
}

/*void closeAllTimetableViewDialogs()
{
}*/

void updateAllTimetableViewDialogs()
{
}

#endif
