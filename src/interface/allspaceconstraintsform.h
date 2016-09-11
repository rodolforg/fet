/***************************************************************************
                          allspaceconstraintsform.h  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#ifndef ALLSPACECONSTRAINTSFORM_H
#define ALLSPACECONSTRAINTSFORM_H

#include "ui_allspaceconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

class AllSpaceConstraintsForm : public QDialog, Ui::AllSpaceConstraintsForm_template  {
	Q_OBJECT

private:
	AdvancedFilterForm* filterForm;

	QList<SpaceConstraint*> visibleSpaceConstraintsList;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;
	
public:
	AllSpaceConstraintsForm(QWidget* parent);
	~AllSpaceConstraintsForm();

	bool filterOk(SpaceConstraint* ctr);
	
	void filterChanged();
	
public slots:
	void constraintChanged();
	void modifyConstraint();
	void removeConstraint();
	void filter(bool active);

	void moveSpaceConstraintUp();
	void moveSpaceConstraintDown();

	void sortedChanged(bool checked);

	//void sortConstraintsByComments();
	void activateConstraint();
	void deactivateConstraint();
	void constraintComments();
};

#endif
