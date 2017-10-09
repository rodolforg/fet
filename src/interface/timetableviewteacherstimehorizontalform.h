/***************************************************************************
                          timetableviewteacherstimehorizontalform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWTEACHERSTIMEHORIZONTALFORM_H
#define TIMETABLEVIEWTEACHERSTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

class QColor; //by Marco Vassura

#include "ui_timetableviewteacherstimehorizontalform_template.h"

class TimetableViewTeachersTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
private:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn.
	
public:
	TimetableViewTeachersTimeHorizontalDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewTeachersTimeHorizontalForm : public QDialog, public Ui::TimetableViewTeachersTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewTeachersTimeHorizontalDelegate* newItemDelegate;

public:
	TimetableViewTeachersTimeHorizontalForm(QWidget* parent);
	~TimetableViewTeachersTimeHorizontalForm();
	
	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateTeachersTimetableTable();

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();
	
	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

protected:
//	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(QString s); //by Marco Vassura
};

#endif
