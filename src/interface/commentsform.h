//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2005 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef COMMENTSFORM_H
#define COMMENTSFORM_H

#include "ui_commentsform_template.h"

class CommentsForm : public QDialog, Ui::CommentsForm_template
{
	Q_OBJECT
public:
	CommentsForm(QWidget* parent);

	~CommentsForm();

public slots:
	void ok();
	void cancel();
};

#endif
