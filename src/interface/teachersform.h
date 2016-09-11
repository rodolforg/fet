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

#ifndef TEACHERSFORM_H
#define TEACHERSFORM_H

#include "ui_teachersform_template.h"

class TeachersForm : public QDialog, Ui::TeachersForm_template
{
	Q_OBJECT
public:
	TeachersForm(QWidget* parent);

	~TeachersForm();

public slots:
	void addTeacher();
	void removeTeacher();
	void renameTeacher();
	
	void targetNumberOfHours();
	void qualifiedSubjects();

	void moveTeacherUp();
	void moveTeacherDown();

	void sortTeachers();
	
	void teacherChanged(int index);
	
	void activateTeacher();
	void deactivateTeacher();
	
	void comments();
};

#endif
