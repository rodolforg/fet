/*
File longtextmessagebox.cpp
*/

/***************************************************************************
                          longtextmessagebox.cpp  -  description
                             -------------------
    begin                : 27 June 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "timetable_defs.h"

#ifndef FET_COMMAND_LINE

#include "longtextmessagebox_auxiliary.h"

#include <QMessageBox>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

#else

#include "messageboxes.h"

#endif

#include "centerwidgetonscreen.h"

const int MIN_WIDTH=420;  //golden ratio 1.618 for min dimensions :-)
const int MAX_WIDTH=1000;
const int MIN_HEIGHT=260;
const int MAX_HEIGHT=650;

const int LARGE_MIN_WIDTH=590;
const int LARGE_MAX_WIDTH=1000;
const int LARGE_MIN_HEIGHT=380;
const int LARGE_MAX_HEIGHT=650;

const int MEDIUM_MIN_WIDTH=461;
const int MEDIUM_MAX_WIDTH=1000;
const int MEDIUM_MIN_HEIGHT=285;
const int MEDIUM_MAX_HEIGHT=650;

int LongTextMessageBox::confirmationWithDimensions
 ( QWidget * parent, const QString & title, const QString & text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton, int MINW, int MAXW, int MINH, int MAXH )
{
#ifndef FET_COMMAND_LINE
	if(button0Text==QString() || button1Text==QString() || button2Text!=QString()){
		QMessageBox::critical(parent, tr("FET critical"), tr("You have met a FET bug. The problem is in file"
		 " %1 line %2, the reason is that a confirmation dialog box does not get exactly 2 arguments. Please report bug. FET will now continue."
		 " You probably don't have any problems with your data file - you can save it.").arg(__FILE__).arg(__LINE__));
	}

	QDialog dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb0=NULL;
	if(button0Text!=QString()){
		pb0=new QPushButton(button0Text);
		if(defaultButton==0)
			connect(pb0, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==0)
			connect(pb0, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	QPushButton* pb1=NULL;
	if(button1Text!=QString()){
		pb1=new QPushButton(button1Text);
		if(defaultButton==1)
			connect(pb1, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==1)
			connect(pb1, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	QPushButton* pb2=NULL;
	if(button2Text!=QString()){
		pb2=new QPushButton(button2Text);
		if(defaultButton==2)
			connect(pb2, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==2)
			connect(pb2, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	if(pb0!=NULL)
		hl->addWidget(pb0);
	if(pb1!=NULL)
		hl->addWidget(pb1);
	if(pb2!=NULL)
		hl->addWidget(pb2);
		
	vl->addWidget(te);
	vl->addLayout(hl);
	
	if(pb0!=NULL && defaultButton==0){
		pb0->setDefault(true);
		pb0->setFocus();
	}
	else if(pb1!=NULL && defaultButton==1){
		pb1->setDefault(true);
		pb1->setFocus();
	}
	else if(pb2!=NULL && defaultButton==2){
		pb2->setDefault(true);
		pb2->setFocus();
	}
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	int b=dialog.exec();
	
	if(b==QDialog::Accepted){
		//cout<<"accepted"<<endl;
		return defaultButton;
	}
	else{
		//cout<<"rejected"<<endl;
		return escapeButton;
	}
#else
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text, button0Text, button1Text, button2Text, defaultButton, escapeButton);
	return defaultButton;
#endif
}

int LongTextMessageBox::confirmation
 ( QWidget * parent, const QString & title, const QString & text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		MIN_WIDTH, MAX_WIDTH, MIN_HEIGHT, MAX_HEIGHT);
}

int LongTextMessageBox::largeConfirmation
 ( QWidget * parent, const QString & title, const QString & text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		LARGE_MIN_WIDTH, LARGE_MAX_WIDTH, LARGE_MIN_HEIGHT, LARGE_MAX_HEIGHT);
}

int LongTextMessageBox::mediumConfirmation
 ( QWidget * parent, const QString & title, const QString & text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		MEDIUM_MIN_WIDTH, MEDIUM_MAX_WIDTH, MEDIUM_MIN_HEIGHT, MEDIUM_MAX_HEIGHT);
}

void LongTextMessageBox::informationWithDimensions
 ( QWidget * parent, const QString & title, const QString & text, int MINW, int MAXW, int MINH, int MAXH)
{
#ifndef FET_COMMAND_LINE
	QDialog dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb=new QPushButton(tr("OK"));
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(accept()));
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);
		
	vl->addWidget(te);
	vl->addLayout(hl);
	
	pb->setDefault(true);
	pb->setFocus();
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	dialog.exec();
#else
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text);
#endif
}

void LongTextMessageBox::information
 ( QWidget * parent, const QString & title, const QString & text)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, MIN_WIDTH, MAX_WIDTH, MIN_HEIGHT, MAX_HEIGHT);
}

void LongTextMessageBox::largeInformation
 ( QWidget * parent, const QString & title, const QString & text)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, LARGE_MIN_WIDTH, LARGE_MAX_WIDTH, LARGE_MIN_HEIGHT, LARGE_MAX_HEIGHT);
}

void LongTextMessageBox::mediumInformation
 ( QWidget * parent, const QString & title, const QString & text)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, MEDIUM_MIN_WIDTH, MEDIUM_MAX_WIDTH, MEDIUM_MIN_HEIGHT, MEDIUM_MAX_HEIGHT);
}

#ifndef FET_COMMAND_LINE
MyDialogWithThreeButtons::MyDialogWithThreeButtons(QWidget* parent): QDialog(parent)
{
}

MyDialogWithThreeButtons::~MyDialogWithThreeButtons()
{
}

void MyDialogWithThreeButtons::setYes()
{
	clickedButton=QMessageBox::Yes;
	accept();
}

void MyDialogWithThreeButtons::setNo()
{
	clickedButton=QMessageBox::No;
	accept();
}

void MyDialogWithThreeButtons::setCancel()
{
	clickedButton=QMessageBox::Cancel;
	reject();
}

int LongTextMessageBox::largeConfirmationWithDimensionsThreeButtonsYesNoCancel
 ( QWidget * parent, const QString & title, const QString & text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton )
 //Yes, No, Cancel, in this order.
{
	int MINW=LARGE_MIN_WIDTH;
	int MAXW=LARGE_MAX_WIDTH;
	int MINH=LARGE_MIN_HEIGHT;
	int MAXH=LARGE_MAX_HEIGHT;

	if(button0Text==QString() || button1Text==QString() || button2Text==QString()){
		assert(0);
	}

	MyDialogWithThreeButtons dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb0=NULL;
	if(button0Text!=QString()){
		pb0=new QPushButton(button0Text);
		if(defaultButton==0)
			QObject::connect(pb0, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==0)
			QObject::connect(pb0, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	QPushButton* pb1=NULL;
	if(button1Text!=QString()){
		pb1=new QPushButton(button1Text);
		if(defaultButton==1)
			QObject::connect(pb1, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==1)
			QObject::connect(pb1, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	QPushButton* pb2=NULL;
	if(button2Text!=QString()){
		pb2=new QPushButton(button2Text);
		if(defaultButton==2)
			QObject::connect(pb2, SIGNAL(clicked()), &dialog, SLOT(accept()));
		if(escapeButton==2)
			QObject::connect(pb2, SIGNAL(clicked()), &dialog, SLOT(reject()));
	}
	
	assert(defaultButton==0);
	assert(escapeButton==2);
	
	QObject::connect(pb0, SIGNAL(clicked()), &dialog, SLOT(setYes()));
	QObject::connect(pb1, SIGNAL(clicked()), &dialog, SLOT(setNo()));
	QObject::connect(pb2, SIGNAL(clicked()), &dialog, SLOT(setCancel()));
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	if(pb0!=NULL)
		hl->addWidget(pb0);
	if(pb1!=NULL)
		hl->addWidget(pb1);
	if(pb2!=NULL)
		hl->addWidget(pb2);
		
	vl->addWidget(te);
	vl->addLayout(hl);
	
	if(pb0!=NULL && defaultButton==0){
		pb0->setDefault(true);
		pb0->setFocus();
	}
	else if(pb1!=NULL && defaultButton==1){
		pb1->setDefault(true);
		pb1->setFocus();
	}
	else if(pb2!=NULL && defaultButton==2){
		pb2->setDefault(true);
		pb2->setFocus();
	}
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	dialog.clickedButton=-1;
	int b=dialog.exec();
	
	if(dialog.clickedButton==-1){
		if(b==QDialog::Accepted){
			//return defaultButton;
			dialog.clickedButton=QMessageBox::Yes;
		}
		else{
			//return escapeButton;
			dialog.clickedButton=QMessageBox::Cancel;
		}
	}
	
	assert(dialog.clickedButton>=0);
	return dialog.clickedButton;
}
#endif
