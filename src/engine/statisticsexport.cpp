/*
File statisticsexport.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      statisticsexport.cpp  -  description
                             -------------------
    begin                : Sep 2008
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
// Many thanks to Liviu Lalescu. He told me some speed optimizations.

#include "timetable_defs.h"
#include "statisticsexport.h"

// BE CAREFUL: DON'T USE INTERNAL VARIABLES HERE, because maybe computeInternalStructure() is not done!

#include <QString>
#include <QStringList>
#include <QHash>
#include <QMultiHash>
#include <QMap>

#include <QMessageBox>

#include <QLocale>
#include <QTime>
#include <QDate>

#include <QDir>

#include <QFile>
#include <QTextStream>

//#include <QApplication>
#include <QProgressDialog>
//extern QApplication* pqapplication;

extern Timetable gt;

//extern bool simulation_running;	//needed?

//TODO: use the external string!!!
//extern const QString STRING_EMPTY_SLOT;
const QString STRING_EMPTY_SLOT_STATISTICS="---";

const char TEACHERS_STUDENTS_STATISTICS[]="teachers_students.html";
const char TEACHERS_SUBJECTS_STATISTICS[]="teachers_subjects.html";
const char STUDENTS_TEACHERS_STATISTICS[]="students_teachers.html";
const char STUDENTS_SUBJECTS_STATISTICS[]="students_subjects.html";
const char SUBJECTS_TEACHERS_STATISTICS[]="subjects_teachers.html";
const char SUBJECTS_STUDENTS_STATISTICS[]="subjects_students.html";
const char STYLESHEET_STATISTICS[]="stylesheet.css";
const char INDEX_STATISTICS[]="index.html";

QString DIRECTORY_STATISTICS;
QString PREFIX_STATISTICS;

class StringListPair{
public:
	QStringList list1;
	QStringList list2;
};

bool operator <(const StringListPair& pair1, const StringListPair& pair2)
{
	return (pair1.list1.join("")+pair1.list2.join("")) < (pair2.list1.join("")+pair2.list2.join(""));
}

StatisticsExport::StatisticsExport()
{
}

StatisticsExport::~StatisticsExport()
{
}

void StatisticsExport::exportStatistics(QWidget* parent){
	assert(gt.rules.initialized);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	FetStatistics statisticValues;
	computeHashForIDsStatistics(&statisticValues);
	getNamesAndHours(&statisticValues);

	DIRECTORY_STATISTICS=OUTPUT_DIR+FILE_SEP+"statistics";
	
	if(INPUT_FILENAME_XML=="")
		DIRECTORY_STATISTICS.append(FILE_SEP+"unnamed");
	else{
		DIRECTORY_STATISTICS.append(FILE_SEP+INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(DIRECTORY_STATISTICS.right(4)==".fet")
			DIRECTORY_STATISTICS=DIRECTORY_STATISTICS.left(DIRECTORY_STATISTICS.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	
	PREFIX_STATISTICS=DIRECTORY_STATISTICS+FILE_SEP;
	
	int ok=QMessageBox::question(parent, tr("FET Question"),
		 StatisticsExport::tr("Do you want to export detailed statistic files into directory %1 as html files?").arg(QDir::toNativeSeparators(DIRECTORY_STATISTICS)), QMessageBox::Yes | QMessageBox::No);
	if(ok==QMessageBox::No)
		return;

	/* need if i use iTeachersList. Currently unneeded. please remove commented asserts in other functions if this is needed again!
	bool tmp=gt.rules.computeInternalStructure();
	if(!tmp){
		QMessageBox::critical(parent, tr("FET critical"),
		StatisticsExport::tr("Incorrect data")+"\n");
		return;
		assert(0==1);
	}*/

	QDir dir;
	if(!dir.exists(OUTPUT_DIR))
		dir.mkpath(OUTPUT_DIR);
	if(!dir.exists(DIRECTORY_STATISTICS))
		dir.mkpath(DIRECTORY_STATISTICS);

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);

	ok=exportStatisticsStylesheetCss(parent, sTime, statisticValues);
	if(ok)
		ok=exportStatisticsIndex(parent, sTime);
	if(ok)
		ok=exportStatisticsTeachersSubjects(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);
	if(ok)
		ok=exportStatisticsSubjectsTeachers(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);
	if(ok)
		ok=exportStatisticsTeachersStudents(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);
	if(ok)
		ok=exportStatisticsStudentsTeachers(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);
	if(ok)
		ok=exportStatisticsSubjectsStudents(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);
	if(ok)
		ok=exportStatisticsStudentsSubjects(parent, sTime, statisticValues, TIMETABLE_HTML_LEVEL);

	if(ok){
		QMessageBox::information(parent, tr("FET Information"),
		 StatisticsExport::tr("Statistic files were exported to directory %1 as html files.").arg(QDir::toNativeSeparators(DIRECTORY_STATISTICS)));
	} else {
		QMessageBox::warning(parent, tr("FET warning"),
		 StatisticsExport::tr("Statistics export incomplete")+"\n");
	}
}

void StatisticsExport::computeHashForIDsStatistics(FetStatistics *statisticValues){		// by Volker Dirr
	//TODO if we use a relational data base this is unneded, because we can use the primary key id of the database 
	//This is very similar to timetable compute hash. so always check it if you change something here!

	assert((*statisticValues).hashStudentIDsStatistics.isEmpty());
	assert((*statisticValues).hashSubjectIDsStatistics.isEmpty());
	assert((*statisticValues).hashActivityTagIDsStatistics.isEmpty());
	assert((*statisticValues).hashTeacherIDsStatistics.isEmpty());
	//assert((*statisticValues).hashRoomIDsStatistics.isEmpty());
	//assert((*statisticValues).hashDayIDsStatistics.isEmpty());
	
	int cnt=1;
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(!(*statisticValues).hashStudentIDsStatistics.contains(sty->name)){
			(*statisticValues).hashStudentIDsStatistics.insert(sty->name, CustomFETString::number(cnt));
			cnt++;
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(!(*statisticValues).hashStudentIDsStatistics.contains(stg->name)){
				(*statisticValues).hashStudentIDsStatistics.insert(stg->name, CustomFETString::number(cnt));
				cnt++;
			}
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(!(*statisticValues).hashStudentIDsStatistics.contains(sts->name)){
					(*statisticValues).hashStudentIDsStatistics.insert(sts->name, CustomFETString::number(cnt));
					cnt++;
				}
			}
		}
	}
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		(*statisticValues).hashSubjectIDsStatistics.insert(gt.rules.subjectsList[i]->name, CustomFETString::number(i+1));
	}
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		(*statisticValues).hashActivityTagIDsStatistics.insert(gt.rules.activityTagsList[i]->name, CustomFETString::number(i+1));
	}
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		(*statisticValues).hashTeacherIDsStatistics.insert(gt.rules.teachersList[i]->name, CustomFETString::number(i+1));
	}
	/*for(int room=0; room<gt.rules.roomsList.size(); room++){
		(*statisticValues).hashRoomIDsStatistics.insert(gt.rules.roomsList[room]->name, CustomFETString::number(room+1));
	}*/
	/*for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		(*statisticValues).hashDayIDsStatistics.insert(gt.rules.daysOfTheWeek[k], CustomFETString::number(k+1));
	}*/
}

void StatisticsExport::getNamesAndHours(FetStatistics *statisticValues){
	assert((*statisticValues).allStudentsNames.isEmpty());
	assert((*statisticValues).allSubjectsNames.isEmpty());
	assert((*statisticValues).allTeachersNames.isEmpty());
	
	assert((*statisticValues).studentsTotalNumberOfHours.isEmpty());
	assert((*statisticValues).studentsTotalNumberOfHours2.isEmpty());
	
	assert((*statisticValues).subjectsTotalNumberOfHours.isEmpty());
	assert((*statisticValues).subjectsTotalNumberOfHours4.isEmpty());
	
	assert((*statisticValues).teachersTotalNumberOfHours.isEmpty());
	assert((*statisticValues).teachersTotalNumberOfHours2.isEmpty());
	
	assert((*statisticValues).studentsActivities.isEmpty());
	assert((*statisticValues).subjectsActivities.isEmpty());
	assert((*statisticValues).teachersActivities.isEmpty());
	
	QSet<QString> allStudentsNamesSet;
	foreach(StudentsYear* sty, gt.rules.yearsList){
		if(!allStudentsNamesSet.contains(sty->name)){
			(*statisticValues).allStudentsNames<<sty->name;
			allStudentsNamesSet.insert(sty->name);
		}
		foreach(StudentsGroup* stg, sty->groupsList){
			if(!allStudentsNamesSet.contains(stg->name)){
				(*statisticValues).allStudentsNames<<stg->name;
				allStudentsNamesSet.insert(stg->name);
			}
			foreach(StudentsSubgroup* sts, stg->subgroupsList){
				if(!allStudentsNamesSet.contains(sts->name)){
					(*statisticValues).allStudentsNames<<sts->name;
					allStudentsNamesSet.insert(sts->name);
				}
			}
		}
	}

	foreach(Teacher* t, gt.rules.teachersList){
		(*statisticValues).allTeachersNames<<t->name;
	}

	
	foreach(Subject* s, gt.rules.subjectsList){
		(*statisticValues).allSubjectsNames<<s->name;
	}

	//QProgressDialog progress(parent);
	//progress.setLabelText(tr("Processing activities...please wait"));
	//progress.setRange(0,gt.rules.activitiesList.count());
	//progress.setModal(true);
	
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		//pqapplication->processEvents();
		
		Activity* act=gt.rules.activitiesList[ai];
		if(act->active){
				(*statisticValues).subjectsActivities.insert(act->subjectName, ai);
				int tmp=(*statisticValues).subjectsTotalNumberOfHours.value(act->subjectName)+act->duration;
				(*statisticValues).subjectsTotalNumberOfHours.insert(act->subjectName, tmp);						// (1) so teamlearning-teaching is not counted twice!
				foreach(QString t, act->teachersNames){
					(*statisticValues).teachersActivities.insert(t, ai);
					tmp=(*statisticValues).teachersTotalNumberOfHours.value(t)+act->duration;
					(*statisticValues).teachersTotalNumberOfHours.insert(t, tmp);							// (3)
					//subjectstTotalNumberOfHours2[act->subjectIndex]+=duration;				// (1) so teamteaching is counted twice!
				}
				foreach(QString st, act->studentsNames){
					(*statisticValues).studentsActivities.insert(st, ai);
					tmp=(*statisticValues).studentsTotalNumberOfHours.value(st)+act->duration;
					(*statisticValues).studentsTotalNumberOfHours.insert(st, tmp);							// (2)
					//subjectstTotalNumberOfHours3[act->subjectIndex]+=duration;				// (1) so teamlearning is counted twice!
				}
				foreach(QString t, act->teachersNames){
					tmp=(*statisticValues).teachersTotalNumberOfHours2.value(t);
					tmp += act->duration * act->studentsNames.count();
					(*statisticValues).teachersTotalNumberOfHours2.insert(t, tmp);						// (3)
				}
				foreach(QString st, act->studentsNames){
					tmp=(*statisticValues).studentsTotalNumberOfHours2.value(st);
					tmp += act->duration * act->teachersNames.count();
					(*statisticValues).studentsTotalNumberOfHours2.insert(st, tmp);					// (2)
				}
				tmp=(*statisticValues).subjectsTotalNumberOfHours4.value(act->subjectName);
				tmp += act->duration * act->studentsNames.count() * act->teachersNames.count();
				(*statisticValues).subjectsTotalNumberOfHours4.insert(act->subjectName, tmp);			// (1) so teamlearning-teaching is counted twice!
			}
	}
	//progress.setValue(gt.rules.activitiesList.count());
	QStringList tmp;
	tmp.clear();
	foreach(QString students, (*statisticValues).allStudentsNames){
		if((*statisticValues).studentsTotalNumberOfHours.value(students)==0 && (*statisticValues).studentsTotalNumberOfHours2.value(students)==0){
			(*statisticValues).studentsTotalNumberOfHours.remove(students);
			(*statisticValues).studentsTotalNumberOfHours2.remove(students);
		} else
			tmp<<students;
	}
	(*statisticValues).allStudentsNames=tmp;
	tmp.clear();
	foreach(QString teachers, (*statisticValues).allTeachersNames){
		if((*statisticValues).teachersTotalNumberOfHours.value(teachers)==0 && (*statisticValues).teachersTotalNumberOfHours2.value(teachers)==0){
				(*statisticValues).teachersTotalNumberOfHours.remove(teachers);
				(*statisticValues).teachersTotalNumberOfHours2.remove(teachers);
		} else
			tmp<<teachers;
	}
	(*statisticValues).allTeachersNames=tmp;
	tmp.clear();
	foreach(QString subjects, (*statisticValues).allSubjectsNames){
		if((*statisticValues).subjectsTotalNumberOfHours.value(subjects)==0 && (*statisticValues).subjectsTotalNumberOfHours4.value(subjects)==0){
			(*statisticValues).subjectsTotalNumberOfHours.remove(subjects);
			(*statisticValues).subjectsTotalNumberOfHours4.remove(subjects);
		} else
			tmp<<subjects;
	}
	(*statisticValues).allSubjectsNames=tmp;
	tmp.clear();
}

bool StatisticsExport::exportStatisticsStylesheetCss(QWidget* parent, QString saveTime, FetStatistics statisticValues){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	
	QString htmlfilename=PREFIX_STATISTICS+s2+bar+STYLESHEET_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	//get used students	//similar to timetableexport.cpp, so maybe use a function?
	QSet<QString> usedStudents;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		foreach(QString st, gt.rules.activitiesList[i]->studentsNames){
			if(gt.rules.activitiesList[i]->active){
				if(!usedStudents.contains(st))
					usedStudents<<st;
			}
		}
	}

	tos<<"@charset \"UTF-8\";"<<"\n\n";

	QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(INPUT_FILENAME_XML=="")
		tt=tr("unnamed");
	tos<<"/* "<<StatisticsExport::tr("CSS Stylesheet of %1", "%1 is the file name").arg(tt);
	tos<<"\n";
	tos<<"   "<<StatisticsExport::tr("Stylesheet generated with FET %1 on %2", "%1 is FET version, %2 is date and time").arg(FET_VERSION).arg(saveTime)<<" */\n\n";

	tos<<"/* "<<StatisticsExport::tr("To hide an element just write the following phrase into the element: %1 (without quotes).",
		"%1 is a short phrase beginning and ending with quotes, and we want the user to be able to add it, but without quotes").arg("\"display:none;\"")<<" */\n\n";
	tos<<"table {\n  text-align: center;\n}\n\n";
	tos<<"caption {\n\n}\n\n";
	tos<<"thead {\n\n}\n\n";

	//workaround begin.
	tos<<"/* "<<StatisticsExport::tr("Some programs import \"tfoot\" incorrectly. So we use \"tr.foot\" instead of \"tfoot\".",
	 "Please keep tfoot and tr.foot untranslated, as they are in the original English phrase")<<" */\n\n";
	//tos<<"tfoot {\n\n}\n\n";
	tos<<"tr.foot {\n\n}\n\n";
	//workaround end
	
	tos<<"tbody {\n\n}\n\n";
	tos<<"th {\n\n}\n\n";
	tos<<"td {\n\n}\n\n";
	tos<<"td.detailed {\n  border: 1px dashed silver;\n  border-bottom: 0;\n  border-top: 0;\n}\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"th.xAxis {\n/*width: 8em; */\n}\n\n";
		tos<<"th.yAxis {\n  height: 8ex;\n}\n\n";
	}
	if(TIMETABLE_HTML_LEVEL>=4){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
		for(int i=0; i<gt.rules.subjectsList.size(); i++){
			tos << "span.s_"<<statisticValues.hashSubjectIDsStatistics.value(gt.rules.subjectsList[i]->name)<<" { /* subject "<<gt.rules.subjectsList[i]->name<<" */\n\n}\n\n";
		}
		if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
			for(int i=0; i<gt.rules.activityTagsList.size(); i++){
				tos << "span.at_"<<statisticValues.hashActivityTagIDsStatistics.value(gt.rules.activityTagsList[i]->name)<<" { /* activity tag "<<gt.rules.activityTagsList[i]->name<<" */\n\n}\n\n";
			}
		}
		for(int i=0; i<gt.rules.yearsList.size(); i++){
			StudentsYear* sty=gt.rules.yearsList[i];
			if(usedStudents.contains(sty->name))
				tos << "span.ss_"<<statisticValues.hashStudentIDsStatistics.value(sty->name)<<" { /* students set "<<sty->name<<" */\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				if(usedStudents.contains(stg->name))
					tos << "span.ss_"<<statisticValues.hashStudentIDsStatistics.value(stg->name)<<" { /* students set "<<stg->name<<" */\n\n}\n\n";
				for(int k=0; k<stg->subgroupsList.size(); k++){
					StudentsSubgroup* sts=stg->subgroupsList[k];
					if(usedStudents.contains(sts->name))
						tos << "span.ss_"<<statisticValues.hashStudentIDsStatistics.value(sts->name)<<" { /* students set "<<sts->name<<" */\n\n}\n\n";
				}
			}
		}
		for(int i=0; i<gt.rules.teachersList.size(); i++){
			tos << "span.t_"<<statisticValues.hashTeacherIDsStatistics.value(gt.rules.teachersList[i]->name)<<" { /* teacher "<<gt.rules.teachersList[i]->name<<" */\n\n}\n\n";
		}
		//for(int room=0; room<gt.rules.roomsList.size(); room++){
		//	tos << "span.r_"<<statisticValues.hashRoomIDsStatistics.value(gt.rules.roomsList[room]->name)<<" { /* room "<<gt.rules.roomsList[room]->name<<" */\n\n}\n\n";
		//}
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"span.subject {\n\n}\n\n";
		if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
			tos<<"span.activitytag {\n\n}\n\n";
		}
		tos<<"span.empty {\n  color: gray;\n}\n\n";
		tos<<"td.empty {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";
		//tos<<"span.notAvailable {\n  color: gray;\n}\n\n";
		//tos<<"td.notAvailable {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";
		tos<<"tr.studentsset {\n\n}\n\n";
		tos<<"tr.teacher {\n\n}\n\n";
		//tos<<"td.room, div.room {\n\n}\n\n";
		tos<<"tr.duration {\n\n}\n\n";
		//tos<<"tr.line0 {\n  font-size: smaller;\n}\n\n";
		tos<<"tr.line1 {\n\n}\n\n";
		tos<<"tr.line2 {\n  font-size: smaller;\n  color: gray;\n}\n\n";
		//tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n  color: silver;\n}\n\n";
	}
	
	tos<<"/* "<<StatisticsExport::tr("End of file.")<<" */\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

bool StatisticsExport::exportStatisticsIndex(QWidget* parent, QString saveTime){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!
	
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	
	QString htmlfilename=PREFIX_STATISTICS+s2+bar+INDEX_STATISTICS;
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";

	if(TIMETABLE_HTML_LEVEL>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";

		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0; spans.length>i; i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	tos<<"    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+tr("Institution name")+":</th>\n        <td>"+protect2(gt.rules.institutionName)+"</td>\n      </tr>\n    </table>\n";
	tos<<"    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+tr("Comments")+":</th>\n        <td>"+protect2(gt.rules.comments).replace(QString("\n"), QString("<br />\n"))+"</td>\n      </tr>\n    </table>\n";
	tos<<"    <p>\n";
	tos<<"    </p>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";
	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\"3\">"+tr("Statistics")+"</th></tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	tos<<"          <th>"+tr("Teachers")+"</th><th>"+tr("Students")+"</th><th>"+tr("Subjects")+"</th>\n";
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	tos<<"      <tbody>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Teachers")+"</th>\n";
	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT_STATISTICS)<<"</td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_STUDENTS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_SUBJECTS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Students")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+STUDENTS_TEACHERS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT_STATISTICS)<<"</td>\n";
	tos<<"          <td><a href=\""<<s2+bar+STUDENTS_SUBJECTS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Subjects")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TEACHERS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_STUDENTS_STATISTICS<<"\">"+tr("view")+"</a></td>\n";
	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT_STATISTICS)<<"</td>\n";
	tos<<"        </tr>\n";
	//workaround begin.
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\"3\">"<<StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

bool StatisticsExport::exportStatisticsTeachersSubjects(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+TEACHERS_SUBJECTS_STATISTICS;
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<exportStatisticsTeachersSubjectsHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allTeachersNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsTeachersSubjectsHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && colspan<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Teachers - Subjects Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allTeachersNames.at(teacher))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing teachers with subjects...please wait"));
	progress.setRange(0, statisticValues.allSubjectsNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString subjects, statisticValues.allSubjectsNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allSubjectsNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/ tmp;
		}
		ttt++;
	
		QList<int> tmpSubjects;
		QMultiHash<QString, int> tmpTeachers;
		tmpSubjects.clear();
		tmpTeachers.clear();
		tmpSubjects=statisticValues.subjectsActivities.values(subjects);
		foreach(int aidx, tmpSubjects){
			Activity* act=gt.rules.activitiesList.at(aidx);
			foreach(QString teacher, act->teachersNames){
				tmpTeachers.insert(teacher, aidx);
			}
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(subjects)+"</th>\n";
		currentCount=0;
		for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
			if(!(*excludedNames).contains(teacher)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpTeachers.values(statisticValues.allTeachersNames.at(teacher));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 1
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=act->studentsNames;
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}
					
					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"studentsset line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						
						StringListPair slp=it2.key();
						QStringList studentsNames=slp.list1;
						QStringList activityTagsNames=slp.list2;
						QString tmpSt=QString("");
						if(studentsNames.size()>0||activityTagsNames.size()>0){
							for(QStringList::Iterator st=studentsNames.begin(); st!=studentsNames.end(); st++){
								switch(htmlLevel){
									case 4 : tmpSt+="<span class=\"ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"\">"+protect2(*st)+"</span>"; break;
									case 5 : ;
									case 6 : tmpSt+="<span class=\"ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"\" onmouseover=\"highlight('ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"')\">"+protect2(*st)+"</span>"; break;
									default: tmpSt+=protect2(*st); break;
									}
								if(st!=studentsNames.end()-1)
									tmpSt+=", ";
							}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpSt+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpSt+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpSt+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpSt+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpSt+=", ";
								}
							}
							
						} else
							tmpSt=" ";

						tmp+=tmpSt;
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(subjects));
		if(statisticValues.subjectsTotalNumberOfHours.value(subjects)!=statisticValues.subjectsTotalNumberOfHours4.value(subjects))
			tmp+="<br />("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(subjects))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allSubjectsNames.count());
	
	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher)));
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))
				tmp+="<br />("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))+")";
			tmp+="</th>\n";
			*excludedNames<<teacher;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";
	return tmp;
}

bool StatisticsExport::exportStatisticsSubjectsTeachers(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+SUBJECTS_TEACHERS_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<StatisticsExport::exportStatisticsSubjectsTeachersHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allSubjectsNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsSubjectsTeachersHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && colspan<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Subjects - Teachers Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allSubjectsNames.at(subject))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing subjects with teachers...please wait"));
	progress.setRange(0, statisticValues.allTeachersNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString teachers, statisticValues.allTeachersNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allTeachersNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/tmp;
		}
		ttt++;
	
		QList<int> tmpTeachers;
		QMultiHash<QString, int> tmpSubjects;
		tmpTeachers.clear();
		tmpSubjects.clear();
		tmpTeachers=statisticValues.teachersActivities.values(teachers);
		foreach(int aidx, tmpTeachers){
			Activity* act=gt.rules.activitiesList.at(aidx);
			tmpSubjects.insert(act->subjectName, aidx);
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(teachers)+"</th>\n";
		currentCount=0;
		for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
			if(!(*excludedNames).contains(subject)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpSubjects.values(statisticValues.allSubjectsNames.at(subject));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 2
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=act->studentsNames;
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}

					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"studentsset line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						
						StringListPair slp=it2.key();
						QStringList studentsNames=slp.list1;
						QStringList activityTagsNames=slp.list2;
						QString tmpSt=QString("");
						if(studentsNames.size()>0||activityTagsNames.size()>0){
							for(QStringList::Iterator st=studentsNames.begin(); st!=studentsNames.end(); st++){
								switch(htmlLevel){
									case 4 : tmpSt+="<span class=\"ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"\">"+protect2(*st)+"</span>"; break;
									case 5 : ;
									case 6 : tmpSt+="<span class=\"ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"\" onmouseover=\"highlight('ss_"+statisticValues.hashStudentIDsStatistics.value(*st)+"')\">"+protect2(*st)+"</span>"; break;
									default: tmpSt+=protect2(*st); break;
									}
								if(st!=studentsNames.end()-1)
									tmpSt+=", ";
							}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpSt+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpSt+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpSt+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpSt+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpSt+=", ";
								}
							}
						} else
							tmpSt=" ";
						tmp+=tmpSt;
						
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(teachers));
		if(statisticValues.teachersTotalNumberOfHours.value(teachers)!=statisticValues.teachersTotalNumberOfHours2.value(teachers))
			tmp+="<br />("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(teachers))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allTeachersNames.count());
	
	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject)));
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))!=statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject)))
				tmp+="<br />("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject)))+")";
			tmp+="</th>\n";
			*excludedNames<<subject;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";
	return tmp;
}

bool StatisticsExport::exportStatisticsTeachersStudents(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+TEACHERS_STUDENTS_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<StatisticsExport::exportStatisticsTeachersStudentsHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allTeachersNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsTeachersStudentsHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && colspan<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Teachers - Students Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allTeachersNames.at(teacher))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing teachers with students...please wait"));
	progress.setRange(0, statisticValues.allStudentsNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString students, statisticValues.allStudentsNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allStudentsNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/tmp;
		}
		ttt++;
	
		QList<int> tmpStudents;
		QMultiHash<QString, int> tmpTeachers;
		tmpStudents.clear();
		tmpTeachers.clear();
		tmpStudents=statisticValues.studentsActivities.values(students);
		foreach(int aidx, tmpStudents){
			Activity* act=gt.rules.activitiesList.at(aidx);
			foreach(QString teacher, act->teachersNames){
				tmpTeachers.insert(teacher, aidx);
			}
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(students)+"</th>\n";
		currentCount=0;
		for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
			if(!(*excludedNames).contains(teacher)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpTeachers.values(statisticValues.allTeachersNames.at(teacher));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 3
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=QStringList(act->subjectName);
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}

					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"subject line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";

						StringListPair slp=it2.key();
						assert(slp.list1.count()==1);
						QString subjectName=slp.list1.at(0);
						QStringList activityTagsNames=slp.list2;
						QString tmpS=QString("");
						if(!subjectName.isEmpty()||activityTagsNames.size()>0){
							if(!subjectName.isEmpty())
								switch(htmlLevel){
									case 3 : tmpS+="<span class=\"subject\">"+protect2(subjectName)+"</span>"; break;
									case 4 : tmpS+="<span class=\"subject\"><span class=\"s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"\">"+protect2(subjectName)+"</span></span>"; break;
									case 5 : ;
									case 6 : tmpS+="<span class=\"subject\"><span class=\"s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"\" onmouseover=\"highlight('s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"')\">"+protect2(subjectName)+"</span></span>"; break;
									default: tmpS+=protect2(subjectName); break;
								}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpS+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpS+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpS+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpS+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpS+=", ";
								}
							}
						} else
							tmpS=" ";

						tmp+=tmpS;
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(students));
		if(statisticValues.studentsTotalNumberOfHours.value(students)!=statisticValues.studentsTotalNumberOfHours2.value(students))
			tmp+="<br />("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(students))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allStudentsNames.count());
	
	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count() && currentCount<maxNames; teacher++){
		if(!(*excludedNames).contains(teacher)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher)));
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))
				tmp+="<br />("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))+")";
			tmp+="</th>\n";
			*excludedNames<<teacher;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";	
	return tmp;
}

bool StatisticsExport::exportStatisticsStudentsTeachers(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+STUDENTS_TEACHERS_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<StatisticsExport::exportStatisticsStudentsTeachersHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allStudentsNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsStudentsTeachersHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && colspan<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Students -Teachers Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allStudentsNames.at(students))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing students with teachers...please wait"));
	progress.setRange(0, statisticValues.allTeachersNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString teachers, statisticValues.allTeachersNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allTeachersNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/tmp;
		}
		ttt++;
	
		QList<int> tmpTeachers;
		QMultiHash<QString, int> tmpStudents;
		tmpTeachers.clear();
		tmpStudents.clear();
		tmpTeachers=statisticValues.teachersActivities.values(teachers);
		foreach(int aidx, tmpTeachers){
			Activity* act=gt.rules.activitiesList.at(aidx);
			foreach(QString students, act->studentsNames){
				tmpStudents.insert(students, aidx);
			}
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(teachers)+"</th>\n";
		currentCount=0;
		for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
			if(!(*excludedNames).contains(students)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpStudents.values(statisticValues.allStudentsNames.at(students));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 4
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=QStringList(act->subjectName);
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}

					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"subject line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
							
						StringListPair slp=it2.key();
						assert(slp.list1.count()==1);
						QString subjectName=slp.list1.at(0);
						QStringList activityTagsNames=slp.list2;
						QString tmpS=QString("");
						if(!subjectName.isEmpty()||activityTagsNames.size()>0){
							if(!subjectName.isEmpty())
								switch(htmlLevel){
									case 3 : tmpS+="<span class=\"subject\">"+protect2(subjectName)+"</span>"; break;
									case 4 : tmpS+="<span class=\"subject\"><span class=\"s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"\">"+protect2(subjectName)+"</span></span>"; break;
									case 5 : ;
									case 6 : tmpS+="<span class=\"subject\"><span class=\"s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"\" onmouseover=\"highlight('s_"+statisticValues.hashSubjectIDsStatistics.value(subjectName)+"')\">"+protect2(subjectName)+"</span></span>"; break;
									default: tmpS+=protect2(subjectName); break;
								}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpS+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpS+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpS+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpS+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpS+=", ";
								}
							}
						} else
							tmpS=" ";
						tmp+=tmpS;
						
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(teachers));
		if(statisticValues.teachersTotalNumberOfHours.value(teachers)!=statisticValues.teachersTotalNumberOfHours2.value(teachers))
			tmp+="<br />("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(teachers))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allTeachersNames.count());
	
	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students)));
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))!=statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))
				tmp+="<br />("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))+")";
			tmp+="</th>\n";
			*excludedNames<<students;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";
	return tmp;
}

bool StatisticsExport::exportStatisticsSubjectsStudents(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+SUBJECTS_STUDENTS_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<StatisticsExport::exportStatisticsSubjectsStudentsHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allSubjectsNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsSubjectsStudentsHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && colspan<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Subjects - Students Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allSubjectsNames.at(subject))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing subjects with students...please wait"));
	progress.setRange(0, statisticValues.allStudentsNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString students, statisticValues.allStudentsNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allStudentsNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/tmp;
		}
		ttt++;
	
		QList<int> tmpStudents;
		QMultiHash<QString, int> tmpSubjects;
		tmpStudents.clear();
		tmpSubjects.clear();
		tmpStudents=statisticValues.studentsActivities.values(students);
		foreach(int aidx, tmpStudents){
			Activity* act=gt.rules.activitiesList.at(aidx);
			tmpSubjects.insert(act->subjectName, aidx);
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(students)+"</th>\n";
		currentCount=0;
		for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
			if(!(*excludedNames).contains(subject)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpSubjects.values(statisticValues.allSubjectsNames.at(subject));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 5
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=act->teachersNames;
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}

					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"teacher line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";

						StringListPair slp=it2.key();
						QStringList teachersNames=slp.list1;
						QStringList activityTagsNames=slp.list2;
						QString tmpT=QString("");

						if(teachersNames.size()>0||activityTagsNames.size()>0){
							for(QStringList::Iterator it=teachersNames.begin(); it!=teachersNames.end(); it++){
								switch(htmlLevel){
									case 4 : tmpT+="<span class=\"t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"\">"+protect2(*it)+"</span>"; break;
									case 5 : ;
									case 6 : tmpT+="<span class=\"t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"\" onmouseover=\"highlight('t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"')\">"+protect2(*it)+"</span>"; break;
									default: tmpT+=protect2(*it); break;
								}
								if(it!=teachersNames.end()-1)
									tmpT+=", ";
							}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpT+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpT+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpT+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpT+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpT+=", ";
								}
							}
						} else
							tmpT=" ";
						tmp+=tmpT;
						
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(students));
		if(statisticValues.studentsTotalNumberOfHours.value(students)!=statisticValues.studentsTotalNumberOfHours2.value(students))
			tmp+="<br />("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(students))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allStudentsNames.count());

	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int subject=0; subject<statisticValues.allSubjectsNames.count() && currentCount<maxNames; subject++){
		if(!(*excludedNames).contains(subject)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject)));
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))!=statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject)))
				tmp+="<br />("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject)))+")";
			tmp+="</th>\n";
			*excludedNames<<subject;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";
	return tmp;
}

bool StatisticsExport::exportStatisticsStudentsSubjects(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel){
	assert(gt.rules.initialized); // && gt.rules.internalStructureComputed);
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because to long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";

	QString htmlfilename=PREFIX_STATISTICS+s2+bar+STUDENTS_SUBJECTS_STATISTICS;

	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return false;
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(htmlLevel>=1){
		QString bar;
		if(INPUT_FILENAME_XML=="")
			bar="";
		else
			bar="_";
	
		QString cssfilename=s2+bar+STYLESHEET_STATISTICS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(htmlLevel>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";
	QSet<int> tmpSet;
	tos<<StatisticsExport::exportStatisticsStudentsSubjectsHtml(parent, saveTime, statisticValues, htmlLevel, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, statisticValues.allStudentsNames.count(), &tmpSet);
	tos<<"  </body>\n</html>\n";

	if(file.error()>0){
		QMessageBox::critical(parent, tr("FET critical"),
		 StatisticsExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
		return false;
	}
	file.close();
	return true;
}

QString StatisticsExport::exportStatisticsStudentsSubjectsHtml(QWidget* parent, QString saveTime, FetStatistics statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int> *excludedNames){
	int colspan=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && colspan<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			colspan++;
		}
	}
	QString tmp;
	tmp+="    <table border=\"1\">\n";	
	tmp+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmp+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(colspan+1)+"\">"+tr("Students -Subjects Matrix")+"</th></tr>\n";
	tmp+="        <tr>\n          <!-- span -->\n";
	int currentCount=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			currentCount++;
			if(htmlLevel>=2)
				tmp+="          <th class=\"xAxis\">";
			else
				tmp+="          <th>";
			tmp+=protect2(statisticValues.allStudentsNames.at(students))+"</th>\n";
		}
	}
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	tmp+="        </tr>\n";
	tmp+="      </thead>\n";
	tmp+="      <tbody>\n";
	
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Exporting statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing students with subjects...please wait"));
	progress.setRange(0, statisticValues.allSubjectsNames.count());
	progress.setModal(true);
	
	int ttt=0;
	
	foreach(QString subjects, statisticValues.allSubjectsNames){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(statisticValues.allSubjectsNames.count());
			QMessageBox::warning(parent, tr("FET warning"), tr("Canceled"));
			return /*false*/tmp;
		}
		ttt++;
	
		QList<int> tmpSubjects;
		QMultiHash<QString, int> tmpStudents;
		tmpSubjects.clear();
		tmpStudents.clear();
		tmpSubjects=statisticValues.subjectsActivities.values(subjects);
		foreach(int aidx, tmpSubjects){
			Activity* act=gt.rules.activitiesList.at(aidx);
			foreach(QString students, act->studentsNames){
				tmpStudents.insert(students, aidx);
			}
		}
		tmp+="        <tr>\n";
		if(htmlLevel>=2)
			tmp+="          <th class=\"yAxis\">";
		else
			tmp+="          <th>";
		tmp+=protect2(subjects)+"</th>\n";
		currentCount=0;
		for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
			if(!(*excludedNames).contains(students)){
				currentCount++;
				QList<int> tmpActivities;
				tmpActivities.clear();
				tmpActivities=tmpStudents.values(statisticValues.allStudentsNames.at(students));
				if(tmpActivities.isEmpty()){
					switch(htmlLevel){
						case 3 : ;
						case 4 : tmp+="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						case 5 : ;
						case 6 : tmp+="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</span></td>\n"; break;
						default: tmp+="          <td>"+protect2(STRING_EMPTY_SLOT_STATISTICS)+"</td>\n";
					}
				} else {
					//optimized by Liviu Lalescu - 6
					QMap<StringListPair, int> durationMap;
					foreach(int tmpAct, tmpActivities){
						Activity* act=gt.rules.activitiesList.at(tmpAct);
						StringListPair slp;
						slp.list1=act->teachersNames;
						slp.list2=act->activityTagsNames;
						int dur=durationMap.value(slp, 0);
						dur+=act->duration;
						durationMap.insert(slp, dur);
					}

					if(htmlLevel>=1)
						tmp+="          <td><table class=\"detailed\">";
					else
						tmp+="          <td><table>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"duration line1\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it(durationMap);
					while(it.hasNext()){
						it.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";
						tmp+=QString::number(it.value())+"</td>";
					}
					tmp+="</tr>";
					if(htmlLevel>=3)
						tmp+="<tr class=\"teacher line2\">";
					else	tmp+="<tr>";
					QMapIterator<StringListPair, int> it2(durationMap);	//do it with the same iterator
					while(it2.hasNext()){
						it2.next();
						if(htmlLevel>=1)
							tmp+="<td class=\"detailed\">";
						else
							tmp+="<td>";

						StringListPair slp=it2.key();
						QStringList teachersNames=slp.list1;
						QStringList activityTagsNames=slp.list2;
						QString tmpT=QString("");

						if(teachersNames.size()>0||activityTagsNames.size()>0){
							for(QStringList::Iterator it=teachersNames.begin(); it!=teachersNames.end(); it++){
								switch(htmlLevel){
									case 4 : tmpT+="<span class=\"t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"\">"+protect2(*it)+"</span>"; break;
									case 5 : ;
									case 6 : tmpT+="<span class=\"t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"\" onmouseover=\"highlight('t_"+statisticValues.hashTeacherIDsStatistics.value(*it)+"')\">"+protect2(*it)+"</span>"; break;
									default: tmpT+=protect2(*it); break;
								}
								if(it!=teachersNames.end()-1)
									tmpT+=", ";
							}
							if(printActivityTags){
								for(QStringList::Iterator atn=activityTagsNames.begin(); atn!=activityTagsNames.end(); atn++){
									switch(htmlLevel){
										case 3 : tmpT+=" <span class=\"activitytag\">"+protect2(*atn)+"</span>"; break;
										case 4 : tmpT+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\">"+protect2(*atn)+"</span></span>"; break;
										case 5 : ;
										case 6 : tmpT+=" <span class=\"activitytag\"><span class=\"at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"\" onmouseover=\"highlight('at_"+statisticValues.hashActivityTagIDsStatistics.value(*atn)+"')\">"+protect2(*atn)+"</span></span>"; break;
										default: tmpT+=" "+protect2(*atn); break;
									}
									if(atn!=activityTagsNames.end()-1)
										tmpT+=", ";
								}
							}
						} else
							tmpT=" ";
						tmp+=tmpT;
						
						tmp+="</td>";
					}
					tmp+="</tr>";
					tmp+="</table></td>\n";
				}
			}
		}
		tmp+="          <th>";
		tmp+=CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(subjects));
		if(statisticValues.subjectsTotalNumberOfHours.value(subjects)!=statisticValues.subjectsTotalNumberOfHours4.value(subjects))
			tmp+="<br />("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(subjects))+")";
		tmp+="</th>\n";
		tmp+="        </tr>\n";
	}
	
	progress.setValue(statisticValues.allSubjectsNames.count());

	tmp+="        <tr>\n";
	if(htmlLevel>=2)
		tmp+="          <th class=\"xAxis\">";
	else
		tmp+="          <th>";
	tmp+=protect2(tr("Sum", "This means the sum of more values, the total"))+"</th>\n";
	currentCount=0;
	for(int students=0; students<statisticValues.allStudentsNames.count() && currentCount<maxNames; students++){
		if(!(*excludedNames).contains(students)){
			currentCount++;
			tmp+="          <th>"+CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students)));
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))!=statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))
				tmp+="<br />("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))+")";
			tmp+="</th>\n";	
			*excludedNames<<students;
		}
	}
	tmp+="          <th></th>\n        </tr>\n";
	//workaround begin.
	tmp+="      <tr class=\"foot\"><td></td><td colspan=\""+QString::number(colspan+1)+"\">"+StatisticsExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)+"</td></tr>\n";
	//workaround end.
	tmp+="      </tbody>\n";
	tmp+="    </table>\n";
	return tmp;
}
