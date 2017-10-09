/***************************************************************************
                          timetableviewstudentstimehorizontalform.h  -  description
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

#ifndef TIMETABLEVIEWSTUDENTSTIMEHORIZONTALFORM_H
#define TIMETABLEVIEWSTUDENTSTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include <QString>
#include <QSet>
//#include <QHash>
#include <QStringList>

class QColor; //by Marco Vassura

//class ConstraintStudentsSetNotAvailableTimes;

#include "ui_timetableviewstudentstimehorizontalform_template.h"

class TimetableViewStudentsTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
private:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn
	
public:
	TimetableViewStudentsTimeHorizontalDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewStudentsTimeHorizontalForm : public QDialog, public Ui::TimetableViewStudentsTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QStringList usedStudentsList;
	QSet<QString> usedStudentsSet;
	//QHash<QString, QList<int> > activitiesForStudentsSet; //activity index in internal activities list
	
	//QHash<QString, ConstraintStudentsSetNotAvailableTimes*> notAvailableHash;

	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewStudentsTimeHorizontalDelegate* newItemDelegate;

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
