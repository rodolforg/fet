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

#include <QString>
#include <QSet>
//#include <QHash>
#include <QStringList>

class QColor; //by Marco Vassura

#include "ui_timetableviewstudentstimehorizontalform_template.h"
#include "timetabletimehorizontalitemdelegate.h"

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
	TimetableTimeHorizontalItemDelegate* newItemDelegate;

public:
	TimetableViewStudentsTimeHorizontalForm(QWidget* parent);
	void newTimetableGenerated();
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

private:
	/** Search for StudentSet in Rules::internalSubgroupsList
	 * @param row index in TimetableView row
	 * @returns index on Rules::internalSubgroupsList, or -1 on failure.
	 */
	int getStudentSetInternalIndexFromRow(int row);
};

#endif
