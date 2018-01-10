/*
File export.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          export.cpp  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

// Code contributed by Volker Dirr ( http://www.timetabling.de/ )

//TODO: protect export strings. textquote must be doubled
//TODO: count skipped min days constraints?
//TODO: add cancel button

#include <QtGlobal>
#include <QFile>
#include <QDir>

#ifndef FET_COMMAND_LINE
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QMessageBox>
#endif

#include <QHash>
#include <QSet>
#include <QTextStream>

void centerWidgetOnScreen(QWidget* widget);

#include "export.h"
#include "solution.h"

static const char CSVActivities[]="activities.csv";
static const char CSVActivitiesStatistic[]="statistics_activities.csv";
static const char CSVActivityTags[]="activity_tags.csv";
static const char CSVRoomsAndBuildings[]="rooms_and_buildings.csv";
static const char CSVSubjects[]="subjects.csv";
static const char CSVTeachers[]="teachers.csv";
static const char CSVStudents[]="students.csv";
static const char CSVTimetable[]="timetable.csv";

#ifdef FET_COMMAND_LINE
#include <iostream>
using namespace std;
#endif

static QString getBasename(){
	QFileInfo input(INPUT_FILENAME_XML);
	if (input.suffix() == "fet")
		return input.baseName();
	return input.fileName();
}

QString Export::getFilePath(QString suffix) {
	QString basename = getBasename();
	if (!basename.isEmpty())
		basename.append("_");
	QString path = directoryCSV;
	if (!directoryCSV.endsWith(FILE_SEP))
		path.append(FILE_SEP);
	return path+basename+suffix;
}

#ifndef FET_COMMAND_LINE
Export::Export(const Timetable &gt, const Solution &solution)
	: gt(gt), solution(solution),
	  textquote("\""), fieldSeparator(","),
	  header(true), setSeparator("+"),
	  overwrite(OVERWRITE_PROMPT)
{
}
#else
Export::Export(const Timetable &gt)
	: gt(gt),
	  textquote("\""), fieldSeparator(","),
	  header(true), setSeparator("+"),
	  overwrite(OVERWRITE_NONE)
{
}
#endif

Export::~Export()
{
}

#ifndef FET_COMMAND_LINE
bool Export::okToWrite(QWidget* parent, const QString& file)
{
	if(QFile::exists(file)){
		if(overwrite==OVERWRITE_ALL){
			return true;
		}
		else if(overwrite==OVERWRITE_NONE){
			return false;
		}
		else if(overwrite==OVERWRITE_PROMPT){
		
			QMessageBox msgBox(parent);
			msgBox.setWindowTitle(tr("FET warning"));
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setText(tr("File %1 exists - are you sure you want to overwrite existing file?").arg(file));
			msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::YesToAll|QMessageBox::NoToAll);
			msgBox.setDefaultButton(QMessageBox::Yes);
			
			QMessageBox::StandardButton msgBoxButton=((QMessageBox::StandardButton)(msgBox.exec()));

			switch (msgBoxButton) {
			case QMessageBox::YesToAll:
				overwrite = OVERWRITE_ALL;
				return true;
			case QMessageBox::Yes:
				return true;
			case QMessageBox::NoToAll:
				overwrite = OVERWRITE_NONE;
				return false;
			case QMessageBox::No:
				return false;
			default:
				return false;
			}
		}
		else{
			assert(0);
			return false;
		}
	}
	else
		return true;
}
#else
bool Export::okToWrite(const QString& file)
{
	if(QFile::exists(file))
		return overwrite == OVERWRITE_ALL;
	else
		return true;
}
#endif

#ifndef FET_COMMAND_LINE
void Export::exportCSV(QWidget* parent) {
	bool ok=true;

	if (directoryCSV.isNull())
		directoryCSV=OUTPUT_DIR+FILE_SEP+"csv";

	QString s2=getBasename();
	if(s2.isEmpty())
		s2="unnamed";
	directoryCSV.append(FILE_SEP+s2+FILE_SEP);

	QDir dir;
	if(!dir.exists(directoryCSV))
		dir.mkpath(directoryCSV);

	QDialog* newParent;
	ok=selectSeparatorAndTextquote(parent, newParent, textquote, fieldSeparator, header);
	
	QString lastWarnings;
	if(!ok){
		lastWarnings.insert(0,Export::tr("Export aborted")+"\n");
	} else {
		bool okat, okr, oks, okt, okst, okact, okacts, oktim;

		OverwriteOptions previousOverwriteOption = overwrite;

		okat=exportCSVActivityTags(newParent);
		okr=exportCSVRoomsAndBuildings(newParent);
		oks=exportCSVSubjects(newParent);
		okt=exportCSVTeachers(newParent);
		okst=exportCSVStudents(newParent);
		okact=exportCSVActivities(newParent);
		okacts=exportCSVActivitiesStatistic(newParent);
		oktim=exportCSVTimetable(newParent);

		overwrite = previousOverwriteOption;
		
		ok=okat && okr && oks && okt && okst && okact && okacts && oktim;
			
		lastWarnings.insert(0,Export::tr("CSV files were exported to directory %1.").arg(QDir::toNativeSeparators(directoryCSV))+"\n");
		if(ok)
			lastWarnings.insert(0,Export::tr("Exported complete")+"\n");
		else
			lastWarnings.insert(0,Export::tr("Export incomplete")+"\n");
	}

	LastWarningsDialogE lwd(newParent, lastWarnings);
	int w=lwd.sizeHint().width();
	int h=lwd.sizeHint().height();
	lwd.resize(w,h);
	centerWidgetOnScreen(&lwd);
	
	ok=lwd.exec();
}
#else
void Export::exportCSV(Solution* bestOrHighest, Solution* current){
	bool ok=true;

	if (directoryCSV.isNull())
		directoryCSV=OUTPUT_DIR+FILE_SEP+"csv";

	QString s2=getBasename();
	if(s2.isEmpty())
		s2="unnamed";
	directoryCSV.append(FILE_SEP+s2);

	QDir dir;
	if(!dir.exists(directoryCSV))
		dir.mkpath(directoryCSV);

	bool okat, okr, oks, okt, okst, okact, okacts, oktim1, oktim2;

	okat=exportCSVActivityTags();
	okr=exportCSVRoomsAndBuildings();
	oks=exportCSVSubjects();
	okt=exportCSVTeachers();
	okst=exportCSVStudents();
	okact=exportCSVActivities();
	okacts=exportCSVActivitiesStatistic();
	
	if(current==NULL){
		lastWarnings.append(Export::tr("Successfully finished timetable:"));
		lastWarnings.append(" ");
		oktim1=exportCSVTimetable(*bestOrHighest);

		oktim2=true;
	}
	else{
		QString oldName=directoryCSV;
		directoryCSV.append("-highest");
		if(!dir.exists(directoryCSV))
			dir.mkpath(directoryCSV);
		lastWarnings.append(Export::tr("Highest stage timetable:"));
		lastWarnings.append(" ");
		oktim1=exportCSVTimetable(*bestOrHighest);
		
		directoryCSV=oldName;
		directoryCSV.append("-current");
		if(!dir.exists(directoryCSV))
			dir.mkpath(directoryCSV);
		lastWarnings.append(Export::tr("Current stage timetable:"));
		lastWarnings.append(" ");
		oktim2=exportCSVTimetable(*current);

		directoryCSV=oldName;
	}
	
	ok=okat && okr && oks && okt && okst && okact && okacts && oktim1 && oktim2;
		
	lastWarnings.insert(0,Export::tr("CSV files were exported to directory %1.").arg(QDir::toNativeSeparators(directoryCSV))+"\n");
	if(ok)
		lastWarnings.insert(0,Export::tr("Exported complete")+"\n");
	else
		lastWarnings.insert(0,Export::tr("Export incomplete")+"\n");
		
	cout<<qPrintable(tr("FET - export comment", "The comment of the exporting operation"))<<endl;
	cout<<qPrintable(lastWarnings); //no endl here - there is one already
}
#endif

QString Export::getTextQuote() const
{
	return textquote;
}

void Export::setTextQuote(const QString &value)
{
	textquote = value;
}

QString Export::getFieldSeparator() const
{
	return fieldSeparator;
}

void Export::setFieldSeparator(const QString &value)
{
	fieldSeparator = value;
}

bool Export::getHeader() const
{
	return header;
}

void Export::setHeader(bool value)
{
	header = value;
}

QString Export::getSetSeparator() const
{
	return setSeparator;
}

QString Export::getDirectoryCSV() const
{
	return directoryCSV;
}

void Export::setDirectoryCSV(const QString &value)
{
	directoryCSV = value;
}

Export::OverwriteOptions Export::getOverwrite() const
{
	return overwrite;
}

void Export::setOverwrite(const OverwriteOptions& value)
{
	overwrite = value;
}

QString Export::protectCSV(const QString& str){
	QString p=str;
	p.replace("\"", "\"\"");
	return p;
}

bool Export::checkSetSeparator(const QString& str, const QString& setSeparator){
	if(str.contains(setSeparator))
		return false;
	return true;
}

bool Export::isActivityNotManualyEdited(int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents, bool& diffCompNStud, bool& diffNStud, bool& diffActive){ //similar to ActivitiesForm::modifyActivity() by Liviu Lalescu, but added diffActive
	diffTeachers=diffSubject=diffActivityTags=diffStudents=diffCompNStud=diffNStud=diffActive=false;

	assert(activityIndex>=0);
	assert(activityIndex<gt.rules.activitiesList.size());

	Activity* act=gt.rules.activitiesList[activityIndex];
	assert(act!=NULL);
	
	QStringList teachers=act->teachersNames;
	QString subject=act->subjectName;
	QStringList activityTags=act->activityTagsNames;
	QStringList students=act->studentsNames;
	
	int nTotalStudents=act->nTotalStudents;
	
	bool computeNTotalStudents=act->computeNTotalStudents;
	bool active=act->active;

	if(act->isSplit()){
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(teachers!=act2->teachersNames){
					//return false;
					diffTeachers=true;
				}
				if(subject!=act2->subjectName){
					//return false;
					diffSubject=true;
				}
				if(activityTags!=act2->activityTagsNames){
					diffActivityTags=true;
					//return false;
				}
				if(students!=act2->studentsNames){
					diffStudents=true;
					//return false;
				}
				if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ nTotalStudents!=act2->nTotalStudents){
					diffNStud=true;
					//return false;
				}
				if(computeNTotalStudents!=act2->computeNTotalStudents){
					diffCompNStud=true;
					//return false;
				}
				if(active!=act2->active){
					diffActive=true;
					//return false;	
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	if(!diffTeachers && !diffSubject && !diffActivityTags && !diffStudents && !diffCompNStud && !diffNStud && !diffActive)
		return true;
	else
		return false;
}

#ifndef FET_COMMAND_LINE
bool Export::selectSeparatorAndTextquote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head){
	newParent=((QDialog*)parent);

	QStringList separators;
	QStringList textquotes;
	separators<<","<<";"<<"|";
	//textquotes<<"\""<<"'"<<Export::tr("no textquote", "The translated field must contain at least 2 characters (normally it should), otherwise the export filter does not work");
	const QString NO_TEXTQUOTE_TRANSLATED=Export::tr("no textquote", "Please use at least 2 characters for the translation of this field, so that the program works OK");
	textquotes<<"\""<<"'"<<NO_TEXTQUOTE_TRANSLATED;
	const int NO_TEXTQUOTE_POS=2; //if you modify line above, modify also this variable to be the position of the no textquote (starts from 0)
	//also, if you add textquotes longer than one character, take care of line 309 (later in the same function) (assert textquote.size()==1)
	//it is permitted for position NO_TEXTQUOTE_POS to have a string longer than 1 QChar
	
	/*if(textquotes[2].size()<=1){
		QMessageBox::warning(parent, tr("FET warning"), tr("Translation is wrong, because translation of 'no textquote' is too short - falling back to English words. Please report bug"));
		textquote=QString("no textquote");
	}
	assert(textquotes[2].size()>1);*/
	
	const QString settingsName=QString("ExportSelectSeparatorsDialog");

	newParent=new QDialog(parent);
	QDialog& separatorsDialog=(*newParent);
	
	separatorsDialog.setWindowTitle(tr("FET question"));
	QVBoxLayout* separatorsMainLayout=new QVBoxLayout(&separatorsDialog);

	QHBoxLayout* top=new QHBoxLayout();
	QLabel* topText=new QLabel();
	topText->setText(Export::tr("Please keep the default settings.\nImport of data will be easier with these settings."));
	top->addWidget(topText);

	QGroupBox* separatorsGroupBox = new QGroupBox(Export::tr("Please specify the separator between fields:"));
	QComboBox* separatorsCB=NULL;
	if(separators.size()>1){
		QHBoxLayout* separatorBoxChoose=new QHBoxLayout();
		separatorsCB=new QComboBox();

		QLabel* separatorTextChoose=new QLabel();
		separatorTextChoose->setText(Export::tr("Use field separator:"));
		separatorsCB->insertItems(0,separators);
		separatorBoxChoose->addWidget(separatorTextChoose);
		separatorBoxChoose->addWidget(separatorsCB);
		separatorsGroupBox->setLayout(separatorBoxChoose);
	}

	QGroupBox* textquoteGroupBox = new QGroupBox(Export::tr("Please specify the text quote of text fields:"));
	QComboBox* textquoteCB=NULL;
	if(textquotes.size()>1){
		QHBoxLayout* textquoteBoxChoose=new QHBoxLayout();
		textquoteCB=new QComboBox();
		
		QLabel* textquoteTextChoose=new QLabel();
		textquoteTextChoose->setText(Export::tr("Use textquote:"));
		textquoteCB->insertItems(0,textquotes);
		textquoteBoxChoose->addWidget(textquoteTextChoose);
		textquoteBoxChoose->addWidget(textquoteCB);
		textquoteGroupBox->setLayout(textquoteBoxChoose);
	}

	QGroupBox* firstLineGroupBox = new QGroupBox(Export::tr("Please specify the contents of the first line:"));
	QVBoxLayout* firstLineChooseBox=new QVBoxLayout();
	QRadioButton* firstLineRadio1 = new QRadioButton(Export::tr("The first line is the heading."));
	QRadioButton* firstLineRadio2 = new QRadioButton(Export::tr("The first line contains data. Don't export heading."));
	firstLineRadio1->setChecked(true);
	firstLineChooseBox->addWidget(firstLineRadio1);
	firstLineChooseBox->addWidget(firstLineRadio2);
	firstLineGroupBox->setLayout(firstLineChooseBox);

	QPushButton* pb=new QPushButton(tr("OK"));
	QPushButton* cancelpb=new QPushButton(tr("Cancel"));
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb);
	hl->addWidget(cancelpb);
	
	separatorsMainLayout->addLayout(top);
	separatorsMainLayout->addWidget(separatorsGroupBox);
	separatorsMainLayout->addWidget(textquoteGroupBox);

	separatorsMainLayout->addWidget(firstLineGroupBox);
	separatorsMainLayout->addLayout(hl);

	pb->setDefault(true);
	pb->setFocus();
	
	QObject::connect(pb, SIGNAL(clicked()), &separatorsDialog, SLOT(accept()));
	QObject::connect(cancelpb, SIGNAL(clicked()), &separatorsDialog, SLOT(reject()));
		
	int w=separatorsDialog.sizeHint().width();
	int h=separatorsDialog.sizeHint().height();
	separatorsDialog.resize(w,h);
	
	centerWidgetOnScreen(&separatorsDialog);
	restoreFETDialogGeometry(&separatorsDialog, settingsName);

	int ok=separatorsDialog.exec();
	saveFETDialogGeometry(&separatorsDialog, settingsName);
	if(ok!=QDialog::Accepted) return false;

	// TODO: if is always true. maybe clean source (also 2 previous if)
	if(separators.size()>1){
		assert(separatorsCB!=NULL);
		assert(textquoteCB!=NULL);
		fieldSeparator=separatorsCB->currentText();
		textquote=textquoteCB->currentText();
		if(textquoteCB->currentIndex()==NO_TEXTQUOTE_POS){
			assert(textquote==NO_TEXTQUOTE_TRANSLATED);
			textquote=QString("no tquote"); //must have length >= 2
		}
		else{
			assert(textquote.size()==1);
			//assert(textquote=="\"" || textquote=="'");
		}
	}
	else{
		assert(separatorsCB==NULL);
		assert(textquoteCB==NULL);
		fieldSeparator="";
		textquote="";
	}

	if(textquote.size()!=1)
		textquote="";

	if(firstLineRadio1->isChecked())
		head=true;
	else head=false;
	return true;
}
#endif

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivityTags(QWidget* parent){
#else
bool Export::exportCSVActivityTags(){
#endif
	QString file=getFilePath(CSVActivityTags);
	
#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);

	if(header)
		tosExport<<textquote<<"Activity Tag"<<textquote<<endl;

	foreach(ActivityTag* a, gt.rules.activityTagsList){
		tosExport<<textquote<<protectCSV(a->name)<<textquote<<endl;
		if(!checkSetSeparator(a->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.").arg(a->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 activity tags exported.").arg(gt.rules.activityTagsList.size())+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVRoomsAndBuildings(QWidget* parent){
#else
bool Export::exportCSVRoomsAndBuildings(){
#endif
	QString file=getFilePath(CSVRoomsAndBuildings);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport	<<textquote<<"Room"<<textquote<<fieldSeparator
				<<textquote<<"Room Capacity"<<textquote<<fieldSeparator
				<<textquote<<"Building"<<textquote<<endl;

	QStringList checkBuildings;
	foreach(Room* r, gt.rules.roomsList){
		tosExport	<<textquote<<protectCSV(r->name)<<textquote<<fieldSeparator
				<<CustomFETString::number(r->capacity)<<fieldSeparator
				<<textquote<<protectCSV(r->building)<<textquote<<endl;
		if(!checkBuildings.contains(r->building)&&!r->building.isEmpty())
			checkBuildings<<r->building;
	}

	lastWarnings+=Export::tr("%1 rooms (with buildings) exported.").arg(gt.rules.roomsList.size())+"\n";
	if(gt.rules.buildingsList.size()!=checkBuildings.size()){
		lastWarnings+=Export::tr("Warning! Only %1 of %2 building names are exported, because %3 buildings don't contain any room.").arg(checkBuildings.size()).arg(gt.rules.buildingsList.size()).arg(gt.rules.buildingsList.size()-checkBuildings.size())+"\n";
	}
	
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVSubjects(QWidget* parent){
#else
bool Export::exportCSVSubjects(){
#endif
	QString file=getFilePath(CSVSubjects);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport<<textquote<<"Subject"<<textquote<<endl;

	foreach(Subject* s, gt.rules.subjectsList){
		tosExport<<textquote<<protectCSV(s->name)<<textquote<<endl;
	}

	lastWarnings+=Export::tr("%1 subjects exported.").arg(gt.rules.subjectsList.size())+"\n";	
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVTeachers(QWidget* parent){
#else
bool Export::exportCSVTeachers(){
#endif
	QString file=getFilePath(CSVTeachers);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport<<textquote<<"Teacher"<<textquote<<endl;

	foreach(Teacher* t, gt.rules.teachersList){
		tosExport<<textquote<<protectCSV(t->name)<<textquote<<endl;
		if(!checkSetSeparator(t->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.").arg(t->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 teachers exported.").arg(gt.rules.teachersList.size())+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVStudents(QWidget* parent){
#else
bool Export::exportCSVStudents(){
#endif
	QString file=getFilePath(CSVStudents);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport	<<textquote<<"Year"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Year"<<textquote<<fieldSeparator
				<<textquote<<"Group"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Group"<<textquote<<fieldSeparator
				<<textquote<<"Subgroup"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Subgroup"<<textquote<<endl;

	int ig=0;
	int is=0;
	foreach(StudentsYear* sty, gt.rules.yearsList){
		tosExport<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
					<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<endl;
		if(!checkSetSeparator(sty->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.").arg(sty->name)+"\n";
		foreach(StudentsGroup* stg, sty->groupsList){
			ig++;
			tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
					<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator
					<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
					<<CustomFETString::number(stg->numberOfStudents)<<fieldSeparator<<fieldSeparator<<endl;
			if(!checkSetSeparator(stg->name, setSeparator))
				lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.").arg(stg->name)+"\n";
			foreach(StudentsSubgroup* sts, stg->subgroupsList){
				is++;
				tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
						<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator
						<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
						<<CustomFETString::number(stg->numberOfStudents)<<fieldSeparator
						<<textquote<<protectCSV(sts->name)<<textquote<<fieldSeparator
						<<CustomFETString::number(sts->numberOfStudents)<<endl;
				if(!checkSetSeparator(sts->name, setSeparator))
					lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.").arg(sts->name)+"\n";
			}
		}
	}

	lastWarnings+=Export::tr("%1 years exported.").arg(gt.rules.yearsList.size())+"\n";
	lastWarnings+=Export::tr("%1 groups exported.").arg(ig)+"\n";
	lastWarnings+=Export::tr("%1 subgroups exported.").arg(is)+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivities(QWidget* parent){
#else
bool Export::exportCSVActivities(){
#endif
	QString file=getFilePath(CSVActivities);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport	<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tags"<<textquote<<fieldSeparator
				<<textquote<<"Total Duration"<<textquote<<fieldSeparator
				<<textquote<<"Split Duration"<<textquote<<fieldSeparator
				<<textquote<<"Min Days"<<textquote<<fieldSeparator
				<<textquote<<"Weight"<<textquote<<fieldSeparator
				<<textquote<<"Consecutive"<<textquote<<endl;

	//code by Liviu Lalescu (begin)
	//better detection of min days constraint
	QHash<int, int> activitiesRepresentant;
	QHash<int, int> activitiesNumberOfSubactivities;
	QHash<int, ConstraintMinDaysBetweenActivities*>activitiesConstraints;
	
	activitiesRepresentant.clear();
	activitiesNumberOfSubactivities.clear();
	activitiesConstraints.clear();
	
	foreach(Activity* act, gt.rules.activitiesList){
		assert(!activitiesRepresentant.contains(act->id));
		activitiesRepresentant.insert(act->id, act->activityGroupId); //act->id is key, act->agid is value
	
		if(act->activityGroupId>0){
			int n=activitiesNumberOfSubactivities.value(act->activityGroupId, 0); //0 here means default value
			n++;
			activitiesNumberOfSubactivities.insert(act->activityGroupId, n); //overwrites old value
		}
	}
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES && tc->active){
			ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) tc;
	
			QSet<int> aset;
			int repres=-1;
	
			for(int i=0; i<c->n_activities; i++){
				int aid=c->activitiesId[i];
				aset.insert(aid);
	
				if(activitiesRepresentant.value(aid,0)==aid)
					repres=aid; //does not matter if there are more representants in this constraint, the constraint will be skipped anyway in this case
			}
	
			bool oktmp=false;
	
			if(repres>0){
				if(aset.count()==activitiesNumberOfSubactivities.value(repres, 0)){
					oktmp=true;
					foreach(int aid, aset)
						if(activitiesRepresentant.value(aid, 0)!=repres){
							oktmp=false;
							break;
						}
				}
			}
	
			if(!oktmp){
				lastWarnings+=Export::tr("Note: Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
				" it refers not to a whole larger container activity")+"\n";
			}
	
			if(oktmp){
				ConstraintMinDaysBetweenActivities* oldc=activitiesConstraints.value(repres, NULL);
				if(oldc!=NULL){
					if(oldc->weightPercentage < c->weightPercentage){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Note: Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with larger weight percentage, referring to the same activities")+"\n";
					}
					else if(oldc->weightPercentage > c->weightPercentage){
						lastWarnings+=Export::tr("Note: Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with larger weight percentage, referring to the same activities")+"\n";
					}
	
					//equal weights - choose the lowest number of min days
					else if(oldc->minDays > c->minDays){
						lastWarnings+=Export::tr("Note: Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and higher number of min days, referring to the same activities")+"\n";
					}
					else if(oldc->minDays < c->minDays){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Note: Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and higher number of min days, referring to the same activities")+"\n";
					}
	
					//equal weights and min days - choose the one with consecutive is same day true
					else if(oldc->consecutiveIfSameDay==true){
						lastWarnings+=Export::tr("Note: Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and same number of min days and"
							" consecutive if same day true, referring to the same activities")+"\n";
					}
					else if(c->consecutiveIfSameDay==true){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Note: Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and same number of min days and"
							" consecutive if same day true, referring to the same activities")+"\n";
					}
	
				}
				else
					activitiesConstraints.insert(repres, c);
			}
		}
	}
	//code by Liviu Lalescu (end)
	
	bool manuallyEdited=false;

	Activity* acti;
	Activity* actiNext;
	int countExportedActivities=0;
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		acti=gt.rules.activitiesList[ai];
		//if(acti->active){
			if((acti->activityGroupId==acti->id)||(acti->activityGroupId==0)){
				bool diffTeachers, diffSubject, diffActivityTag, diffStudents, diffCompNStud, diffNStud, diffActive;
				if(isActivityNotManualyEdited(ai, diffTeachers, diffSubject, diffActivityTag, diffStudents, diffCompNStud, diffNStud, diffActive)){
				}
				else{
					QStringList s;
					if(diffTeachers)
						s.append(tr("different teachers"));
					if(diffSubject)
						s.append(tr("different subject"));
					if(diffActivityTag)
						s.append(tr("different activity tags"));
					if(diffStudents)
						s.append(tr("different students"));
					if(diffCompNStud)
						s.append(tr("different boolean variable 'must compute n total students'"));
					if(diffNStud)
						s.append(tr("different number of students"));
					if(diffActive)
						s.append(tr("different active flag"));
					
					manuallyEdited=true;
					
					lastWarnings+=tr("Subactivities with activity group id %1 are different between themselves (they were separately edited),"
						" so the export will not be very accurate. The fields which are different will be considered those of the representative subactivity. Fields which were"
						" different are: %2").arg(CustomFETString::number(acti->activityGroupId)).arg(s.join(", "))+"\n";
				}
				if(!acti->active){
					if(acti->activityGroupId==0)
						lastWarnings+=tr("Activity with id %1 has disabled active flag but it is exported.").arg(CustomFETString::number(acti->id))+"\n";
					else
						lastWarnings+=tr("Subactivities with activity group id %1 have disabled active flag but they are exported.").arg(CustomFETString::number(acti->activityGroupId))+"\n";
				}
				
				countExportedActivities++;
				//students set
				tosExport<<textquote;
				for(int s=0; s<acti->studentsNames.size(); s++){
					if(s!=0)
						tosExport<<"+";
					tosExport<<protectCSV(acti->studentsNames[s]);
				}
				tosExport<<textquote<<fieldSeparator<<textquote;
				//subject
				tosExport<<protectCSV(acti->subjectName);
				tosExport<<textquote<<fieldSeparator<<textquote;
				//teachers
				for(int t=0; t<acti->teachersNames.size(); t++){
					if(t!=0)
						tosExport<<"+";
					tosExport<<protectCSV(acti->teachersNames[t]);
				}
				tosExport<<textquote<<fieldSeparator<<textquote;
				//activity tags
				for(int s=0; s<acti->activityTagsNames.size(); s++){
					if(s!=0)
						tosExport<<"+";
					tosExport<<protectCSV(acti->activityTagsNames[s]);
				}
				tosExport<<textquote<<fieldSeparator;
				//total duration
				tosExport<<CustomFETString::number(acti->totalDuration);
				tosExport<<fieldSeparator<<textquote;
				//split duration
				for(int aiNext=ai; aiNext<gt.rules.activitiesList.size(); aiNext++){
					actiNext=gt.rules.activitiesList[aiNext];
					if(acti->activityGroupId!=0&&actiNext->activityGroupId==acti->activityGroupId){
						if(aiNext!=ai)
							tosExport<<"+";
						tosExport<<actiNext->duration;
					} else {
						if(acti->activityGroupId==0&&actiNext->activityGroupId==acti->activityGroupId){
							assert(ai==aiNext);
							assert(actiNext->duration==actiNext->totalDuration);
							if(actiNext->duration>1)
								tosExport<<actiNext->duration;
						}	
						aiNext=gt.rules.activitiesList.size();
					}	
				}
				tosExport<<textquote<<fieldSeparator;
				//min days
				//start new code, because of Liviu's detection
				bool careAboutMinDay=false;
				ConstraintMinDaysBetweenActivities* tcmd=activitiesConstraints.value(acti->id, NULL);
				if(acti->id==acti->activityGroupId){
					if(tcmd!=NULL){
						careAboutMinDay=true;
					}
				}
				//end new code
				if(careAboutMinDay){
					assert(tcmd->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES);
					tosExport<<CustomFETString::number(tcmd->minDays);
				}
				tosExport<<fieldSeparator;
				//min days weight
				if(careAboutMinDay)
					tosExport<<CustomFETString::number(tcmd->weightPercentage);
				tosExport<<fieldSeparator;
				//min days consecutive
				if(careAboutMinDay)
					tosExport<<tcmd->consecutiveIfSameDay;
				tosExport<<endl;
			}
		//}
	}
	
#ifndef FET_COMMAND_LINE
	if(manuallyEdited){
		QMessageBox msgBox(parent);
		msgBox.setWindowTitle(tr("FET warning"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(tr("There are subactivities which were modified separately - so the "
		 "components had different values for subject, activity tags, teachers, students or number of students from the representative subactivity. The export was done, but it is not very accurate."));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		
		msgBox.exec();
	}
#else
	if(manuallyEdited){
		cout<<qPrintable(tr("FET warning"))<<endl;
		cout<<qPrintable(tr("There are subactivities which were modified separately - so the "
		 "components had different values for subject, activity tags, teachers, students or number of students from the representative subactivity. The export was done, but it is not very accurate."))
		 <<endl;
	}
#endif

	lastWarnings+=Export::tr("%1 activities exported.").arg(countExportedActivities)+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivitiesStatistic(QWidget* parent){
#else
bool Export::exportCSVActivitiesStatistic(){
#endif
	QString file=getFilePath(CSVActivitiesStatistic);

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	if(header)
		tosExport	<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Total Duration"<<textquote<<"\n";

	Activity* acti;
	int countExportedActivities=0;
	QMap<QString, int> tmpIdentDuration;	//not QHash, because i want a nice order of the activities
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		acti=gt.rules.activitiesList[ai];
		if(acti->active){
			int tmpD=acti->duration;
			QString tmpIdent=textquote;
			if(acti->studentsNames.size()>0){
				for(QStringList::Iterator it=acti->studentsNames.begin(); it!=acti->studentsNames.end(); it++){
					tmpIdent+=protectCSV(*it);
					if(it!=acti->studentsNames.end()-1)
						tmpIdent+="+";
				}
			}
			tmpIdent+=textquote+fieldSeparator+textquote+protectCSV(acti->subjectName)+textquote+fieldSeparator+textquote;
			if(acti->teachersNames.size()>0){
				for(QStringList::Iterator it=acti->teachersNames.begin(); it!=acti->teachersNames.end(); it++){
					tmpIdent+=protectCSV(*it);
					if(it!=acti->teachersNames.end()-1)
						tmpIdent+="+";
				}
			}
			tmpIdent+=textquote+fieldSeparator;
			tmpD+=tmpIdentDuration.value(tmpIdent);
			tmpIdentDuration.insert(tmpIdent, tmpD);
		}
	}
	QMapIterator<QString, int> it(tmpIdentDuration);
	while(it.hasNext()){
		countExportedActivities++;
		it.next();
		tosExport<<it.key();
		tosExport<<textquote<<CustomFETString::number(it.value())<<textquote<<"\n";
	}

	lastWarnings+=Export::tr("%1 active activities statistics exported.").arg(countExportedActivities)+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVTimetable(QWidget* parent){
#else
bool Export::exportCSVTimetable(const Solution &solution){
#endif
	QString file=getFilePath(CSVTimetable);
	
#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);
	
	//section "Activity Id" was added by Liviu Lalescu on 2010-01-26, as suggested on the forum
	if(header)
		tosExport
				<<textquote<<"Activity Id"<<textquote<<fieldSeparator
				<<textquote<<"Day"<<textquote<<fieldSeparator
				<<textquote<<"Hour"<<textquote<<fieldSeparator
				<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tags"<<textquote<<fieldSeparator
				<<textquote<<"Room"<<textquote<<fieldSeparator
				<<textquote<<"Comments"<<textquote<<endl;

	if(gt.rules.initialized && gt.rules.internalStructureComputed
	 && solution.subgroupsMatrixReady && solution.teachersMatrixReady && solution.roomsMatrixReady){
		const Activity *act;
		int exportedActivities=0;
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			if(solution.time(i)!=UNALLOCATED_TIME) {
				exportedActivities++;
				act=&gt.rules.internalActivitiesList[i];
				int hour=solution.hour(i, gt.rules);
				int day=solution.day(i, gt.rules);
				int r=solution.room(i);
				for(int dd=0; dd < act->duration; dd++){
					assert(hour+dd<gt.rules.nHoursPerDay);
					
					//Activity id - added by Liviu on 2010-01-26
					tosExport<<textquote<<CustomFETString::number(act->id)<<textquote<<fieldSeparator;
					
					//Day
					tosExport<<textquote<<protectCSV(gt.rules.daysOfTheWeek[day])<<textquote<<fieldSeparator;
					//Period
					tosExport<<textquote<<protectCSV(gt.rules.hoursOfTheDay[hour+dd])<<textquote<<fieldSeparator<<textquote;
					//Students Sets
					for(int s=0; s<act->studentsNames.size(); s++){
						if(s!=0)
							tosExport<<"+";
						tosExport<<protectCSV(act->studentsNames[s]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Subject
					tosExport<<protectCSV(act->subjectName);
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Teachers
					for(int t=0; t<act->teachersNames.size(); t++){
						if(t!=0)
							tosExport<<"+";
						tosExport<<protectCSV(act->teachersNames[t]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Activity Tags
					for(int s=0; s<act->activityTagsNames.size(); s++){
						if(s!=0)
							tosExport<<"+";
						tosExport<<protectCSV(act->activityTagsNames[s]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Room
					if(solution.room(i) != UNSPECIFIED_ROOM && solution.room(i) != UNALLOCATED_SPACE){
						assert(solution.room(i)>=0 && solution.room(i)<gt.rules.nInternalRooms);
						tosExport<<protectCSV(gt.rules.internalRoomsList[r]->name);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Comments
					QString tmpString=protectCSV(act->comments);
					tmpString.replace("\n", " ");
					tosExport<<tmpString<<textquote<<endl;
				}
			}
		}	
		lastWarnings+=Export::tr("%1 scheduled activities exported.").arg(exportedActivities)+"\n";
	} else {
		lastWarnings+=Export::tr("0 scheduled activities exported, because no timetable was generated.")+"\n";
	}
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
LastWarningsDialogE::LastWarningsDialogE(QWidget* parent, const QString& lastWarning)
	: QDialog(parent)
{
	this->setWindowTitle(tr("FET - export comment", "The comment of the exporting operation"));
	QVBoxLayout* lastWarningsMainLayout=new QVBoxLayout(this);

	QPlainTextEdit* lastWarningsText=new QPlainTextEdit();
	lastWarningsText->setMinimumWidth(500);				//width
	lastWarningsText->setMinimumHeight(250);
	lastWarningsText->setReadOnly(true);
	lastWarningsText->setWordWrapMode(QTextOption::NoWrap);
	lastWarningsText->setPlainText(lastWarning);

	//Start Buttons
	QPushButton* pb1=new QPushButton(tr("&Ok"));
	//pb1->setAutoDefault(true);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb1);

	//Start adding all into main layout
	lastWarningsMainLayout->addWidget(lastWarningsText);
	lastWarningsMainLayout->addLayout(hl);

	QObject::connect(pb1, SIGNAL(clicked()), this, SLOT(accept()));
	
	//pb1->setDefault(true);

	pb1->setDefault(true);
	pb1->setFocus();
}

LastWarningsDialogE::~LastWarningsDialogE()
{
}
#endif
