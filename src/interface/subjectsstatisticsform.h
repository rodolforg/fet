/***************************************************************************
                          subjectsstatisticsform.h  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#ifndef SUBJECTSSTATISTICSFORM_H
#define SUBJECTSSTATISTICSFORM_H

#include "ui_subjectsstatisticsform_template.h"

#include <QDialog>

class SubjectsStatisticsForm : public QDialog, Ui::SubjectsStatisticsForm_template  {
	Q_OBJECT

public:
	SubjectsStatisticsForm(QWidget* parent);
	~SubjectsStatisticsForm();
};

#endif
