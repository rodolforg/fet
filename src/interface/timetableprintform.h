/***************************************************************************
                                FET
                          -------------------
   copyright             : (C) by Liviu Lalescu, Volker Dirr
    email                : Liviu Lalescu: see https://lalescu.ro/liviu/ , Volker Dirr: see http://www.timetabling.de/
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

#include "ui_timetableprintform_template.h"

class StartTimetablePrint: public QObject{
	Q_OBJECT

public:
	static void startTimetablePrint(QWidget* parent);
};

class TimetablePrintForm: public QDialog, Ui::TimetablePrintForm_template
{
	Q_OBJECT
	
public:
	TimetablePrintForm(QWidget* parent);
	~TimetablePrintForm();

private slots:
	void selectAll();
	void unselectAll();

	void updateNamesList();
	QString updateHtmlPrintString(bool printAll);
	
	//void updateCBDivideTimeAxisByDay();

	void print();
	void printPreviewFull();
	void printPreviewSmall();
#ifndef QT_NO_PRINTER
	void updatePreviewFull(QPrinter* printer);
	void updatePreviewSmall(QPrinter* printer);
#endif
};

#endif
