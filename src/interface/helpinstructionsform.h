/***************************************************************************
                          helpinstructionsform.h  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef HELPINSTRUCTIONSFORM_H
#define HELPINSTRUCTIONSFORM_H

#include "ui_helpinstructionsform_template.h"

class HelpInstructionsForm : public QDialog, public Ui::HelpInstructionsForm_template
{
	Q_OBJECT
	
public:
	HelpInstructionsForm(QWidget* parent);
	~HelpInstructionsForm();
	
	void setText();
};

#endif
