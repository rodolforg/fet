/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      sparseitemmodel.cpp  -  description
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

#include "timetable_defs.h"

#include "sparseitemmodel.h"

SparseItemModel::SparseItemModel()
{
	n_rows=0;
	n_columns=0;

	items.clear();
	horizontalHeaderItems.clear();
	verticalHeaderItems.clear();
}

QModelIndex SparseItemModel::index ( int row, int column, const QModelIndex & parent ) const
{
	Q_UNUSED(parent);

	return createIndex(row, column);
}

QModelIndex SparseItemModel::parent ( const QModelIndex & index ) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int SparseItemModel::rowCount ( const QModelIndex & parent ) const
{
	if(parent!=QModelIndex())
		return 0; //qt doc says that

	return n_rows;
}

int SparseItemModel::columnCount ( const QModelIndex & parent ) const
{
	if(parent!=QModelIndex())
		return 0; //qt doc says that

	return n_columns;
}

QVariant SparseItemModel::data ( const QModelIndex & index, int role ) const
{
	if(role!=Qt::DisplayRole)
		return QVariant();

	int row=index.row();
	assert(row>=0 && row<n_rows);
	int column=index.column();
	assert(column>=0 && column<n_columns);
	
	QPair<int, int> pair;
	pair.first=row;
	pair.second=column;
	if(items.contains(pair))
		return items.value(pair);
	else
		return QVariant();
}

QVariant SparseItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if(role!=Qt::DisplayRole)
		return QVariant();

	if(orientation==Qt::Horizontal){
		assert(section>=0 && section<horizontalHeaderItems.count());
		return horizontalHeaderItems.at(section);
	}
	else{
		assert(section>=0 && section<verticalHeaderItems.count());
		return verticalHeaderItems.at(section);
	}
}

void SparseItemModel::clear()
{
	assert(n_rows>=0);
	if(n_rows>0){
		beginRemoveRows(QModelIndex(), 0, n_rows-1);
		n_rows=0;
		endRemoveRows();
	}
	
	assert(n_columns>=0);
	if(n_columns>0){
		beginRemoveColumns(QModelIndex(), 0, n_columns-1);
		n_columns=0;
		endRemoveColumns();
	}

	items.clear();
	horizontalHeaderItems.clear();
	verticalHeaderItems.clear();
}

void SparseItemModel::clearDataAndHeaders()
{
	items.clear();
	
	assert(n_columns==horizontalHeaderItems.count());
	for(int i=0; i<n_columns; i++)
		horizontalHeaderItems[i]="";
		
	assert(n_rows==verticalHeaderItems.count());
	for(int i=0; i<n_rows; i++)
		verticalHeaderItems[i]="";
}

void SparseItemModel::resize(int _nr, int _nc)
{
	if(_nr==n_rows && _nc==n_columns)
		return;

	this->clear();
	
	assert(verticalHeaderItems.count()==0);
	for(int i=0; i<_nr; i++)
		verticalHeaderItems.append("");
	assert(horizontalHeaderItems.count()==0);
	for(int i=0; i<_nc; i++)
		horizontalHeaderItems.append("");
	
	assert(n_rows==0);
	beginInsertRows(QModelIndex(), 0, _nr-1);
	n_rows=_nr;
	endInsertRows();

	assert(n_columns==0);
	beginInsertColumns(QModelIndex(), 0, _nc-1);
	n_columns=_nc;
	endInsertColumns();
}

void SparseItemModel::allItemsChanged()
{
	emit dataChanged(index(0,0), index(n_rows-1, n_columns-1));
}

void SparseItemModel::allHeadersChanged()
{
	emit headerDataChanged(Qt::Horizontal, 0, n_columns-1);
	emit headerDataChanged(Qt::Vertical, 0, n_rows-1);
}
