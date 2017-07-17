/***************************************************************************
                          timetableviewstudentstimehorizontalform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
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

#ifndef TIMETABLEVIEWSTUDENTSTIMEHORIZONTALFORM_H
#define TIMETABLEVIEWSTUDENTSTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QStyledItemDelegate>

class QColor; //by Marco Vassura

#include "ui_timetableviewstudentstimehorizontalform_template.h"

class TimetableViewStudentsTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
public:
	TimetableViewStudentsTimeHorizontalDelegate(QWidget* parent=NULL): QStyledItemDelegate(parent){}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override; //As in a Qt example
};

class TimetableViewStudentsTimeHorizontalForm : public QDialog, public Ui::TimetableViewStudentsTimeHorizontalForm_template
{
	Q_OBJECT

public:
	TimetableViewStudentsTimeHorizontalForm(QWidget* parent);
	~TimetableViewStudentsTimeHorizontalForm();
	
	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateStudentsTimetableTable();

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();
	
	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();
	
protected:
//	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(QString s); //by Marco Vassura
};

#endif
