//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBJECTSFORM_H
#define SUBJECTSFORM_H

#include "ui_subjectsform_template.h"

class SubjectsForm : public QDialog, Ui::SubjectsForm_template
{
	Q_OBJECT
public:
	SubjectsForm(QWidget* parent);

	~SubjectsForm();

public slots:
	void addSubject();
	void removeSubject();
	void renameSubject();
	
	void moveSubjectUp();
	void moveSubjectDown();
	
	void sortSubjects();
	
	void subjectChanged(int index);
	
	void activateSubject();
	void deactivateSubject();
	
	void comments();
};

#endif
