/***************************************************************************
                          activitiesroomsstatisticsform.h  -  description
                             -------------------
    begin                : October 6, 2011
    copyright            : (C) 2011 by Lalescu Liviu
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

#ifndef ACTIVITIESROOMSSTATISTICSFORM_H
#define ACTIVITIESROOMSSTATISTICSFORM_H

#include "ui_activitiesroomsstatisticsform_template.h"

#include <QDialog>

class ActivitiesRoomsStatisticsForm : public QDialog, Ui::ActivitiesRoomsStatisticsForm_template  {
	Q_OBJECT

public:
	ActivitiesRoomsStatisticsForm(QWidget* parent);
	~ActivitiesRoomsStatisticsForm();
};

#endif
