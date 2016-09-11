/***************************************************************************
                                FET
                          -------------------
   copyright             : (C) by Liviu Lalescu, Volker Dirr
    email                : Liviu Lalescu: see http://lalescu.ro/liviu/ , Volker Dirr: see http://www.timetabling.de/
 ***************************************************************************
                          statisticsprintform.h  -  description
                             -------------------
    begin                : November 2013
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

#ifndef STATISTICSPRINTFORM_H
#define STATISTICSPRINTFORM_H

#include <QObject>
#include <QList>

#include <QTextDocument> //maybe better TextEdit, so you can also edit?!
#include <QSpinBox>
#include <QDialog>

#include "statisticsexport.h"

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

class QTableWidget;
class QRadioButton;
class QCheckBox;
class QPushButton;
class QTableWidgetItem;
class QGroupBox;
class QComboBox;
class QToolButton;
class QSizePolicy;
class QListWidget;

class StartStatisticsPrint: public QObject{
	Q_OBJECT

public:
	StartStatisticsPrint();
	~StartStatisticsPrint();

	static void startStatisticsPrint(QWidget* parent);
};

class StatisticsPrintForm: public QDialog{
	Q_OBJECT
	
public:
	StatisticsPrintForm(QWidget* parent);
	~StatisticsPrintForm();

private:
	FetStatistics statisticValues;

	QListWidget* namesList;
	QPushButton* pbSelectAll;
	QPushButton* pbUnselectAll;

	QGroupBox* actionsBox;
	QRadioButton* studentSubjectRB;
	QRadioButton* studentTeacherRB;
	QRadioButton* teacherSubjectRB;
	QRadioButton* teacherStudentRB;
	QRadioButton* subjectStudentRB;
	QRadioButton* subjectTeacherRB;

	QGroupBox* optionsBox;
	QComboBox* CBBreak;
	QComboBox* CBWhiteSpace;
//	QComboBox* CBprinterMode;
	QComboBox* CBpaperSize;
	QComboBox* CBorientationMode;
	QSpinBox* activitiesPadding;
	QSpinBox* tablePadding;
	QSpinBox* fontSizeTable;
	QSpinBox* maxNames;
	
	QSpinBox* leftPageMargin;
	QSpinBox* topPageMargin;
	QSpinBox* rightPageMargin;
	QSpinBox* bottomPageMargin;
	
	//QCheckBox* printDetailedTables;	//maybe TODO: only a single number per cell if not detailed
	QCheckBox* printActivityTags;
	
	QPushButton* pbPrintPreviewSmall;
	QPushButton* pbPrintPreviewFull;
	QPushButton* pbPrint;
	
	QPushButton* pbClose;

private slots:
	void selectAll();
	void unselectAll();

	void updateNamesList();
	QString updateHTMLprintString(bool printAll);
	
	void print();
	void printPreviewFull();
	void updatePreviewFull(QPrinter* printer);
	void printPreviewSmall();
	void updatePreviewSmall(QPrinter* printer);
};

#endif
