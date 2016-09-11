/***************************************************************************
                                FET
                          -------------------
   copyright             : (C) by Liviu Lalescu, Volker Dirr
    email                : Liviu Lalescu: see http://lalescu.ro/liviu/ , Volker Dirr: see http://www.timetabling.de/
 ***************************************************************************
                          timetableprintform.h  -  description
                             -------------------
    begin                : March 2010
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

#ifndef TIMETABLEPRINTFORM_H
#define TIMETABLEPRINTFORM_H

#include <QObject>
#include <QList>

#include <QTextDocument> //maybe better TextEdit, so you can also edit?!
#include <QSpinBox>
#include <QDialog>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

class QTableWidget;
class QRadioButton;
class QCheckBox;
class QPushButton;
class QTableWidgetItem;
class QComboBox;
class QGroupBox;
class QToolButton;
class QSizePolicy;
class QListWidget;

class StartTimetablePrint: public QObject{
	Q_OBJECT

public:
	StartTimetablePrint();
	~StartTimetablePrint();

	static void startTimetablePrint(QWidget* parent);
};

class TimetablePrintForm: public QDialog{
	Q_OBJECT
	
public:
	TimetablePrintForm(QWidget* parent);
	~TimetablePrintForm();

private:
	QComboBox* CBTables;
	QListWidget* namesList;
	QPushButton* pbSelectAll;
	QPushButton* pbUnselectAll;

	QGroupBox* actionsBox;
	QRadioButton* RBDaysHorizontal;
	QRadioButton* RBDaysVertical;
	QRadioButton* RBTimeHorizontal;
	QRadioButton* RBTimeVertical;
	//By Liviu Lalescu - unused anymore
	//QCheckBox* CBDivideTimeAxisByDay;
	QRadioButton* RBTimeHorizontalDay;
	QRadioButton* RBTimeVerticalDay;

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
	
//	QCheckBox* markNotAvailable;
//	QCheckBox* markBreak;
//	QCheckBox* printSameStartingTime;
	QCheckBox* printDetailedTables;
	QCheckBox* printActivityTags;	//TODO: to this with combo box: "always", "never", "if available".
									//maybe TODO: do it similar with students, teachers, rooms, ...
	QCheckBox* repeatNames;
	QCheckBox* automaticColors;
	
	QPushButton* pbPrintPreviewSmall;
	QPushButton* pbPrintPreviewFull;
	QPushButton* pbPrint;
	
	QPushButton* pbClose;

private slots:
	void selectAll();
	void unselectAll();

	void updateNamesList();
	QString updateHtmlPrintString(bool printAll);
	
	//void updateCBDivideTimeAxisByDay();

	void print();
	void printPreviewFull();
	void updatePreviewFull(QPrinter* printer);
	void printPreviewSmall();
	void updatePreviewSmall(QPrinter* printer);
};

#endif
