/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      sparseitemmodel.h  -  description
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

#ifndef SPARSEITEMMODEL_H
#define SPARSEITEMMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QPair>
#include <QString>
#include <QStringList>

class SparseItemModel: public QAbstractItemModel{
	int n_rows;
	int n_columns;

public:

	QHash<QPair<int, int> , QString> items;
	QStringList horizontalHeaderItems;
	QStringList verticalHeaderItems;

	SparseItemModel();

	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const ;
	QModelIndex parent ( const QModelIndex & index ) const ;
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const ;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const ;
	
	void clear();
	void clearDataAndHeaders();
	void resize(int _nr, int _nc);
	void allItemsChanged();
	void allHeadersChanged();
};

#endif
