/*
File longtextmessagebox_auxiliary.h
*/

/***************************************************************************
                          longtextmessagebox_auxiliary.h  -  description
                             -------------------
    begin                : 6 October 2015
    copyright            : (C) 2015 by Lalescu Liviu
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

#ifndef LONGTEXTMESSAGEBOX_AUXILIARY_H
#define LONGTEXTMESSAGEBOX_AUXILIARY_H

#include <QObject>

#include <QDialog>

class QWidget;

class MyDialogWithThreeButtons: public QDialog
{
	Q_OBJECT
	
public:
	int clickedButton;
	
	MyDialogWithThreeButtons(QWidget* parent);
	~MyDialogWithThreeButtons();
	
public slots:
	void setYes();
	void setNo();
	void setCancel();
};

#endif
