//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <http://lalescu.ro/liviu/>
// Copyright (C) 2013 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGEBOXES_H
#define MESSAGEBOXES_H

#include "timetable_defs.h"

#include <QString>

#include <QObject>

#ifndef FET_COMMAND_LINE
#include <QWidget>
#else
class QWidget{
};
#endif

#ifdef FET_COMMAND_LINE
class FetCommandLine: public QObject{
	Q_OBJECT
};

void commandLineMessage(QWidget* parent, const QString& title, const QString& message);
int commandLineMessage(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text, int defaultButton, int escapeButton);
#else
//Just to disable a Qt moc warning
class DummyFetGuiClass: public QObject{
	Q_OBJECT
};
#endif

//Rules

class RulesConstraintIgnored{
public:
	static int mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);
};

class RulesImpossible{
public:
	static void warning(QWidget* parent, const QString& title, const QString& message);
};

class RulesReconcilableMessage{
public:
	static void warning(QWidget* parent, const QString& title, const QString& message);

	static int warning(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);

	static int mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);

	static void information(QWidget* parent, const QString& title, const QString& message);

	static int information(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);
};

class RulesIrreconcilableMessage{
public:
	static void warning(QWidget* parent, const QString& title, const QString& message);

/*	static int warning(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);*/
};

class RulesUsualInformation{
public:
	static void information(QWidget* parent, const QString& title, const QString& message);
};

class RulesReadingWrongConstraint{
public:
	static void warning(QWidget* parent, const QString& title, const QString& message);
};

class IrreconcilableCriticalMessage{
public:
	static void critical(QWidget* parent, const QString& title, const QString& message);
};

//GeneratePre

class GeneratePreReconcilableMessage{
public:
	static int mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);

	static void largeInformation(QWidget* parent, const QString& title, const QString& message);
};

class GeneratePreIrreconcilableMessage{
public:
	static void information(QWidget* parent, const QString& title, const QString& message);

	static int information(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);

	static void mediumInformation(QWidget* parent, const QString& title, const QString& message);

	static int mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);
};

//TimetableExport

class TimetableExportMessage{
public:
	static int information(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);

	static int warning(QWidget* parent, const QString& title, const QString& message,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton);
};

//TimeConstraint

class TimeConstraintIrreconcilableMessage{
public:
	static void information(QWidget* parent, const QString& title, const QString& message);
	static void warning(QWidget* parent, const QString& title, const QString& message);
};

//SpaceConstraint

class SpaceConstraintIrreconcilableMessage{
public:
	static void information(QWidget* parent, const QString& title, const QString& message);
	static void warning(QWidget* parent, const QString& title, const QString& message);
};

//Fet

class FetMessage{
public:
	static void information(QWidget* parent, const QString& title, const QString& message);
	static void warning(QWidget* parent, const QString& title, const QString& message);
};

//QProgressDialog

#ifdef FET_COMMAND_LINE

class QProgressDialog{
public:
	QProgressDialog(QWidget* parent);
	void setWindowTitle(const QString& title);
	void setLabelText(const QString& title);
	void setRange(int a, int b);
	void setModal(bool m);
	void setValue(int v);
	bool wasCanceled();
};

#endif

#endif
