/***************************************************************************
                          timetablegeneratemultipleform.cpp  -  description
                             -------------------
    begin                : Aug 20, 2007
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

#include "longtextmessagebox.h"

#include "generate.h"

#include "timetablegeneratemultipleform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include <ctime>

#include <QMessageBox>

#include <QMutex>

#include <QScrollBar>

#include <QDir>

#include <QApplication>

extern QMutex myMutex;

static GenerateMultipleThread generateMultipleThread;

#include <QSemaphore>

static QSemaphore semaphoreTimetableFinished; 

static QSemaphore semaphoreTimetableStarted;

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running_multi;

extern QSemaphore semaphorePlacedActivity;

Generate genMulti;

static int nTimetables;
static int timeLimit;

extern int maxActivitiesPlaced;

extern Solution best_solution;

extern QString conflictsStringTitle;
extern QString conflictsString;

static time_t start_time;

static time_t initial_time;

extern int permutation[MAX_ACTIVITIES];
int savedPermutation[MAX_ACTIVITIES];

void GenerateMultipleThread::run()
{
	genMulti.abortOptimization=false;
	
	time(&initial_time);

	for(int i=0; i<nTimetables; i++){
		time(&start_time);
	
		bool impossible;
		bool timeExceeded;
		
		for(int qq=0; qq<gt.rules.nInternalActivities; qq++)
			permutation[qq]=savedPermutation[qq];
			
		emit(timetableStarted(i+1));
		semaphoreTimetableStarted.acquire();

		genMulti.generate(timeLimit, impossible, timeExceeded, true); //true means threaded

		QString s;
		
		bool ok;

		myMutex.lock();
		if(genMulti.abortOptimization){
			myMutex.unlock();
			return;
		}
		else if(impossible){
			s=tr("Timetable impossible to generate");
			s+=QString(".");
			ok=false;
		}
		else if(timeExceeded){
			s=tr("Time exceeded for current timetable");

			////////2011-05-26
			int mact=maxActivitiesPlaced;
			int mseconds=genMulti.timeToHighestStage;

			bool zero=false;
			if(mseconds==0)
				zero=true;
			int hh=mseconds/3600;
			mseconds%=3600;
			int mm=mseconds/60;
			mseconds%=60;
			int ss=mseconds;

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
			s+=QString(". ");
			s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);
			///////

			s+=QString(".");

			ok=false;
		}
		else{
			ok=true;
			
			time_t finish_time;
			time(&finish_time);
			int seconds=int(finish_time-start_time);
			int hours=seconds/3600;
			seconds%=3600;
			int minutes=seconds/60;
			seconds%=60;
			
			QString tmp;
			genMulti.c.fitness(gt.rules, &tmp);
			
			s=tr("Timetable has %1 soft conflicts factor and was generated in %2 hours, %3 minutes and %4 seconds")
			 .arg(CustomFETString::number(genMulti.c.conflictsTotal))
			 .arg(hours)
			 .arg(minutes)
			 .arg(seconds);

			s+=QString(".");
		}
		myMutex.unlock();
		
		emit(timetableGenerated(i+1, s, ok));
		semaphoreTimetableFinished.acquire();
	}
	
	emit(finished());
}

TimetableGenerateMultipleForm::TimetableGenerateMultipleForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentResultsTextEdit->setReadOnly(true);
	
	startPushButton->setDefault(true);

	connect(startPushButton, SIGNAL(clicked()), this, SLOT(start()));
	connect(stopPushButton, SIGNAL(clicked()), this, SLOT(stop()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(closePressed()));
	connect(pushButton4, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	simulation_running_multi=false;

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);

	connect(&generateMultipleThread, SIGNAL(timetableGenerated(int, const QString&, bool)),
		this, SLOT(timetableGenerated(int, const QString&, bool)));

	connect(&generateMultipleThread, SIGNAL(timetableStarted(int)),
		this, SLOT(timetableStarted(int)));

	connect(&generateMultipleThread, SIGNAL(finished()),
		this, SLOT(finished()));

	connect(&genMulti, SIGNAL(activityPlaced(int)),
		this, SLOT(activityPlaced(int)));
}

TimetableGenerateMultipleForm::~TimetableGenerateMultipleForm()
{
	saveFETDialogGeometry(this);
	if(simulation_running_multi)
		this->stop();
}

void TimetableGenerateMultipleForm::help()
{
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";

	QString s=tr("You can only see generated timetables on the hard disk,"
	 " in HTML and XML formats and soft conflicts in text format, or latest timetable in the Timetable/View menu."
	 " It is needed that the directory"
	 " %1 to be emptied and deleted before proceeding.").arg(QDir::toNativeSeparators(destDir))
	 +"\n\n"
	 +tr("Note that, for large data, each timetable might occupy more"
	 " megabytes of hard disk space,"
	 " so make sure you have enough space (you can check the dimension of a single timetable as a precaution).")
	 +"\n\n"
	 +tr("There are also saved the timetables in .fet format (data + constraints to lock the timetable), so that you can open each of them later.")
	 +"\n\n"
	 +tr("If you get impossible timetable, please enter menu Generate (single) and see the initial order of evaluation of activities,"
	 " this might help.")
	 +"\n\n"
	 +tr("You can limit the search time, by specifying the maximum number of minutes allowed to spend for each timetable (option %1).").arg("'"+tr("Limit for each timetable")+"'")
	 +" "+tr("The maximum and also the predefined value is %1 minutes, which means %2 hours, so virtually unlimited.").arg(60000).arg(1000)
	 ;
	 
	 LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void TimetableGenerateMultipleForm::start(){
	nTimetables=timetablesSpinBox->value();
	assert(nTimetables>0);
	timeLimit=60*minutesSpinBox->value(); //seconds
	assert(timeLimit>0);

	QDir dir;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";
	if(dir.exists(destDir)){
		QMessageBox::warning(this, tr("FET information"), tr("Directory %1 exists and might not be empty,"
		 " (it might contain old files). You need to manually remove all contents of this directory AND the directory itself (or rename it)"
		 " and then you can generate multiple timetables")
		 .arg(QDir::toNativeSeparators(destDir)));
		 
		return;
	}

	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure(this)){
			QMessageBox::warning(this, TimetableGenerateMultipleForm::tr("FET warning"), TimetableGenerateMultipleForm::tr("Data is wrong. Please correct and try again"));
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
			TimetableGenerateMultipleForm::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	currentResultsTextEdit->setPlainText("");

	bool ok=genMulti.precompute(this);
	if(!ok){
		currentResultsTextEdit->setPlainText(TimetableGenerateMultipleForm::tr("Cannot optimize - please modify your data"));
		currentResultsTextEdit->update();

		QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Your data cannot be processed - please modify it as instructed."));

		return;
	}

	startPushButton->setDisabled(true);
	stopPushButton->setEnabled(true);
	minutesGroupBox->setDisabled(true);
	timetablesGroupBox->setDisabled(true);
	closePushButton->setDisabled(true);

	simulation_running_multi=true;

	for(int qq=0; qq<gt.rules.nInternalActivities; qq++)
		savedPermutation[qq]=permutation[qq];
		
	genMulti.c.makeUnallocated(gt.rules);

	generateMultipleThread.start();
}

void TimetableGenerateMultipleForm::timetableStarted(int timetable)
{
	TimetableExport::writeRandomSeed(this, timetable, true); //true represents 'before' state
	
	semaphoreTimetableStarted.release();
}

void TimetableGenerateMultipleForm::timetableGenerated(int timetable, const QString& description, bool ok)
{
	TimetableExport::writeRandomSeed(this, timetable, false); //false represents 'before' state

	QString s=QString("");
	s+=tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description").arg(timetable).arg(description);
	currentResultsTextEdit->appendPlainText(s);
	
	bool begin;
	if(timetable==1)
		begin=true;
	else
		begin=false;
	TimetableExport::writeReportForMultiple(this, s, begin);

	if(ok){
		//needed to get the conflicts string
		QString tmp;
		genMulti.c.fitness(gt.rules, &tmp);
	
		TimetableExport::getStudentsTimetable(genMulti.c);
		TimetableExport::getTeachersTimetable(genMulti.c);
		TimetableExport::getRoomsTimetable(genMulti.c);

		TimetableExport::writeSimulationResults(this, timetable);

		//update the string representing the conflicts
		conflictsStringTitle=tr("Soft conflicts", "Title of dialog");
		conflictsString = "";
		conflictsString+=tr("Total soft conflicts:");
		conflictsString+=" ";
		conflictsString+=CustomFETString::number(best_solution.conflictsTotal);
		conflictsString+="\n";
		conflictsString += tr("Soft conflicts listing (in decreasing order):")+"\n";

		foreach(QString t, best_solution.conflictsDescriptionList)
			conflictsString+=t+"\n";
	}

	semaphoreTimetableFinished.release();
}

void TimetableGenerateMultipleForm::stop()
{
	if(!simulation_running_multi){
		return;
	}

	simulation_running_multi=false;

	myMutex.lock();
	genMulti.abortOptimization=true;
	myMutex.unlock();

	QString s=TimetableGenerateMultipleForm::tr("Simulation interrupted!");
	s+="\n\n";

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";

	time_t final_time;
	time(&final_time);
	int sec=int(final_time-initial_time);
	int h=sec/3600;
	sec%=3600;
	int m=sec/60;
	sec%=60;

	s+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1").arg(QDir::toNativeSeparators(destDir));
	s+="\n\n";
	s+=tr("Total searching time: %1h %2m %3s").arg(h).arg(m).arg(sec);
	
	TimetableExport::writeReportForMultiple(this, QString("\n")+s, false);

	QMessageBox::information(this, tr("FET information"), s);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);
	closePushButton->setEnabled(true);
}

void TimetableGenerateMultipleForm::finished()
{
	simulationFinished();
}

void TimetableGenerateMultipleForm::simulationFinished()
{
	if(!simulation_running_multi){
		return;
	}

	simulation_running_multi=false;

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";
	
	time_t final_time;
	time(&final_time);
	int s=int(final_time-initial_time);
	int h=s/3600;
	s%=3600;
	int m=s/60;
	s%=60;

	QString ms=QString("");
	ms+=TimetableGenerateMultipleForm::tr("Simulation terminated successfully!");
	ms+=QString("\n\n");
	ms+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1").arg(QDir::toNativeSeparators(destDir));
	ms+=QString("\n\n");
	ms+=TimetableGenerateMultipleForm::tr("Total searching time was %1h %2m %3s").arg(h).arg(m).arg(s);
	
	TimetableExport::writeReportForMultiple(this, QString("\n")+ms, false);

#ifndef Q_WS_QWS
	QApplication::beep();
#endif
	
	//Trick so that the message box will be silent (the only sound is thus the beep above).
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(TimetableGenerateMultipleForm::tr("FET information"));
	msgBox.setText(ms);
	msgBox.exec();
	//QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"), ms);
	
	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);
	closePushButton->setEnabled(true);
}

void TimetableGenerateMultipleForm::activityPlaced(int na)
{
	time_t finish_time;
	time(&finish_time);
	int seconds=int(finish_time-start_time);
	int hours=seconds/3600;
	seconds%=3600;
	int minutes=seconds/60;
	seconds%=60;
			
	////////2011-05-26
	int mact=maxActivitiesPlaced;
	int mseconds=genMulti.timeToHighestStage;

	QString s;

	bool zero=false;
	if(mseconds==0)
		zero=true;
	int hh=mseconds/3600;
	mseconds%=3600;
	int mm=mseconds/60;
	mseconds%=60;
	int ss=mseconds;

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
	s+=QString("\n");
	s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);
	///////
	
	textLabel->setText(tr("Current timetable: %1 out of %2 activities placed, %3h %4m %5s")
	 .arg(na)
	 .arg(gt.rules.nInternalActivities)
	 .arg(hours)
	 .arg(minutes)
	 .arg(seconds)+s);
	
	semaphorePlacedActivity.release();
}

void TimetableGenerateMultipleForm::closePressed()
{
	if(!generateMultipleThread.isRunning())
		this->close();
}
