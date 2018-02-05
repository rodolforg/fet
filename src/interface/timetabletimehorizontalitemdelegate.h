/***************************************************************************
                          timetabletimehorizontalitemdelegate.h  -  description
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

#ifndef TIMETABLETIMEHORIZONTALITEMDELEGATE_H
#define TIMETABLETIMEHORIZONTALITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

class TimetableTimeHorizontalItemDelegate: public QStyledItemDelegate
{
	Q_OBJECT

private:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn.

public:
	TimetableTimeHorizontalItemDelegate(QWidget* parent, int _nRows, int _nColumns);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // TIMETABLETIMEHORIZONTALITEMDELEGATE_H
