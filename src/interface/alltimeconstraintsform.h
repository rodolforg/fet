/***************************************************************************
                          alltimeconstraintsform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ALLTIMECONSTRAINTSFORM_H
#define ALLTIMECONSTRAINTSFORM_H

#include "ui_alltimeconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

class AllTimeConstraintsForm : public QDialog, Ui::AllTimeConstraintsForm_template  {
	Q_OBJECT
	
private:
	AdvancedFilterForm* filterForm;

	QList<TimeConstraint*> visibleTimeConstraintsList;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;
	
public:
	AllTimeConstraintsForm(QWidget* parent);
	~AllTimeConstraintsForm();
	
	bool filterOk(TimeConstraint* ctr);
	
	void filterChanged();
	
public slots:
	void constraintChanged();
	void modifyConstraint();
	void removeConstraint();
	void filter(bool active);

	void moveTimeConstraintUp();
	void moveTimeConstraintDown();

	void sortedChanged(bool checked);
	
	//void sortConstraintsByComments();
	void activateConstraint();
	void deactivateConstraint();
	void constraintComments();
};

#endif
