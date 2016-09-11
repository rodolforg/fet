/***************************************************************************
                          helptipsform.h  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef HELPTIPSFORM_H
#define HELPTIPSFORM_H

#include "ui_helptipsform_template.h"

class HelpTipsForm : public QDialog, public Ui::HelpTipsForm_template
{
	Q_OBJECT
	
public:
	HelpTipsForm(QWidget* parent);
	~HelpTipsForm();
	
	void setText();
};

#endif
