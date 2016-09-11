/***************************************************************************
                          constraintsubjectactivitytagpreferredroomsform.h  -  description
                             -------------------
    begin                : 18 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef CONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMSFORM_H
#define CONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMSFORM_H

#include "ui_constraintsubjectactivitytagpreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintSubjectActivityTagPreferredRoomsForm : public QDialog, Ui::ConstraintSubjectActivityTagPreferredRoomsForm_template  {
	Q_OBJECT
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintSubjectActivityTagPreferredRoomsForm(QWidget* parent);
	~ConstraintSubjectActivityTagPreferredRoomsForm();

	void refreshConstraintsListWidget();

	bool filterOk(SpaceConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void removeConstraint();
	void modifyConstraint();
	
	void filterChanged();
};

#endif
