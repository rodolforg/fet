/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      activityplanning.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITYPLANNINGFORM_H
#define ACTIVITYPLANNINGFORM_H

#include <QResizeEvent>

#include <QObject>

#include "sparsetableview.h"

#include <QList>

#include <QDialog>

class QTableWidget;
class QRadioButton;
class QCheckBox;
class QPushButton;
class QTableWidgetItem;
class QComboBox;
class QGroupBox;
class QToolButton;
class QSizePolicy;
class QSplitter;

class StartActivityPlanning{
public:
	StartActivityPlanning();
	~StartActivityPlanning();

	static void startActivityPlanning(QWidget* parent);
};

class ActivityPlanningForm: public QDialog{
	Q_OBJECT
	
public:
	ActivityPlanningForm(QWidget *parent);
	~ActivityPlanningForm();

private:
	bool buttonsVisible;

	QSplitter* leftSplitter;
	SparseTableView* activitiesTableView;
	SparseTableView* teachersTableView;
	QRadioButton* RBActivity;
	QRadioButton* RBSubactivity;
	QRadioButton* RBAdd;
	QRadioButton* RBModify;
	QRadioButton* RBDelete;
	QComboBox* CBActive;
	QCheckBox* showDuplicates;
	QCheckBox* showYears;
	QCheckBox* showGroups;
	QCheckBox* showSubgroups;
	QCheckBox* showTeachers;
	QCheckBox* showActivityTags;
	QCheckBox* hideEmptyLines;
	QCheckBox* hideUsedTeachers;
	QCheckBox* swapAxis;
	QPushButton* pbPseudoActivities;
	QPushButton* pbDeleteAll;
	//QPushButton* pbHelp;
	
	QToolButton* showHideButton;
	QSizePolicy origShowHideSizePolicy;
	
	QPushButton* pbClose;
	QCheckBox* changedActivities;
	
	QGroupBox* actionsBox;
	QGroupBox* optionsBox;
	
	void computeActivitiesForDeletion(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
		const QList<int>& tmpID, const QList<int>& tmpGroupID,
		int& nTotalActsDeleted,
		QList<int>& _idsToBeRemoved, QList<int>& _agidsToBeRemoved,
		QList<bool>& _affectOtherTeachersToBeRemoved, bool& _affectOtherTeachersOverall,
		QList<bool>& _affectOtherStudentsSetsToBeRemoved, bool& _affectOtherStudentsSetsOverall,
		QList<bool>& _affectOtherSubjectsToBeRemoved, bool& _affectOtherSubjectsOverall);
	
private slots:
	//void activitiesCellSelected(int, int);
	void activitiesCellSelected(const QModelIndex& index);
	//void teachersCellSelected(QTableWidgetItem*);
	void teachersCellSelected(const QModelIndex& index);
	//mouseTracking (start 1/4)
	//void ActivtiesCellEntered(int, int);
	//void TeachersCellEntered(int, int);
	//mouseTracking (end 1/4)
	void activitiesTableHorizontalHeaderClicked(int);
	void activitiesTableVerticalHeaderClicked(int);
	void teachersTableHorizontalHeaderClicked(int);
	void updateTables(int);
	void updateTables_Students_Subjects();
	void updateTables_Teachers();
	void updateTablesVisual(int);
	//void help();
	void pseudoActivities();
	void deleteAll();
	
	void showHide();
	
/*protected:
	void resizeEvent(QResizeEvent* event);*/
};


//communication box by Liviu Lalescu
class PlanningCommunicationSpinBox: public QObject{
	Q_OBJECT

private:
	int value;
	int maxValue;
	int minValue;
	
public:
	PlanningCommunicationSpinBox();
	~PlanningCommunicationSpinBox();
	
signals:
	int valueChanged(int newValue);
	
public slots:
	void increaseValue();
};


class PlanningChanged{
public:
	static void increasePlanningCommunicationSpinBox();
};

#endif
