/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      sparsetableview.cpp  -  description
                             -------------------
    begin                : 2010
    copyright            : (C) 2010 by Liviu Lalescu
                         : http://lalescu.ro/liviu/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "sparsetableview.h"

#include "timetable_defs.h"

#include <QHeaderView>

#include <QHash>
#include <QPair>

#include <QSize>

SparseTableView::SparseTableView() : QTableView()
{
	this->setModel(&model);
}

int SparseTableView::maxHorizontalHeaderSectionSize()
{
	return this->width()/3;
}

void SparseTableView::resizeToFit()
{
	QHash<int, int> columnSizes;
	QHash<int, int> rowSizes;
	
	QHashIterator<QPair<int, int>, QString> i(model.items);
	while(i.hasNext()){
		i.next();
		
		QPair<int, int> pair=i.key();
		
		if(this->isRowHidden(pair.first))
			continue;
		if(this->isColumnHidden(pair.second))
			continue;
		
		QString str=i.value();
		
		QAbstractItemDelegate* delegate=this->itemDelegate(model.index(pair.first, pair.second));
		
		QSize size=delegate->sizeHint(this->viewOptions(), model.index(pair.first, pair.second));
		
		if(size.width() > columnSizes.value(pair.second, 0))
			columnSizes.insert(pair.second, size.width());
		if(size.height() > rowSizes.value(pair.first, 0))
			rowSizes.insert(pair.first, size.height());
	}
	
	horizontalSizesUntruncated.clear();
	
	for(int i=0; i<model.columnCount(); i++){
		int k=columnSizes.value(i, 0);
		if(this->showGrid())
			k++;
		if(k<this->horizontalHeader()->sectionSizeHint(i))
			k=this->horizontalHeader()->sectionSizeHint(i);
			
		//not too wide
		horizontalSizesUntruncated.append(k);
		
		if(k>maxHorizontalHeaderSectionSize()){
			k=maxHorizontalHeaderSectionSize();
		}
			
		this->horizontalHeader()->resizeSection(i, k);
	}

	for(int i=0; i<model.rowCount(); i++){
		int k=rowSizes.value(i, 0);
		if(this->showGrid())
			k++;
		if(k<this->verticalHeader()->sectionSizeHint(i))
			k=this->verticalHeader()->sectionSizeHint(i);
		this->verticalHeader()->resizeSection(i, k);
	}
}

/*void SparseTableView::itWasResized()
{
	assert(model.columnCount()==horizontalSizesUntruncated.count());
	for(int i=0; i<model.columnCount(); i++){
		int size=horizontalSizesUntruncated.at(i);
		
		if(size>maxHorizontalHeaderSectionSize())
			size=maxHorizontalHeaderSectionSize();
		
		if(this->horizontalHeader()->sectionSize(i)!=size)
			this->horizontalHeader()->resizeSection(i, size);
	}
}*/

void SparseTableView::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);
	
	//So that if user resizes horizontal header and scroll bar appears, no automatic resize is done.
	if(event){
		if(event->size().width()==event->oldSize().width())
			return;
	}
	
	//this->itWasResized();
	if(model.columnCount()!=horizontalSizesUntruncated.count()){ //do NOT remove this test, or you will get assertion failed a few lines below, when user clicks "Swap axes".
	//This situation can appear if user clicks "Swap axes", and it is taken care of then, so we can return in this case, safely assuming that the table will be redrawn correctly.
		//cout<<"different sizes"<<endl;
		return;
	}
	assert(model.columnCount()==horizontalSizesUntruncated.count());
	for(int i=0; i<model.columnCount(); i++){
		int size=horizontalSizesUntruncated.at(i);
		
		if(size>maxHorizontalHeaderSectionSize())
			size=maxHorizontalHeaderSectionSize();
		
		if(this->horizontalHeader()->sectionSize(i)!=size)
			this->horizontalHeader()->resizeSection(i, size);
	}
}

void SparseTableView::allTableChanged()
{
	model.allItemsChanged();
	model.allHeadersChanged();
}
