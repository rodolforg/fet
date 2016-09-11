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

#include "messageboxes.h"

#ifndef FET_COMMAND_LINE
#include <QMessageBox>
#else
#include <iostream>
using namespace std;
#endif

#include "longtextmessagebox.h"

#ifdef FET_COMMAND_LINE
void commandLineMessage(QWidget* parent, const QString& title, const QString& message)
{
	Q_UNUSED(parent);

	cout<<qPrintable(FetCommandLine::tr("Title: %1").arg(title))<<endl;
	cout<<qPrintable(FetCommandLine::tr("Message: %1").arg(message))<<endl;
	cout<<endl;
}

int commandLineMessage(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text, int defaultButton, int escapeButton)
{
	Q_UNUSED(parent);

	cout<<qPrintable(FetCommandLine::tr("Title: %1").arg(title))<<endl;
	cout<<qPrintable(FetCommandLine::tr("Message: %1").arg(message))<<endl;

	if(button0Text!=QString())
		cout<<qPrintable(FetCommandLine::tr("Button 0 text: %1").arg(button0Text))<<endl;
	if(button1Text!=QString())
		cout<<qPrintable(FetCommandLine::tr("Button 1 text: %1").arg(button1Text))<<endl;
	if(button2Text!=QString())
		cout<<qPrintable(FetCommandLine::tr("Button 2 text: %1").arg(button2Text))<<endl;

	cout<<qPrintable(FetCommandLine::tr("Default button: %1").arg(defaultButton))<<endl;
	cout<<qPrintable(FetCommandLine::tr("Escape button: %1").arg(escapeButton))<<endl;

	cout<<qPrintable(FetCommandLine::tr("Pressing default button %1").arg(defaultButton))<<endl;
	
	cout<<endl;
	
	return defaultButton;
}
#endif

//Rules

int RulesConstraintIgnored::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

void RulesImpossible::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void RulesReconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

int RulesReconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

int RulesReconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

void RulesReconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

int RulesReconcilableMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

void RulesIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

/*int RulesIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}*/

void RulesUsualInformation::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void RulesReadingWrongConstraint::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void IrreconcilableCriticalMessage::critical(QWidget* parent, const QString& title, const QString& message)
{
#ifndef FET_COMMAND_LINE
	QMessageBox::critical(parent, title, message);
#else
	commandLineMessage(parent, title, message);
#endif
}

//GeneratePre

int GeneratePreReconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

void GeneratePreReconcilableMessage::largeInformation(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::largeInformation(parent, title, message);
}

void GeneratePreIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

int GeneratePreIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

void GeneratePreIrreconcilableMessage::mediumInformation(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

int GeneratePreIrreconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

//TimetableExport

int TimetableExportMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

int TimetableExportMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}

//TimeConstraint

void TimeConstraintIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void TimeConstraintIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

//SpaceConstraint

void SpaceConstraintIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void SpaceConstraintIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

//Fet

void FetMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

void FetMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message);
}

//QProgressDialog

#ifdef FET_COMMAND_LINE

QProgressDialog::QProgressDialog(QWidget* parent)
{
	Q_UNUSED(parent);
}

void QProgressDialog::setWindowTitle(const QString& title){
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress title: %1").arg(title))<<endl;
	}
}

void QProgressDialog::setLabelText(const QString& label){
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress label: %1").arg(label))<<endl;
	}
}

void QProgressDialog::setRange(int a, int b){
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress range: %1..%2").arg(a).arg(b))<<endl;
	}
}

void QProgressDialog::setModal(bool m){
	if(VERBOSE){
		if(m)
			cout<<qPrintable(FetCommandLine::tr("Progress setModal(true)"))<<endl;
		else
			cout<<qPrintable(FetCommandLine::tr("Progress setModal(false)"))<<endl;
	}
}

void QProgressDialog::setValue(int v){
	Q_UNUSED(v);
	//cout<<qPrintable(FetCommandLine::tr("Progress value: %1").arg(v))<<endl;
}

bool QProgressDialog::wasCanceled(){
	return false;
}

#endif
