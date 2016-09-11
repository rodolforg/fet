/***************************************************************************
                          tablewidgetupdatebug.cpp  -  description
                             -------------------
    begin                : 2009
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

#include "tablewidgetupdatebug.h"

#include <QtGlobal>
#include <QString>

#include <QWidget>

void tableWidgetUpdateBug(QTableWidget* table)
{
	//this is for a bug appeared in Qt 4.5.3. The bug seems to be solved in Qt subsequent versions.

	QString buggy1=QString("4.5.3");
	QString crtVersion=qVersion();
	if(crtVersion==buggy1){
		if(table->viewport())
			table->viewport()->update();
	}
}
