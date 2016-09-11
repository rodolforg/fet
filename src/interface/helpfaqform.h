/***************************************************************************
                          helpfaqform.h  -  description
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

#ifndef HELPFAQFORM_H
#define HELPFAQFORM_H

#include "ui_helpfaqform_template.h"

class HelpFaqForm : public QDialog, public Ui::HelpFaqForm_template
{
	Q_OBJECT
	
public:
	HelpFaqForm(QWidget* parent);
	~HelpFaqForm();
	
	void setText();
};

#endif
