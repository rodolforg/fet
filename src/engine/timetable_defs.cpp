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
const QString FET_VERSION="5.30.8";

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

/**
Timetable html css javaScript Level, by Volker Dirr
*/
int TIMETABLE_HTML_LEVEL;

bool TIMETABLE_HTML_PRINT_ACTIVITY_TAGS;

bool PRINT_DETAILED_HTML_TIMETABLES;

bool PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS;

bool PRINT_NOT_AVAILABLE_TIME_SLOTS;

bool PRINT_BREAK_TIME_SLOTS;

bool PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME;

bool DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS;

bool TIMETABLE_HTML_REPEAT_NAMES;

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

int XX;
int YY;
int ZZ;

//random routines
void initRandomKnuth()
{
	assert(MM==2147483647);
	assert(AA==48271);
	assert(QQ==44488);
	assert(RR==3399);
	
	assert(MMM==2147483399);
	assert(MMM==MM-248);
	assert(AAA==40692);
	assert(QQQ==52774);
	assert(RRR==3791);
	
	//a few tests
	XX=123; YY=123;
	int tttt=randomKnuth1MM1();
	assert(XX==5937333);
	assert(YY==5005116);
	assert(tttt==932217);

	XX=4321; YY=54321;
	tttt=randomKnuth1MM1();
	assert(XX==208578991);
	assert(YY==62946733);
	assert(tttt==145632258);

	XX=87654321; YY=987654321;
	tttt=randomKnuth1MM1();
	assert(XX==618944401);
	assert(YY==1625301246);
	assert(tttt==1141126801);

	XX=1; YY=1;
	tttt=randomKnuth1MM1();
	assert(XX==48271);
	assert(YY==40692);
	assert(tttt==7579);

	XX=MM-1; YY=MMM-1;
	tttt=randomKnuth1MM1();
	assert(XX==2147435376);
	assert(YY==2147442707);
	assert(tttt==2147476315);

	XX=100; YY=1000;
	tttt=randomKnuth1MM1();
	assert(XX==4827100);
	assert(YY==40692000);
	assert(tttt==2111618746);
	//////////
	
	//unsigned tt=unsigned(time(NULL));
	qint64 tt=qint64(time(NULL));
	
	//XX is the current time
	//XX = 1 + ( (unsigned(tt)) % (unsigned(MM-1)) );
	XX = 1 + int( tt%(qint64(MM-1)) );
	assert(XX>0);
	assert(XX<MM);

	//YY is the next random, after initializing YY with the current time
	//YY = 1 + ( (unsigned(tt)) % (unsigned(MMM-1)) );
	YY = 1 + int( tt%(qint64(MMM-1)) );
	assert(YY>0);
	assert(YY<MMM);
	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;
	assert(YY>0);
	assert(YY<MMM);
	
	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).
}

int randomKnuth1MM1()
{
	assert(XX>0);
	assert(XX<MM);

	XX=AA*(XX%QQ)-RR*(XX/QQ);
	if(XX<0)
		XX+=MM;

	assert(XX>0);
	assert(XX<MM);

	assert(YY>0);
	assert(YY<MMM);

	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;
	
	assert(YY>0);
	assert(YY<MMM);

	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).
	
	return ZZ;
}

int randomKnuth(int k)
{
	//like in Knuth TAOCP vol.2, reject some numbers (very few), so that the distribution is perfectly uniform
	for(;;){
		int U=randomKnuth1MM1();
		if( U <= k * ((MM-1)/k) )
			return U%k;
	}
}
