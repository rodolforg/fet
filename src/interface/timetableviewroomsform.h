/***************************************************************************
                          timetableviewroomsform.h  -  description
                             -------------------
    begin                : Wed May 14 2003
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

#ifndef TIMETABLEVIEWROOMSFORM_H
#define TIMETABLEVIEWROOMSFORM_H

#include "ui_timetableviewroomsform_template.h"

#include <QResizeEvent>

class QColor; //by Marco Vassura

class TimetableViewRoomsForm : public QDialog, public Ui::TimetableViewRoomsForm_template
{
	Q_OBJECT
	
public:
	TimetableViewRoomsForm(QWidget* parent);
	~TimetableViewRoomsForm();

	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void updateRoomsTimetableTable();

	void roomChanged(const QString& roomName);

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
	
	void lock();
	void lockTime();
	void lockSpace();
	void help();
	
protected:
	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(QString s); //by Marco Vassura
};

#endif
