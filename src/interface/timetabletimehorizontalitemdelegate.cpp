/***************************************************************************
                          timetabletimehorizontalitemdelegate.cpp  -  description
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

#include "timetabletimehorizontalitemdelegate.h"

#include <QPainter>

TimetableTimeHorizontalItemDelegate::TimetableTimeHorizontalItemDelegate(QWidget* parent, int _nRows, int _nColumns)
	: QStyledItemDelegate(parent)
{
	nRows=_nRows;
	nColumns=_nColumns;
}

void TimetableTimeHorizontalItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	//int day=index.column()/gt.rules.nHoursPerDay;
	int hour=index.column()%nColumns;

	/*if(day>=0 && day<gt.rules.nDaysPerWeek-1 && hour==gt.rules.nHoursPerDay-1){
		QPen pen(painter->pen());
		pen.setWidth(2);
		painter->setPen(pen);
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
	}*/

	/*assert(table!=NULL);
	QBrush bg(table->item(index.row(), index.column())->background());
	QPen pen(painter->pen());

	double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
	if (brightness<0.5)
		pen.setColor(Qt::white);
	else
		pen.setColor(Qt::black);

	painter->setPen(pen);*/

	if(hour==0)
		painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
	if(hour==nColumns-1)
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

	if(index.row()==0)
		painter->drawLine(option.rect.topLeft(), option.rect.topRight());
	if(index.row()==nRows-1)
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
}
