/***************************************************************************
                          helponstudentsminhoursdaily.cpp  -  description
                             -------------------
    begin                : 2010
    copyright            : (C) 2010 by Lalescu Liviu
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

#include "helponstudentsminhoursdaily.h"

#include "longtextmessagebox.h"

#include <QApplication>

void HelpOnStudentsMinHoursDaily::help(QWidget* parent)
{
	QString s=tr("Important information about constraints students (set) min hours daily:");
	
	s+="\n\n";
	
	s+=tr("Normally, these constraints do not allow empty days for students (and the check boxes in the dialogs are not selectable).");
	s+=" ";
	s+=tr("If you really need this constraint to allow empty days for students, you must enable the option from Settings->Advanced menu and select the check box.");
	s+="\n\n";
	s+=tr("Be very careful: if your school does not allow empty days for students or if a timetable does not exist with empty days for students, you MUST"
		" keep the constraints the usual way, without allowing empty days.");
	s+=" ";
	s+=tr("The reason is performance: speed of generation and the ability of FET to find a solution.");
	s+=" ";
	s+=tr("If you allow empty days to these constraints and a solution exists only with non-empty days, FET might not be able to find it.");
	s+="\n\n";
	s+=tr("For normal schools or high-schools, probably you won't need to enable this option. Maybe for universities you will need to enable this option.");
	s+="\n\n";
	s+=tr("So, remember: don't allow empty days unless you need it.");
	
	LongTextMessageBox::largeInformation(parent, tr("FET information"), s);
}
