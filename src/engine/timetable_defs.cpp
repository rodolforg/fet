/*
File timetable_defs.cpp
*/

/***************************************************************************
                          timetable_defs.cpp  -  description
                             -------------------
    begin                : Sat Mar 15 2003
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

#include "timetable_defs.h"

#include <ctime>

#include <QHash>

#include <QLocale>

bool checkForUpdates;

QString internetVersion;

/**
FET version
*/
const QString FET_VERSION="5.32.3";

/**
FET language
*/
QString FET_LANGUAGE="en_US";

/**
The output directory. Please be careful when editing it,
because the functions add a FILE_SEP sign at the end of it
and then the name of a file. If you make OUTPUT_DIR="",
there will be problems.
*/
QString OUTPUT_DIR;

bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

QString LANGUAGE_FOR_HTML;

bool VERBOSE;

//these hashes are needed to get the IDs for html and css in timetableexport and statistics
/*QHash<QString, QString> hashSubjectIDs;
QHash<QString, QString> hashActivityTagIDs;
QHash<QString, QString> hashStudentIDs;
QHash<QString, QString> hashTeacherIDs;
QHash<QString, QString> hashRoomIDs;
QHash<QString, QString> hashDayIDs;*/

/**
A log file explaining how the xml input file was parsed
*/
const QString XML_PARSING_LOG_FILENAME="file_open.log";

/**
The predefined names of the days of the week
*/
const QString PREDEFINED_DAYS_OF_THE_WEEK[]={"Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday", "Sunday", "Monday2",
	"Tuesday2", "Wednesday2", "Thursday2", "Friday2", "Saturday2", "Sunday2",
	"Monday3", "Tuesday3", "Wednesday3",
	"Thursday3", "Friday3", "Saturday3", "Sunday3", "Monday4",
	"Tuesday4", "Wednesday4", "Thursday4", "Friday4", "Saturday4", "Sunday4"};

/**
File and directory separator
*/
const QString FILE_SEP="/";

QString protect(const QString& str) //used for xml
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	p.replace("'", "&apos;");
	return p;
}

QString protect2(const QString& str) //used for html
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&apos;");
	return p;
}

QString protect2vert(const QString& str) //used for html
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&apos;");

	QString returnstring;
	for(int i=0; i<p.size();i++){
		QString a=p.at(i);
		QString b="<br />";
		returnstring.append(a);
		returnstring.append(b);
	}
	return returnstring;
}

///////begin tricks
void weight_sscanf(const QString& str, const char* fmt, double* result)
{
	assert(QString(fmt)==QString("%lf"));

	bool ok;
	double myres=customFETStrToDouble(str, &ok);
	if(!ok)
		(*result)=-2.5; //any value that does not belong to {>=0.0 and <=100.0} or {-1.0}
						//not -1.0 because of modify multiple constraints min days between activities,
						//-1 there represents any weight
						//potential bug found by Volker Dirr
	else
		(*result)=myres;
}

QString CustomFETString::number(int n)
{
	return QString::number(n);
}

QString CustomFETString::number(double x)
{
	QString tmp=QString::number(x, 'f', CUSTOM_DOUBLE_PRECISION);
	
	//remove trailing zeroes AFTER decimal points
	if(tmp.contains('.')){
		int n=tmp.length()-1;
		int del=0;
		while(tmp.at(n)=='0'){
			n--;
			del++;
		}
		if(tmp.at(n)=='.'){
			n--;
			del++;
		}
		tmp.chop(del);
	}

	return tmp;
}

double customFETStrToDouble(const QString& str, bool* ok)
{
	QLocale c(QLocale::C);

	//tricks to convert numbers like 97.123456789 to 97.123457, to CUSTOM_DOUBLE_PRECISION (6) decimal digits after decimal point
	double tmpd=c.toDouble(str, ok);
	if(ok!=0)
		if((*ok)==false)
			return tmpd;
	QString tmps=CustomFETString::number(tmpd);
	return c.toDouble(tmps, ok);
}
///////end tricks
