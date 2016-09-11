/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      sparsetableview.h  -  description
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

#ifndef SPARSETABLEVIEW_H
#define SPARSETABLEVIEW_H

#include <QTableView>

#include <QResizeEvent>

#include "sparseitemmodel.h"

class SparseTableView: public QTableView{
private:
	QList<int> horizontalSizesUntruncated;
	
	int maxHorizontalHeaderSectionSize();

public:
	SparseItemModel model;

	SparseTableView();

	void resizeToFit();
	
	//void itWasResized();
	
	void allTableChanged();
	
protected:
	void resizeEvent(QResizeEvent* event);
};

#endif
