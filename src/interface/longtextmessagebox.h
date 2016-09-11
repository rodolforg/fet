/*
File longtextmessagebox.h
*/

/***************************************************************************
                          longtextmessagebox.h  -  description
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

#ifndef LONGTEXTMESSAGEBOX_H
#define LONGTEXTMESSAGEBOX_H

#include <QObject>

class QString;
class QWidget;

class LongTextMessageBox: public QObject
{
	Q_OBJECT
	
public:
	static int confirmation
	 ( QWidget * parent, const QString & title, const QString & text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton );

	static int largeConfirmation
	 ( QWidget * parent, const QString & title, const QString & text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton );

	static int mediumConfirmation
	 ( QWidget * parent, const QString & title, const QString & text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton );

	static void information
	 ( QWidget * parent, const QString & title, const QString & text);

	static void largeInformation
	 ( QWidget * parent, const QString & title, const QString & text);

	static void mediumInformation
	 ( QWidget * parent, const QString & title, const QString & text);

	static int confirmationWithDimensions
	 ( QWidget * parent, const QString & title, const QString & text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton, int MINW, int MAXW, int MINH, int MAXH );

	static void informationWithDimensions
	 ( QWidget * parent, const QString & title, const QString & text, int MINW, int MAXW, int MINH, int MAXH);

	//Used only in modifystudentsyearform.cpp
	static int largeConfirmationWithDimensionsThreeButtonsYesNoCancel
	 ( QWidget * parent, const QString & title, const QString & text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton );
	 //Yes, No, Cancel, in this order
};

#endif
