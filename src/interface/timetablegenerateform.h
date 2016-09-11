/***************************************************************************
                          timetablegenerateform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef TIMETABLEGENERATEFORM_H
#define TIMETABLEGENERATEFORM_H

#include "ui_timetablegenerateform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QThread>

class GenerateThread: public QThread{
	Q_OBJECT

public:
	void run();

signals:
	void generationComputed(int generation);
};

class TimetableGenerateForm : public QDialog, Ui::TimetableGenerateForm_template  {
	Q_OBJECT

public:

	TimetableGenerateForm(QWidget* parent);

	~TimetableGenerateForm();

public slots:
	void start();
	void stop();
	void stopHighest();
	void write();
	void closePressed();
	void help();
	void writeHighestStage();

	//void generationLogging(int generation);

private slots:
	void activityPlaced(int na);

	void simulationFinished();

	void impossibleToSolve();
	
	void seeImpossible();
	
	void seeInitialOrder();
	
//	void seeHighestStage();
};

#endif
