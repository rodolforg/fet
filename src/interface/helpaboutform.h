/***************************************************************************
                          helpaboutform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef HELPABOUTFORM_H
#define HELPABOUTFORM_H

#include "ui_helpaboutform_template.h"

class HelpAboutForm : public QDialog, public Ui::HelpAboutForm_template
{
	Q_OBJECT
	
public:
	HelpAboutForm(QWidget* parent);
	~HelpAboutForm();
};

#endif
