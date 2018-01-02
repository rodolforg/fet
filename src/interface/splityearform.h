/***************************************************************************
                          splityearform.h  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef SPLITYEARFORM_H
#define SPLITYEARFORM_H

#include <QString>

#include "ui_splityearform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

const int MAX_CATEGORIES=14;
const int MAX_SUBGROUPS_PER_YEAR=MAX_TOTAL_SUBGROUPS;

class SplitYearForm : public QDialog, Ui::SplitYearForm_template  {
	Q_OBJECT

private:
	QString _sep;
	int _nCategories;
	int _nDivisions[MAX_CATEGORIES];
	QStringList _divisions[MAX_CATEGORIES];
	
	QListWidget* listWidgets[MAX_CATEGORIES];
	
public:
	QString year;
	
	SplitYearForm(QWidget* parent, const QString& _year);
	~SplitYearForm();
	
	void updateNumberOfSubgroups();
	void updateDivisionsLabel();
	
public slots:
	void tabIndexChanged(int i);
	
	void ok();
	void numberOfCategoriesChanged();
	
	void addClicked();
	void modifyClicked();
	void modifyDoubleClicked(int i);
	void removeClicked();
	void removeAllClicked();
	
	void list0DoubleClicked();
	void list1DoubleClicked();
	void list2DoubleClicked();
	void list3DoubleClicked();
	void list4DoubleClicked();
	void list5DoubleClicked();
	void list6DoubleClicked();
	void list7DoubleClicked();
	void list8DoubleClicked();
	void list9DoubleClicked();
	void list10DoubleClicked();
	void list11DoubleClicked();
	void list12DoubleClicked();
	void list13DoubleClicked();
	
	void help();
	void reset();
};

#endif
