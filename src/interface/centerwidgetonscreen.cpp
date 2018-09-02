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

#include "centerwidgetonscreen.h"
#include "studentsset.h"

#include <QWidget>
#include <QApplication>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QScreen>
#endif
#include <QDesktopWidget>

#include <QRect>

#include <QCoreApplication>
#include <QMessageBox>

#include <QSettings>

#include <QHeaderView>
#include <QTableWidget>

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

int maxRecommendedWidth(QWidget* widget)
{
	int d=QApplication::desktop()->availableGeometry(widget).width();
	
	if(d<800)
		d=800;
		
	//if(d>1000)
	//	d=1000;
	
	return d;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings;
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	
	QRect rect=widget->geometry();
	settings.setValue(name+QString("/geometry"), rect);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings;
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	if(settings.contains(name+QString("/geometry"))){
		QRect rect=settings.value(name+QString("/geometry")).toRect();
		if(rect.isValid()){
			bool ok=false;
#if QT_VERSION >= 0x050000
			foreach(QScreen* screen, QGuiApplication::screens()){
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
