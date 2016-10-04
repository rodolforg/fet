/***************************************************************************
                          timetablegenerateform.cpp  -  description
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

#include "generate.h"

#include "timetablegenerateform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include <QString>

#include <QPlainTextEdit>

#include <QDateTime>
#include <QLocale>

#include <QMessageBox>

#include <QMutex>

#include <QDir>

#include <QApplication>

#include "longtextmessagebox.h"

QMutex myMutex;

static GenerateThread generateThread;

#include <QSemaphore>

QSemaphore semaphorePlacedActivity; //used to update when an activity is placed

QSemaphore finishedSemaphore;

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;

extern Solution highestStageSolution;

extern QString conflictsStringTitle;
extern QString conflictsString;

Generate gen;

QString initialOrderOfActivities;

int initialOrderOfActivitiesIndices[MAX_ACTIVITIES];

extern int maxActivitiesPlaced;

extern QDateTime generationStartDateTime;
extern QDateTime generationHighestStageDateTime;

QString getActivityDetailedDescription(Rules& r, int id);

const QString settingsName=QString("TimetableGenerateUnsuccessfulForm");

void GenerateThread::run()
{
	const int INF=2000000000;
	bool impossible, timeExceeded;

	gen.generate(INF, impossible, timeExceeded, true); //true means threaded
}

TimetableGenerateForm::TimetableGenerateForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentResultsTextEdit->setReadOnly(true);
	
	connect(startPushButton, SIGNAL(clicked()), this, SLOT(start()));
	connect(stopPushButton, SIGNAL(clicked()), this, SLOT(stop()));
	connect(writeResultsPushButton, SIGNAL(clicked()), this, SLOT(write()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(closePressed()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(seeImpossiblePushButton, SIGNAL(clicked()), this, SLOT(seeImpossible()));
	connect(seeInitialOrderPushButton, SIGNAL(clicked()), this, SLOT(seeInitialOrder()));
	connect(writeHighestStagePushButton, SIGNAL(clicked()), this, SLOT(writeHighestStage()));
	connect(stopHighestPushButton, SIGNAL(clicked()), this, SLOT(stopHighest()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	simulation_running=false;

	startPushButton->setDefault(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
	seeInitialOrderPushButton->setDisabled(true);

	connect(&gen, SIGNAL(activityPlaced(int)),
	 this, SLOT(activityPlaced(int)));
	connect(&gen, SIGNAL(simulationFinished()),
	 this, SLOT(simulationFinished()));
	connect(&gen, SIGNAL(impossibleToSolve()),
	 this, SLOT(impossibleToSolve()));
}

TimetableGenerateForm::~TimetableGenerateForm()
{
	saveFETDialogGeometry(this);
	if(simulation_running)
		this->stop();
}

void TimetableGenerateForm::start(){
	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure(this)){
			QMessageBox::warning(this, TimetableGenerateForm::tr("FET warning"), TimetableGenerateForm::tr("Data is wrong. Please correct and try again"));
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
			TimetableGenerateForm::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	currentResultsTextEdit->setPlainText(TimetableGenerateForm::tr("Entering simulation....precomputing, please be patient"));
	
	gen.abortOptimization=false;
	bool ok=gen.precompute(this);
	
	if(!ok){
		currentResultsTextEdit->setPlainText(TimetableGenerateForm::tr("Cannot generate - please modify your data"));
		currentResultsTextEdit->update();

		QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Your data cannot be processed - please modify it as instructed"));

		return;
	}

	startPushButton->setDisabled(true);
	stopPushButton->setEnabled(true);
	stopHighestPushButton->setEnabled(true);
	closePushButton->setDisabled(true);
	writeResultsPushButton->setEnabled(true);
	writeHighestStagePushButton->setEnabled(true);
	seeImpossiblePushButton->setEnabled(true);
	seeInitialOrderPushButton->setEnabled(true);

	simulation_running=true;
	
	gen.c.makeUnallocated(gt.rules);
	
	TimetableExport::writeRandomSeed(this, true); //true represents 'before' state

	generateThread.start();
}

void TimetableGenerateForm::stop()
{
	if(!simulation_running){
		return;
	}

	simulation_running=false;

	myMutex.lock();
	gen.abortOptimization=true;
	myMutex.unlock();

	myMutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults(this);

	QString s=TimetableGenerateForm::tr("Simulation interrupted! FET could not find a timetable."
	 " Maybe you can consider lowering the constraints.");

	s+=" ";
	
	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");

	s+=TimetableGenerateForm::tr("The partial results were saved in the directory %1")
	 .arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:");
	s+="\n\n";

	s+=tr("FET managed to schedule correctly the first %1 most difficult activities."
	 " You can see initial order of placing the activities in the generate dialog. The activity which might cause problems"
	 " might be the next activity in the initial order of evaluation. This activity is listed below:")
	 .arg(maxActivitiesPlaced);
	 
	s+="\n\n";
	
	s+=tr("Please check constraints related to following possibly problematic activity (or teacher(s), or students set(s)):");
	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";
	s+="\n";
	
	if(maxActivitiesPlaced>=0 && maxActivitiesPlaced<gt.rules.nInternalActivities 
	 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]<gt.rules.nInternalActivities){
		int ai=initialOrderOfActivitiesIndices[maxActivitiesPlaced];

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
	}
	else
		s+=tr("Difficult activity cannot be computed - please report possible bug");

	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Please check the constraints related to the last "
	 "activities in the list below, which might be impossible to schedule:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Here are the placed activities which lead to an inconsistency, "
	 "in order from the first one to the last (the last one FET failed to schedule "
	 "and the last ones are most likely impossible):");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=", ";

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation stopped", "The title of a dialog, meaning that the generation of the timetable was stopped."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));
	
	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
}

void TimetableGenerateForm::stopHighest()
{
	if(!simulation_running){
		return;
	}

	simulation_running=false;

	myMutex.lock();
	gen.abortOptimization=true;
	myMutex.unlock();

	myMutex.lock();

	Solution& c=highestStageSolution;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeHighestStageResults(this);

	QString s=TimetableGenerateForm::tr("Simulation interrupted! FET could not find a timetable."
	 " Maybe you can consider lowering the constraints.");

	s+=" ";
	
	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");

	s+=TimetableGenerateForm::tr("The partial highest-stage results were saved in the directory %1")
	 .arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:");
	s+="\n\n";

	s+=tr("FET managed to schedule correctly the first %1 most difficult activities."
	 " You can see initial order of placing the activities in the generate dialog. The activity which might cause problems"
	 " might be the next activity in the initial order of evaluation. This activity is listed below:")
	 .arg(maxActivitiesPlaced);
	 
	s+="\n\n";
	
	s+=tr("Please check constraints related to following possibly problematic activity (or teacher(s), or students set(s)):");
	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";
	s+="\n";
	
	if(maxActivitiesPlaced>=0 && maxActivitiesPlaced<gt.rules.nInternalActivities 
	 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]<gt.rules.nInternalActivities){
		int ai=initialOrderOfActivitiesIndices[maxActivitiesPlaced];

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
	}
	else
		s+=tr("Difficult activity cannot be computed - please report possible bug");

	s+="\n";

	myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);

	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation stopped (highest stage)", "The title of a dialog, meaning that the generation of the timetable was stopped "
		"and highest stage timetable written."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
}

void TimetableGenerateForm::impossibleToSolve()
{
	if(!simulation_running){
		return;
	}

	simulation_running=false;

	myMutex.lock();
	gen.abortOptimization=true;
	myMutex.unlock();

	myMutex.lock();


	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults(this);


	QString s=TimetableGenerateForm::tr("Simulation impossible! Maybe you can consider lowering the constraints.");

	s+=" ";

	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");

	s+=TimetableGenerateForm::tr("The partial results were saved in the directory %1")
	 .arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Please check the constraints related to the "
	 "activity below, which might be impossible to schedule:");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=", ";

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);

	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation impossible", "The title of a dialog, meaning that the generation of the timetable is impossible."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);

#ifndef Q_WS_QWS
	QApplication::beep();
#endif

	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
}

void TimetableGenerateForm::simulationFinished()
{
	if(!simulation_running){
		return;
	}

	simulation_running=false;

	finishedSemaphore.acquire();

	TimetableExport::writeRandomSeed(this, false); //false represents 'before' state

	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Soft conflicts", "Title of dialog");
	conflictsString="";

	conflictsString+=tr("Number of broken soft constraints: %1").arg(c.conflictsWeightList.count());

	conflictsString+="\n";

	conflictsString+=tr("Total soft conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Soft conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults(this);

	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");

/*	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Allocation terminated successfully, remaining %1 weighted"
		" soft conflicts from constraints with weight percentage lower than 100%"
		" (see menu Timetable/Show soft conflicts or the text file in"
		" the output directory for details)."
		"\n\nSimulation results should be now written. You may check now Timetable/View."
		" The results are also saved in the directory %2 in"
		" html and xml mode and the soft conflicts in txt mode").arg(c.conflictsTotal).arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk))
		+". "+tr("Data+timetable is saved as a .fet data file (with activities locked by constraints)"
		", so that you can open/modify/regenerate the current timetable later"));
*/

	QString s=QString("");
	s+=tr("Generation successful!");
	s+=QString("\n\n");
	s+=tr("Number of broken soft constraints: %1").arg(c.conflictsWeightList.count());
	s+=QString("\n");
	s+=tr("Weighted soft conflicts: %1").arg(CustomFETString::number(c.conflictsTotal));
	s+=QString("\n\n");
	s+=tr("Results were saved in the directory %1").arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk));

#ifndef Q_WS_QWS
	QApplication::beep();
#endif

	//Trick so that the message box will be silent (the only sound is thus the beep above).
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(TimetableGenerateForm::tr("FET information"));
	msgBox.setText(s);
	msgBox.exec();
	//QMessageBox::information(this, TimetableGenerateForm::tr("FET information"), s);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
}

void TimetableGenerateForm::activityPlaced(int na){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	myMutex.lock();
	int t=gen.searchTime; //seconds
	int mact=maxActivitiesPlaced;
	int seconds=gen.timeToHighestStage;
	myMutex.unlock();

	//write to the Qt interface
	QString s;
	s+=TimetableGenerateForm::tr("%1 out of %2 activities placed").arg(na).arg(gt.rules.nInternalActivities)+"\n";

	s+=TimetableGenerateForm::tr("Elapsed time:");
	int h=t/3600;
	if(h>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 h", "hours").arg(h);
	}
	t=t%3600;
	int m=t/60;
	if(m>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 m", "minutes").arg(m);
	}
	t=t%60;
	if(t>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 s", "seconds").arg(t);
	}
	
	bool zero=false;
	if(seconds==0)
		zero=true;
	int hh=seconds/3600;
	seconds%=3600;
	int mm=seconds/60;
	seconds%=60;
	int ss=seconds;

	QString tim;
	if(hh>0){
		tim+=" ";
		tim+=tr("%1 h", "hours").arg(hh);
	}
	if(mm>0){
		tim+=" ";
		tim+=tr("%1 m", "minutes").arg(mm);
	}
	if(ss>0 || zero){
		tim+=" ";
		tim+=tr("%1 s", "seconds").arg(ss);
	}
	tim.remove(0, 1);
	s+="\n\n";
	s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);

	currentResultsTextEdit->setPlainText(s);

	semaphorePlacedActivity.release();
}

void TimetableGenerateForm::help()
{
	QString s;
	
	s+=TimetableGenerateForm::tr("Please wait. It might take 5 to 20 minutes or even more for very difficult timetables");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Activities are placed in order, most difficult ones first");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("The process of searching is semi-randomized, which means that "
	 "you will get different timetables and running times each time. You can choose the best timetable from several runs");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Usually, there is no need to stop and restart the search."
	 " But for very difficult timetables this can help. Sometimes in such cases FET can become stuck and cycle forever,"
	 " and restarting might produce a very fast solution.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("It is recommended to strengthen the constraints step by step (for"
	 " instance min days between activities weight or teacher(s) max gaps), as you obtain feasible timetables.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("If your timetable gets stuck on a certain activity number k (and then"
	 " begins going back), please check the initial evaluation order and see activity number k+1 in this list. You may find"
	 " errors this way.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("If the generation is successful, you cannot have hard conflicts. You can have only soft conflicts,"
	 " corresponding to constraints with weight lower than 100.0%, which are reported in detail.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("After the generation (successful or interrupted), you can view the current (complete or incomplete) timetable"
	 " in the corresponding view timetable dialogs, and the list of conflicts in the view conflicts dialog.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("The results are saved in your selected results directory in HTML and XML mode and the soft conflicts"
	 " in text mode, along with the current data and timetable, saved as a .fet data file (with activities locked by constraints), so"
	 " that you can open, modify and regenerate the current timetable later");
	
	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void TimetableGenerateForm::write(){
	myMutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults(this);

	myMutex.unlock();

	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Simulation results should now be written in the directory %1 in html and xml mode"
		" and the conflicts in txt mode").arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk)));
}

void TimetableGenerateForm::writeHighestStage(){
	myMutex.lock();

	Solution& c=highestStageSolution;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeHighestStageResults(this);

	myMutex.unlock();

	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Highest stage results should now be written in the directory %1 in html and xml mode"
		" and the conflicts in txt mode").arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"timetables"+kk)));
}

void TimetableGenerateForm::closePressed()
{
	if(!generateThread.isRunning())
		this->close();
}

void TimetableGenerateForm::seeImpossible()
{
	QString s;

	myMutex.lock();

	s+=TimetableGenerateForm::tr("Information relating difficult to schedule activities:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Please check the constraints related to the last "
	 "activities in the list below, which might be difficult to schedule:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Here are the placed activities which lead to a difficulty, "
	 "in order from the first one to the last (the last one FET failed to schedule "
	 "and the last ones are difficult):");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=", ";

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	myMutex.unlock();
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - information about difficult activities"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
}

void TimetableGenerateForm::seeInitialOrder()
{
	QString s=initialOrderOfActivities;

	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - information about initial order of evaluation of activities"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
}

