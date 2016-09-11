/*
File import.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          import.cpp  -  description
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

//TODO: import days per week
//TODO: import hours per day

#include "import.h"

#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QProgressDialog>

#include <QSet>
#include <QHash>
#include <QList>
#include <QPair>

#include "centerwidgetonscreen.h"

extern Timetable gt;

// maybe I can add some of them again as parameter into function name?
// it is not possible with fieldNumber and fieldName, because that conflicts with the using of command connect with chooseFieldsDialogUpdate (there are no parameters possible)
static QString fileName;				// file name of the csv file

static QString fieldSeparator=",";
static QString textquote="\"";
static QString setSeparator="+";

static QString importThing;

static int numberOfFields;				// number of fields per line of the csv file
static QStringList fields;				// list of the fields of the first line of the csv file
static bool head;					// true = first line of csv file contain heading, so skip this line
static QString fieldName[NUMBER_OF_FIELDS];		// field name (normally similar to the head)
static int fieldNumber[NUMBER_OF_FIELDS];		// field number in the csv file
							// fieldNumber >= 0 is the number of the field
							// fieldNumber can also be IMPORT_DEFAULT_ITEM (==IMPORT_DEFAULT_ITEM). That mean that items of fieldList get the name of fieldDefaultItem (not of field[fieldNumber] from csv
							// fieldNumber can also be DO_NOT_IMPORT (==-2). That mean that items are not imported into fieldList.
static QStringList fieldList[NUMBER_OF_FIELDS];	// items of the fields (items from "field number")
static QString fieldDefaultItem[NUMBER_OF_FIELDS];	// used, if fieldNumber == IMPORT_DEFAULT_ITEM
static QString warnText;				// warnings about the csv file
static QStringList dataWarning;			// warnings about the current conflicts between the csv file and the data that is already in memory
static QString lastWarning;


int Import::chooseWidth(int w)
{
	int ww=w;
	if(ww>1000)
		ww=1000;
	
	return ww;
}

int Import::chooseHeight(int h)
{
	int hh=h;
	if(hh>650)
		hh=650;
	
	return hh;
}

Import::Import()
{
}

Import::~Import()
{
}

void Import::prearrangement(){
	assert(gt.rules.initialized);

	fieldName[FIELD_LINE_NUMBER]=Import::tr("Line");
	fieldName[FIELD_YEAR_NAME]=Import::tr("Year");
	fieldName[FIELD_YEAR_NUMBER_OF_STUDENTS]=Import::tr("Number of Students per Year");
	fieldName[FIELD_GROUP_NAME]=Import::tr("Group");
	fieldName[FIELD_GROUP_NUMBER_OF_STUDENTS]=Import::tr("Number of Students per Group");
	fieldName[FIELD_SUBGROUP_NAME]=Import::tr("Subgroup");
	fieldName[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]=Import::tr("Number of Students per Subgroup");
	fieldName[FIELD_SUBJECT_NAME]=Import::tr("Subject");
	fieldName[FIELD_ACTIVITY_TAG_NAME]=Import::tr("Activity Tag");
	fieldName[FIELD_TEACHER_NAME]=Import::tr("Teacher");
	fieldName[FIELD_BUILDING_NAME]=Import::tr("Building");
	fieldName[FIELD_ROOM_NAME]=Import::tr("Room Name");
	fieldName[FIELD_ROOM_CAPACITY]=Import::tr("Room Capacity");
	fieldName[FIELD_STUDENTS_SET]=Import::tr("Students Sets");
	fieldName[FIELD_TEACHERS_SET]=Import::tr("Teachers");
	fieldName[FIELD_TOTAL_DURATION]=Import::tr("Total Duration");
	fieldName[FIELD_SPLIT_DURATION]=Import::tr("Split Duration");
	fieldName[FIELD_MIN_DAYS]=Import::tr("Min Days");
	fieldName[FIELD_MIN_DAYS_WEIGHT]=Import::tr("Min Days Weight");
	fieldName[FIELD_MIN_DAYS_CONSECUTIVE]=Import::tr("Min Days Consecutive");
	fieldName[FIELD_ACTIVITY_TAGS_SET]=Import::tr("Activity Tags");
	for(int i=0; i<NUMBER_OF_FIELDS; i++){
		fieldNumber[i]=DO_NOT_IMPORT;
		fieldDefaultItem[i]="";
		fieldList[i].clear();
	}
	fieldNumber[FIELD_LINE_NUMBER]=IMPORT_DEFAULT_ITEM;
	fieldDefaultItem[FIELD_LINE_NUMBER]=Import::tr("line");
	dataWarning.clear();
	warnText.clear();
	lastWarning.clear();
}


//TODO: add this into the first function!? form too full?!
ChooseFieldsDialog::ChooseFieldsDialog(QWidget *parent): QDialog(parent)
{
	assert(fields.size()>0);

	this->setWindowTitle(tr("FET - Import from CSV file"));
	QGridLayout* chooseFieldsMainLayout=new QGridLayout(this);

	QVBoxLayout* fieldBox[NUMBER_OF_FIELDS];
	QHBoxLayout* fieldLine1[NUMBER_OF_FIELDS];
	QHBoxLayout* fieldLine2[NUMBER_OF_FIELDS];
	QHBoxLayout* fieldLine3[NUMBER_OF_FIELDS];
	QHBoxLayout* fieldLine3b[NUMBER_OF_FIELDS];

	int gridRow=0;
	int gridColumn=0;
	for(int i=1; i<NUMBER_OF_FIELDS; i++){
		assert(fieldNumber[i]==DO_NOT_IMPORT || fieldNumber[i]==IMPORT_DEFAULT_ITEM);
		fieldGroupBox[i] = new QGroupBox(Import::tr("Please specify the %1 field:").arg(fieldName[i]));
		fieldBox[i] = new QVBoxLayout();
		fieldRadio1[i] = new QRadioButton(Import::tr("Don't import \"%1\".").arg(fieldName[i]));
		fieldRadio2[i] = new QRadioButton(Import::tr("Import this field from CSV:"));
		
		//trick to keep the translation active, maybe we need it in the future
		QString s=Import::tr("Set always the same name:");
		Q_UNUSED(s);
		
		//fieldRadio3[i] = new QRadioButton(Import::tr("Set always the same name:"));
		fieldRadio3[i] = new QRadioButton(Import::tr("Set always the same value:")); //modified by Liviu on 18 March 2009
		fieldRadio3b[i] = new QRadioButton(Import::tr("Set always the same value:"));

		fieldLine1[i] = new QHBoxLayout();
		fieldLine1[i]->addWidget(fieldRadio1[i]);
		fieldBox[i]->addLayout(fieldLine1[i]);
		
		fieldLine2CB[i] = new QComboBox();
		fieldLine2CB[i]->setMaximumWidth(220);			//max
		fieldLine2CB[i]->insertItems(0,fields);
		fieldLine2[i] = new QHBoxLayout();
		fieldLine2[i]->addWidget(fieldRadio2[i]);
		fieldLine2[i]->addWidget(fieldLine2CB[i]);
		fieldBox[i]->addLayout(fieldLine2[i]);
		fieldRadio2[i]->setChecked(true);

		fieldLine3Text[i] = new QLineEdit(Import::tr("Please modify this text."));
//		fieldLine3Text[i]->setMaximumWidth(220);		//max
		fieldLine3[i] = new QHBoxLayout();
		fieldLine3[i]->addWidget(fieldRadio3[i]);
		
		//Added by Liviu - 18 March 2009, so that the dialog looks nice when dialog is maximized
		
		//TODO: add this line or not???
		fieldLine3[i]->addStretch();
		//fieldLine3Text[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
		
		fieldLine3[i]->addWidget(fieldLine3Text[i]);
		fieldBox[i]->addLayout(fieldLine3[i]);

		fieldLine3bSpinBox[i] = new QSpinBox();
		fieldLine3bSpinBox[i]->setMaximumWidth(220);		//max
		fieldLine3b[i] = new QHBoxLayout();
		fieldLine3b[i]->addWidget(fieldRadio3b[i]);
		fieldLine3b[i]->addWidget(fieldLine3bSpinBox[i]);
		fieldBox[i]->addLayout(fieldLine3b[i]);

		fieldGroupBox[i]->setLayout(fieldBox[i]);
		chooseFieldsMainLayout->addWidget(fieldGroupBox[i], gridRow, gridColumn);
		if(fieldNumber[i]==DO_NOT_IMPORT)
			fieldGroupBox[i]->hide();
		else {
			if(gridColumn==1){
				gridColumn=0;
				gridRow++;
			} else
				gridColumn++;
		}

		if(i==FIELD_YEAR_NAME || i==FIELD_TEACHER_NAME)
			fieldRadio1[i]->hide();
		if(i==FIELD_ROOM_CAPACITY){
			fieldRadio1[i]->hide();
			fieldLine3bSpinBox[i]->setMaximum(MAX_ROOM_CAPACITY);
			fieldLine3bSpinBox[i]->setMinimum(0);
			fieldLine3bSpinBox[i]->setValue(MAX_ROOM_CAPACITY);
			fieldRadio3b[i]->setChecked(true);
			fieldRadio3[i]->hide();
			fieldLine3Text[i]->hide();
		} else if(i==FIELD_YEAR_NUMBER_OF_STUDENTS || i==FIELD_GROUP_NUMBER_OF_STUDENTS || i==FIELD_SUBGROUP_NUMBER_OF_STUDENTS){
			fieldRadio1[i]->hide();
			fieldLine3bSpinBox[i]->setMaximum(MAX_TOTAL_SUBGROUPS);
			fieldLine3bSpinBox[i]->setMinimum(0);
			fieldLine3bSpinBox[i]->setValue(0);
			fieldRadio3b[i]->setChecked(true);
			fieldRadio3[i]->hide();
			fieldLine3Text[i]->hide();
		} else {
			fieldRadio3b[i]->hide();
			fieldLine3bSpinBox[i]->hide();
		}
		if(i==FIELD_SUBJECT_NAME){
			fieldRadio1[i]->hide();
			fieldRadio3[i]->hide();
			fieldLine3Text[i]->hide();
		}
		if(i==FIELD_ACTIVITY_TAG_NAME){
			fieldRadio1[i]->hide();
			fieldRadio3[i]->hide();
			fieldLine3Text[i]->hide();
		}
		if(i==FIELD_ROOM_NAME){
			fieldRadio3[i]->hide();
			fieldLine3Text[i]->hide();
		}
		
		if(i==FIELD_TOTAL_DURATION){
			fieldRadio1[i]->hide();
			fieldLine3Text[i]->setText("1");
		}
		if(i==FIELD_SPLIT_DURATION){
			fieldRadio1[i]->hide();
			fieldLine3Text[i]->setText("1");
		}
		if(i==FIELD_MIN_DAYS){
			fieldRadio1[i]->hide();
			fieldLine3Text[i]->setText("1");
		}
		if(i==FIELD_MIN_DAYS_WEIGHT){
			fieldRadio1[i]->hide();
			fieldLine3Text[i]->setText("95");
		}
		if(i==FIELD_MIN_DAYS_CONSECUTIVE){
			fieldRadio1[i]->hide();
			fieldLine3Text[i]->setText("1");
		}
	}

	gridRow++;
	/*
	pb=new QPushButton(tr("OK"));
	chooseFieldsMainLayout->addWidget(pb,gridRow,1);
	cancelpb=new QPushButton(tr("Cancel"));
	chooseFieldsMainLayout->addWidget(cancelpb,gridRow,2);*/
	pb=new QPushButton(tr("OK"));
	cancelpb=new QPushButton(tr("Cancel"));
	buttonsLayout=new QHBoxLayout();
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(pb);
	buttonsLayout->addWidget(cancelpb);
	chooseFieldsMainLayout->addLayout(buttonsLayout,gridRow,1);

	chooseFieldsDialogUpdateRadio1();
	chooseFieldsDialogUpdateRadio2();
	chooseFieldsDialogUpdateRadio3();
	chooseFieldsDialogUpdateRadio3b();

	//connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
	connect(pb, SIGNAL(clicked()), this, SLOT(chooseFieldsDialogClose()));
	connect(cancelpb, SIGNAL(clicked()), this, SLOT(reject()));
	for(int i=1; i<NUMBER_OF_FIELDS; i++){
		connect(fieldRadio1[i], SIGNAL(toggled(bool)), this, SLOT(chooseFieldsDialogUpdateRadio1()));
		connect(fieldRadio2[i], SIGNAL(toggled(bool)), this, SLOT(chooseFieldsDialogUpdateRadio2()));
		connect(fieldRadio3[i], SIGNAL(toggled(bool)), this, SLOT(chooseFieldsDialogUpdateRadio3()));
		connect(fieldRadio3b[i], SIGNAL(toggled(bool)), this, SLOT(chooseFieldsDialogUpdateRadio3b()));
		connect(fieldLine3Text[i], SIGNAL(textChanged(QString)), this, SLOT(chooseFieldsDialogUpdateLine3Text()));
	}
	
	pb->setDefault(true);
	pb->setFocus();
	
	//_settingsName=settingsName;
	//restoreFETDialogGeometry(this, _settingsName);
}

ChooseFieldsDialog::~ChooseFieldsDialog()
{
	//saveFETDialogGeometry(this, _settingsName);
}

void ChooseFieldsDialog::chooseFieldsDialogUpdateRadio1(){
	if(fieldRadio1[FIELD_GROUP_NAME]->isChecked()){
		fieldRadio1[FIELD_GROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		fieldRadio1[FIELD_SUBGROUP_NAME]->setChecked(true);
		fieldRadio1[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		fieldGroupBox[FIELD_GROUP_NUMBER_OF_STUDENTS]->setDisabled(true);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(true);
		fieldGroupBox[FIELD_SUBGROUP_NAME]->setDisabled(true);
	}
	if(fieldRadio1[FIELD_SUBGROUP_NAME]->isChecked()){
		fieldRadio1[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(true);
	}
	if(fieldRadio1[FIELD_ROOM_NAME]->isChecked()){
		fieldRadio1[FIELD_ROOM_CAPACITY]->setChecked(true);
		fieldGroupBox[FIELD_ROOM_CAPACITY]->setDisabled(true);
	}
	if(fieldRadio1[FIELD_BUILDING_NAME]->isChecked()&&fieldRadio1[FIELD_ROOM_NAME]->isChecked()){
		pb->setDisabled(true);
	}
	if((fieldRadio1[FIELD_BUILDING_NAME]->isChecked()&&!fieldRadio1[FIELD_ROOM_NAME]->isChecked()&&fieldLine3Text[FIELD_BUILDING_NAME]->displayText()!="")
		||(!fieldRadio1[FIELD_BUILDING_NAME]->isChecked()&&fieldRadio1[FIELD_ROOM_NAME]->isChecked()&&fieldLine3Text[FIELD_BUILDING_NAME]->displayText()!="")){
		pb->setDisabled(false);
	}
}


void ChooseFieldsDialog::chooseFieldsDialogUpdateRadio2(){
	if(fieldRadio2[FIELD_GROUP_NAME]->isChecked()){
		fieldGroupBox[FIELD_SUBGROUP_NAME]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_GROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		if(fieldRadio1[FIELD_GROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_GROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		if(fieldRadio1[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setChecked(true);
	}
	if(fieldRadio2[FIELD_ROOM_NAME]->isChecked()){
		if(fieldRadio1[FIELD_ROOM_CAPACITY]->isChecked())
			fieldRadio3b[FIELD_ROOM_CAPACITY]->setChecked(true);
		fieldGroupBox[FIELD_ROOM_CAPACITY]->setDisabled(false);
	}
}


void ChooseFieldsDialog::chooseFieldsDialogUpdateRadio3(){
	if(fieldRadio3[FIELD_GROUP_NAME]->isChecked()){
		fieldGroupBox[FIELD_SUBGROUP_NAME]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_GROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		if(fieldRadio1[FIELD_GROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_GROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		if(fieldRadio1[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setChecked(true);
	}
	if(fieldRadio3[FIELD_ROOM_NAME]->isChecked()){
		if(fieldRadio1[FIELD_ROOM_CAPACITY]->isChecked())
			fieldRadio3b[FIELD_ROOM_CAPACITY]->setChecked(true);
		fieldGroupBox[FIELD_ROOM_CAPACITY]->setDisabled(false);
	}
}

void ChooseFieldsDialog::chooseFieldsDialogUpdateRadio3b(){
	if(fieldRadio3b[FIELD_GROUP_NAME]->isChecked()){
		fieldGroupBox[FIELD_SUBGROUP_NAME]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_GROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		fieldGroupBox[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setDisabled(false);
		if(fieldRadio1[FIELD_GROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_GROUP_NUMBER_OF_STUDENTS]->setChecked(true);
		if(fieldRadio1[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->isChecked())
			fieldRadio3b[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]->setChecked(true);
	}
	if(fieldRadio3b[FIELD_ROOM_NAME]->isChecked()){
		if(fieldRadio1[FIELD_ROOM_CAPACITY]->isChecked())
			fieldRadio3b[FIELD_ROOM_CAPACITY]->setChecked(true);
		fieldGroupBox[FIELD_ROOM_CAPACITY]->setDisabled(false);
	}
}


void ChooseFieldsDialog::chooseFieldsDialogUpdateLine3Text(){
	bool textOK=true;
	for(int i=1; i<NUMBER_OF_FIELDS; i++){
		if(fieldLine3Text[i]->displayText()=="")
			textOK=false;
	}
	//by Liviu - always enabled
	if(1 || textOK)
		pb->setDisabled(false);
	else
		pb->setDisabled(true);
}


void ChooseFieldsDialog::chooseFieldsDialogClose(){
	for(int i=1; i<NUMBER_OF_FIELDS; i++){
		if(fieldNumber[i]!=DO_NOT_IMPORT){
			if(fieldRadio1[i]->isChecked()){
				fieldNumber[i]=DO_NOT_IMPORT;
			}
			if(fieldRadio2[i]->isChecked()){
				fieldNumber[i]=fieldLine2CB[i]->currentIndex();
				assert(fieldNumber[i]<fields.size());
				assert(fieldNumber[i]>=0);
			}
			if(fieldRadio3[i]->isChecked()){
				fieldNumber[i]=IMPORT_DEFAULT_ITEM;
				//fieldName[i]=fieldLine3CB[i]->currentText();
				fieldDefaultItem[i]=fieldLine3Text[i]->displayText();
			}
			if(fieldRadio3b[i]->isChecked()){
				fieldNumber[i]=IMPORT_DEFAULT_ITEM;
				fieldDefaultItem[i]=fieldLine3bSpinBox[i]->cleanText();
			}
		}
	}
	
	this->accept();
}


LastWarningsDialog::LastWarningsDialog(QWidget *parent): QDialog(parent)
{
	this->setWindowTitle(tr("FET - import %1 comment", "The comment of the importing of the category named %1").arg(importThing));
	QVBoxLayout* lastWarningsMainLayout=new QVBoxLayout(this);

	QPlainTextEdit* lastWarningsText=new QPlainTextEdit();
	lastWarningsText->setMinimumWidth(500);				//width
	lastWarningsText->setMinimumHeight(250);
	lastWarningsText->setReadOnly(true);
	lastWarningsText->setWordWrapMode(QTextOption::NoWrap);
	lastWarningsText->setPlainText(lastWarning);

	//Start Buttons
	QPushButton* pb1=new QPushButton(tr("&OK"));
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

LastWarningsDialog::~LastWarningsDialog()
{
}

// private funtions ---------------------------------------------------------------------------------------------------
int Import::getFileSeparatorFieldsAndHead(QWidget* parent, QDialog* &newParent){
	assert(gt.rules.initialized);
	
	newParent=((QDialog*)parent);
	
	QString settingsName;

	if(fieldNumber[FIELD_ACTIVITY_TAG_NAME]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("activity tags");
		settingsName=QString("ImportActivityTagsSelectSeparatorsDialog");
	}
	if(fieldNumber[FIELD_ROOM_NAME]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("buildings and rooms");
		settingsName=QString("ImportBuildingsRoomsSelectSeparatorsDialog");
	}
	if(fieldNumber[FIELD_TEACHER_NAME]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("teachers");
		settingsName=QString("ImportTeachersSelectSeparatorsDialog");
	}
	if(fieldNumber[FIELD_SUBJECT_NAME]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("subjects");
		settingsName=QString("ImportSubjectsSelectSeparatorsDialog");
	}
	if(fieldNumber[FIELD_YEAR_NAME]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("years, groups and subgroups");
		settingsName=QString("ImportYearsGroupsSubgroupsSelectSeparatorsDialog");
	}
	if(fieldNumber[FIELD_STUDENTS_SET]==IMPORT_DEFAULT_ITEM){
		importThing=Import::tr("activities");
		settingsName=QString("ImportActivitiesSelectSeparatorsDialog");
	}

	fileName=QFileDialog::getOpenFileName(parent, Import::tr("FET - Import %1 from CSV file").arg(importThing), IMPORT_DIRECTORY, 
		Import::tr("Text Files")+" (*.csv *.dat *.txt)" + ";;" + Import::tr("All Files") + " (*)");

	const QString NO_SEPARATOR_TRANSLATED=Import::tr("no separator");
	fieldSeparator=NO_SEPARATOR_TRANSLATED;	//needed, because a csv file contain maybe just one field!
	const QString NO_TEXTQUOTE_TRANSLATED=Import::tr("no textquote");
	textquote=NO_TEXTQUOTE_TRANSLATED;
	fields.clear();
	QFile file(fileName);
	if(fileName.isEmpty()){
		return false;
	}
	if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
		return false;
	}
	QTextStream in(&file);
	in.setCodec("UTF-8");
	QString line = in.readLine();
	file.close();
	
	if(line.size()<=0){
		QMessageBox::warning(parent, tr("FET warning"), tr("The first line of the file is empty. Please fix this."));
		return false;
	}
	
	if(fieldNumber[FIELD_ACTIVITY_TAG_NAME]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Activity Tag\"")
		&& line.size()<=QString("\"Activity Tag\"").length()+1
		&& line.size()>=QString("\"Activity Tag\"").length()){
		fieldNumber[FIELD_ACTIVITY_TAG_NAME]=0;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields<<line;
		return true;
	}

	if(fieldNumber[FIELD_ROOM_NAME]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Room\",\"Room Capacity\",\"Building\"")
		&& line.size()<=QString("\"Room\",\"Room Capacity\",\"Building\"").length()+1
		&& line.size()>=QString("\"Room\",\"Room Capacity\",\"Building\"").length()){
		fieldNumber[FIELD_BUILDING_NAME]=2;
		fieldNumber[FIELD_ROOM_NAME]=0;
		fieldNumber[FIELD_ROOM_CAPACITY]=1;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields=line.split(fieldSeparator);
		return true;
	}

	if(fieldNumber[FIELD_TEACHER_NAME]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Teacher\"")
		&& line.size()<=QString("\"Teacher\"").length()+1
		&& line.size()>=QString("\"Teacher\"").length()){
		fieldNumber[FIELD_TEACHER_NAME]=0;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields<<line;
		return true;
	}

	if(fieldNumber[FIELD_SUBJECT_NAME]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Subject\"")
		&& line.size()<=QString("\"Subject\"").length()+1
		&& line.size()>=QString("\"Subject\"").length()){
		fieldNumber[FIELD_SUBJECT_NAME]=0;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields<<line;
		return true;
	}

	if(fieldNumber[FIELD_YEAR_NAME]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Year\",\"Number of Students per Year\",\"Group\",\"Number of Students per Group\",\"Subgroup\",\"Number of Students per Subgroup\"")
		&& line.size()<=QString("\"Year\",\"Number of Students per Year\",\"Group\",\"Number of Students per Group\",\"Subgroup\",\"Number of Students per Subgroup\"").length()+1
		&& line.size()>=QString("\"Year\",\"Number of Students per Year\",\"Group\",\"Number of Students per Group\",\"Subgroup\",\"Number of Students per Subgroup\"").length()){
		fieldNumber[FIELD_YEAR_NAME]=0;
		fieldNumber[FIELD_YEAR_NUMBER_OF_STUDENTS]=1;
		fieldNumber[FIELD_GROUP_NAME]=2;
		fieldNumber[FIELD_GROUP_NUMBER_OF_STUDENTS]=3;
		fieldNumber[FIELD_SUBGROUP_NAME]=4;
		fieldNumber[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]=5;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields=line.split(fieldSeparator);
		return true;
	}

	if(fieldNumber[FIELD_STUDENTS_SET]==IMPORT_DEFAULT_ITEM
		&& line.contains("\"Students Sets\",\"Subject\",\"Teachers\",\"Activity Tags\",\"Total Duration\",\"Split Duration\",\"Min Days\",\"Weight\",\"Consecutive\"")
		&& line.size()<=QString("\"Students Sets\",\"Subject\",\"Teachers\",\"Activity Tags\",\"Total Duration\",\"Split Duration\",\"Min Days\",\"Weight\",\"Consecutive\"").length()+1
		&& line.size()>=QString("\"Students Sets\",\"Subject\",\"Teachers\",\"Activity Tags\",\"Total Duration\",\"Split Duration\",\"Min Days\",\"Weight\",\"Consecutive\"").length()){
		fieldNumber[FIELD_ACTIVITY_TAGS_SET]=3;
		fieldNumber[FIELD_SUBJECT_NAME]=1;
		fieldNumber[FIELD_STUDENTS_SET]=0;
		fieldNumber[FIELD_TEACHERS_SET]=2;
		fieldNumber[FIELD_TOTAL_DURATION]=4;
		fieldNumber[FIELD_SPLIT_DURATION]=5;
		fieldNumber[FIELD_MIN_DAYS]=6;
		fieldNumber[FIELD_MIN_DAYS_WEIGHT]=7;
		fieldNumber[FIELD_MIN_DAYS_CONSECUTIVE]=8;
		head=true;
		fieldSeparator=",";
		textquote="\"";
		fields=line.split(fieldSeparator);
		return true;
	}

	QStringList separators;
	QStringList textquotes;
	separators<<fieldSeparator;
	const int NO_SEPARATOR_POS=0; //it is the first element. It may have length > 1 QChar
	textquotes<<textquote;
	const int NO_TEXTQUOTE_POS=0; //it is the first element. It may have length > 1 QChar
	for(int i=0; i<line.size();i++){
		//if(!(line.at(i)>='A'&&line.at(i)<='Z')&&!(line.at(i)>='a'&&line.at(i)<='z')&&!(line.at(i)>='0'&&line.at(i)<='9')&&!separators.contains(line.at(i))){
		if(!(line.at(i).isLetterOrNumber())&&!separators.contains(line.at(i))){
			separators<<line.at(i);
			//careful: if you intend to add strings longer than one QChar, take care of assert in line 647 (below in the same function) (fieldSeparator.size()==1)
		}
		if(!(line.at(i).isLetterOrNumber())&&!textquotes.contains(line.at(i))){
			textquotes<<line.at(i);
			//careful: if you intend to add strings longer than one QChar, take care of assert in line 659 (below in the same function) (textquote.size()==1)
		}
	}

	newParent=new QDialog(parent);
	QDialog& separatorsDialog=(*newParent);
	separatorsDialog.setWindowTitle(Import::tr("FET - Import %1 from CSV file").arg(importThing));
	QVBoxLayout* separatorsMainLayout=new QVBoxLayout(&separatorsDialog);

	QHBoxLayout* top=new QHBoxLayout();
	QLabel* topText=new QLabel();

	int tmpi=fileName.lastIndexOf(FILE_SEP);
	tmpi=fileName.size()-tmpi-1;
	QString shortFileName=fileName.right(tmpi);
	topText->setText(Import::tr("The first line of file\n%1\nis:").arg(shortFileName));
	top->addWidget(topText);
	top->addStretch();
	QPlainTextEdit* textOfFirstLine=new QPlainTextEdit();
	textOfFirstLine->setReadOnly(true);
	textOfFirstLine->setPlainText(line);

	QGroupBox* separatorsGroupBox = new QGroupBox(Import::tr("Please specify the used separator between fields:"));
	QComboBox* separatorsCB=NULL;
	if(separators.size()>1){
		QHBoxLayout* separatorBoxChoose=new QHBoxLayout();
		separatorsCB=new QComboBox();
		
		QLabel* separatorTextChoose=new QLabel();
		separatorTextChoose->setText(Import::tr("Used field separator:"));
		separatorsCB->insertItems(0,separators);
		separatorBoxChoose->addWidget(separatorTextChoose);
		separatorBoxChoose->addWidget(separatorsCB);
		separatorsGroupBox->setLayout(separatorBoxChoose);
	}

	QGroupBox* textquoteGroupBox = new QGroupBox(Import::tr("Please specify the used text quote of text fields:"));
	QComboBox* textquoteCB=NULL;
	if(textquotes.size()>1){
		QHBoxLayout* textquoteBoxChoose=new QHBoxLayout();
		textquoteCB=new QComboBox();
		
		QLabel* textquoteTextChoose=new QLabel();
		textquoteTextChoose->setText(Import::tr("Used textquote:"));
		textquoteCB->insertItems(0,textquotes);
		textquoteBoxChoose->addWidget(textquoteTextChoose);
		textquoteBoxChoose->addWidget(textquoteCB);
		textquoteGroupBox->setLayout(textquoteBoxChoose);
	}

	QGroupBox* firstLineGroupBox = new QGroupBox(Import::tr("Please specify the contents of the first line:"));
	QVBoxLayout* firstLineChooseBox=new QVBoxLayout();
	QRadioButton* firstLineRadio1 = new QRadioButton(Import::tr("The first line is the heading. Don't import that line."));
	QRadioButton* firstLineRadio2 = new QRadioButton(Import::tr("The first line contains data. Import that line."));
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
	separatorsMainLayout->addWidget(textOfFirstLine);
	if(separators.size()>1){
		separatorsMainLayout->addWidget(separatorsGroupBox);
		separatorsMainLayout->addWidget(textquoteGroupBox);
	}
	else{
		delete separatorsGroupBox;
		delete textquoteGroupBox;
	}
	separatorsMainLayout->addWidget(firstLineGroupBox);
	separatorsMainLayout->addLayout(hl);
	QObject::connect(pb, SIGNAL(clicked()), &separatorsDialog, SLOT(accept()));
	QObject::connect(cancelpb, SIGNAL(clicked()), &separatorsDialog, SLOT(reject()));
	
	pb->setDefault(true);
	pb->setFocus();
	
	int w=chooseWidth(separatorsDialog.sizeHint().width());
	int h=chooseHeight(separatorsDialog.sizeHint().height());
	separatorsDialog.resize(w,h);
	centerWidgetOnScreen(&separatorsDialog);
	restoreFETDialogGeometry(&separatorsDialog, settingsName);
	
	int ok=separatorsDialog.exec();
	saveFETDialogGeometry(&separatorsDialog, settingsName);
	if(!ok) return false;
	
	if(separators.size()>1){
		assert(separatorsCB!=NULL);
		assert(textquoteCB!=NULL);
		fieldSeparator=separatorsCB->currentText();
		
		if(separatorsCB->currentIndex()==NO_SEPARATOR_POS){
			assert(fieldSeparator==NO_SEPARATOR_TRANSLATED);
			fieldSeparator=QString("no sep"); //must have length >= 2
		}
		else{
			assert(fieldSeparator.size()==1);
			//assert(!fieldSeparator.at(0).isLetterOrNumber());
		}
		
		textquote=textquoteCB->currentText();
		
		if(textquoteCB->currentIndex()==NO_TEXTQUOTE_POS){
			assert(textquote==NO_TEXTQUOTE_TRANSLATED);
			textquote=QString("no tquote"); //must have length >= 2
		}
		else{
			assert(textquote.size()==1);
			//assert(!textquote.at(0).isLetterOrNumber());
		}
	}
	else{
		assert(separatorsCB==NULL);
		assert(textquoteCB==NULL);
		fieldSeparator="";
		textquote="";
	}
//NEW start
			QString tmp;
			QString tmpLine=line;
			while(!tmpLine.isEmpty()){
				tmp.clear();
				bool foundField=false;
				if(tmpLine.left(1)==textquote){
					tmpLine.remove(0,1);
					while(!foundField && tmpLine.size()>1){
						if(tmpLine.left(1)!=textquote){
							tmp+=tmpLine.left(1);
						} else {
							if(tmpLine.mid(1,1)==fieldSeparator){
								foundField=true;
								tmpLine.remove(0,1);
							} else if(tmpLine.mid(1,1)==textquote){
								tmp+=textquote;
								tmpLine.remove(0,1);
							} else {
								QMessageBox::critical(newParent, tr("FET warning"), Import::tr("Missing field separator or text quote in first line. Import might be incorrect.")+"\n");
							}
						}
						tmpLine.remove(0,1);
					}
					if(!foundField && tmpLine.size()==1){
						if(tmpLine.left(1)==textquote){
							tmpLine.remove(0,1);
						} else {
							QMessageBox::critical(newParent, tr("FET warning"), Import::tr("Missing closing text quote in first line. Import might be incorrect.")+"\n");
							tmp+=tmpLine.left(1);
							tmpLine.remove(0,1);
						}
						
					}
				} else {
					while(!foundField && !tmpLine.isEmpty()){
						if(tmpLine.left(1)!=fieldSeparator)
							tmp+=tmpLine.left(1);
						else
							foundField=true;
						tmpLine.remove(0,1);
					}
				}
				fields << tmp;
				if(foundField && tmpLine.isEmpty())
					fields << "";
			}
//NEW end

/* OLD
	if(separators.size()>1){
		fieldSeparator=separatorsCB->currentText();
		fields=line.split(fieldSeparator);
	} else {
		fieldSeparator=separators.takeFirst();
		fields<<line;
	}
OLD */

	if(firstLineRadio1->isChecked())
		head=true;
	else head=false;
	return true;
}

int Import::readFields(QWidget* parent){
	QSet<QString> checkSet;
	QString check;
	numberOfFields=fields.size();
	assert(numberOfFields>0);
	for(int i=0; i<NUMBER_OF_FIELDS; i++){
		assert(fieldNumber[i]<=numberOfFields);
		assert(fieldNumber[i]>=DO_NOT_IMPORT);
	}

	QFile file(fileName);
	if(fileName.isEmpty()){
		QMessageBox::warning(parent, tr("FET warning"), tr("Empty filename."));
		return false;
	}
	if(!file.open(QIODevice::ReadOnly)){
		QMessageBox::warning(parent, tr("Error! Can't open file."),fileName);
		return false;
	}
	QTextStream in(&file);
	in.setCodec("UTF-8");

	qint64 size=file.size();
	QProgressDialog* _progress=new QProgressDialog(parent);
	QProgressDialog& progress=(*_progress);
	progress.setWindowTitle(tr("Importing", "Title of a progress dialog"));
	progress.setLabelText(tr("Loading file"));
	progress.setModal(true);
	progress.setRange(0, size);
	//cout<<"progress in readFields starts"<<endl;
	qint64 crt=0;

	QStringList fields;
	QString itemOfField[NUMBER_OF_FIELDS];
	int lineNumber=0;
	while(!in.atEnd()){
		progress.setValue(crt);
		QString line = in.readLine();
		lineNumber++;
		crt+=line.length();
		if(progress.wasCanceled()){
			progress.setValue(size);
			QMessageBox::warning(parent, "FET", Import::tr("Loading canceled by user."));
			file.close();
			return false;
		}
		bool ok=true;
		if(!(lineNumber==1&&head)){
			fields.clear();
			QString tmp;
			QString tmpLine=line;
			while(!tmpLine.isEmpty()){
				tmp.clear();
				bool foundField=false;
				if(tmpLine.left(1)==textquote){
					tmpLine.remove(0,1);
					while(!foundField && tmpLine.size()>1){
						if(tmpLine.left(1)!=textquote){
							tmp+=tmpLine.left(1);
						} else {
							if(tmpLine.mid(1,1)==fieldSeparator){
								foundField=true;
								tmpLine.remove(0,1);
							} else if(tmpLine.mid(1,1)==textquote){
								tmp+=textquote;
								tmpLine.remove(0,1);
							} else
								warnText+=Import::tr("Warning: FET expected field separator or text separator in line %1. Import might be incorrect.").arg(lineNumber)+"\n";
						}
						tmpLine.remove(0,1);
					}
					if(!foundField && tmpLine.size()==1){
						if(tmpLine.left(1)==textquote){
							tmpLine.remove(0,1);
						} else {
							warnText+=Import::tr("Warning: FET expected closing text separator in line %1. Import might be incorrect.").arg(lineNumber)+"\n";
							tmp+=tmpLine.left(1);
							tmpLine.remove(0,1);
						}
						
					}
				} else {
					while(!foundField && !tmpLine.isEmpty()){
						if(tmpLine.left(1)!=fieldSeparator)
							tmp+=tmpLine.left(1);
						else
							foundField=true;
						tmpLine.remove(0,1);
					}
				}
				fields << tmp;
				if(foundField && tmpLine.isEmpty())
					fields << "";
			}
/*
			if(separator.size()==1){
				fields = line.split(separator);
			} else
				fields << line;
*/
			if(numberOfFields!=fields.size()){
				warnText+=Import::tr("Skipped line %1: FET expected %2 fields but found %3 fields.").arg(lineNumber).arg(numberOfFields).arg(fields.size())+"\n";
				ok=false;
			} else {
				for(int i=0; i<NUMBER_OF_FIELDS; i++){
					if(fieldNumber[i]>=0){
						itemOfField[i].clear();
						itemOfField[i] = fields[fieldNumber[i]];
						if(itemOfField[i].isEmpty()){
							if(i==FIELD_YEAR_NAME || i==FIELD_TEACHER_NAME || i==FIELD_SUBJECT_NAME){
								warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(lineNumber).arg(fieldName[i])+"\n";
								ok=false;
							}
							if(i==FIELD_YEAR_NUMBER_OF_STUDENTS || i==FIELD_GROUP_NUMBER_OF_STUDENTS || i==FIELD_SUBGROUP_NUMBER_OF_STUDENTS){
								itemOfField[i]="0";
							}
							//if(i==FIELD_SUBGROUP_NAME) is OK
							//if(i==FIELD_ACTIVITY_TAG_NAME) is OK
							//if(i==FIELD_ROOM_NAME) is OK
							if(i==FIELD_ROOM_CAPACITY){
								itemOfField[i]=fieldDefaultItem[i];
							}
							if(i==FIELD_MIN_DAYS){
								itemOfField[i]="0";
							}
							if(i==FIELD_MIN_DAYS_WEIGHT){
								if(itemOfField[FIELD_MIN_DAYS].isEmpty()){
									ok=false;
									warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(lineNumber).arg(fieldName[FIELD_MIN_DAYS])+"\n";
								} else 
									itemOfField[i]="95";
							}
							if(i==FIELD_MIN_DAYS_CONSECUTIVE){
								if(itemOfField[FIELD_MIN_DAYS].isEmpty()){
									ok=false;
									warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(lineNumber).arg(fieldName[FIELD_MIN_DAYS])+"\n";
								} else 
									itemOfField[i]="N";
							}
						}
						if(ok && i==FIELD_SUBGROUP_NAME && !itemOfField[FIELD_SUBGROUP_NAME].isEmpty() && itemOfField[FIELD_GROUP_NAME].isEmpty()){
							warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(lineNumber).arg(fieldName[FIELD_GROUP_NAME])+"\n";
							ok=false;
						}
						if(ok && i==FIELD_SPLIT_DURATION){
							if(itemOfField[FIELD_SPLIT_DURATION].isEmpty()){
								if(!itemOfField[FIELD_TOTAL_DURATION].isEmpty()){
									int totalInt=itemOfField[FIELD_TOTAL_DURATION].toInt(&ok, 10);
									if(ok && totalInt>=1){
										if(totalInt<=MAX_SPLIT_OF_AN_ACTIVITY){
											QString tmpString;
											for(int n=0; n<totalInt; n++){
												if(n!=0)
													tmpString+="+";
												tmpString+="1";
											}
											itemOfField[FIELD_SPLIT_DURATION]=tmpString;
										} else {
											warnText+=Import::tr("Skipped line %1: Field '%2' produces too many subactivities.").arg(lineNumber).arg(fieldName[FIELD_TOTAL_DURATION])+"\n";
											ok=false;
										}
									} else {
										warnText+=Import::tr("Skipped line %1: Field '%2' contain incorrect data.").arg(lineNumber).arg(fieldName[FIELD_TOTAL_DURATION])+"\n";
										ok=false;
									}
								} else {
									warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(lineNumber).arg(fieldName[i])+"\n";
									ok=false;
								}
							} else {
								QStringList splitList;
								if(itemOfField[FIELD_SPLIT_DURATION].count("+")<MAX_SPLIT_OF_AN_ACTIVITY){
									splitList = itemOfField[FIELD_SPLIT_DURATION].split("+", QString::SkipEmptyParts);
									int tmpInt=0;
									QString split;
									while(ok && !splitList.isEmpty()){
										split=splitList.takeFirst();
										tmpInt+=split.toInt(&ok, 10);
										if(!ok)
											warnText+=Import::tr("Skipped line %1: Field '%2' doesn't contain an integer value.").arg(lineNumber).arg(fieldName[FIELD_SPLIT_DURATION])+"\n";
									}
									if(itemOfField[FIELD_TOTAL_DURATION].isEmpty()){
										itemOfField[FIELD_TOTAL_DURATION]=CustomFETString::number(tmpInt);
									} else {
										int totalInt=itemOfField[FIELD_TOTAL_DURATION].toInt(&ok, 10);
										if(totalInt!=tmpInt){
											warnText+=Import::tr("Skipped line %1: Fields '%2' and '%3' haven't the same value.").arg(lineNumber).arg(fieldName[i]).arg(fieldName[FIELD_TOTAL_DURATION])+"\n";
											ok=false;
										}
									}
								} else {
									warnText+=Import::tr("Skipped line %1: Field '%2' contains too many subactivities.").arg(lineNumber).arg(fieldName[i])+"\n";
									ok=false;
								}
							}
						}
						if(ok && !itemOfField[FIELD_BUILDING_NAME].isEmpty() && itemOfField[FIELD_ROOM_NAME].isEmpty() && i==FIELD_ROOM_NAME){
							warnText+=Import::tr("Warning in line %1: Field with building name doesn't affect to a room").arg(lineNumber)+"\n";
						}
						if(ok && (i==FIELD_YEAR_NUMBER_OF_STUDENTS || i==FIELD_GROUP_NUMBER_OF_STUDENTS || i==FIELD_SUBGROUP_NUMBER_OF_STUDENTS || i==FIELD_ROOM_CAPACITY || i==FIELD_TOTAL_DURATION || i==FIELD_MIN_DAYS)){
							if(!itemOfField[i].isEmpty()){
								int value=itemOfField[i].toInt(&ok, 10);
								if(!ok)
									warnText+=Import::tr("Skipped line %1: Field '%2' doesn't contain an integer value.").arg(lineNumber).arg(fieldName[i])+"\n";
								else {
									if(value<0){
										warnText+=Import::tr("Skipped line %1: Field '%2' contains an invalid integer value.").arg(lineNumber).arg(fieldName[i])+"\n";
										ok=false;
									}
								}
							} else if(i==FIELD_TOTAL_DURATION){
								 assert(true);
							}else{
								ok=false;
								warnText+=Import::tr("Skipped line %1: Field '%2' doesn't contain an integer value.").arg(lineNumber).arg(fieldName[i])+"\n";
								//because of bug reported by murad on 25 May 2010, crash when importing rooms, if capacity is empty
								//assert(false);
							}
						}
						if(ok && i==FIELD_MIN_DAYS_WEIGHT){
//							double weight=itemOfField[i].toDouble(&ok);
							double weight=customFETStrToDouble(itemOfField[i], &ok);
							if(!ok)
								warnText+=Import::tr("Skipped line %1: Field '%2' doesn't contain a number (double) value.").arg(lineNumber).arg(fieldName[i])+"\n";
							else {
								if(weight<0.0 || weight>100.0){
									warnText+=Import::tr("Skipped line %1: Field '%2' contains an number (double) value.").arg(lineNumber).arg(fieldName[i])+"\n";
									ok=false;
								}
							}
						}
						if(ok && i==FIELD_MIN_DAYS_CONSECUTIVE){
							QString tmpString;
							tmpString=itemOfField[i];
							tmpString=tmpString.toUpper();
							if(tmpString=="Y" || tmpString=="YES" ||  tmpString=="T" || tmpString=="TRUE" || tmpString=="1")
								itemOfField[i]="yes";
							else if(tmpString=="N" || tmpString=="NO" || tmpString=="F" || tmpString=="FALSE" || tmpString=="0")
								itemOfField[i]="no";
							else
								ok=false;
							if(!ok)
								warnText+=Import::tr("Skipped line %1: Field '%2' contain an unknown value.").arg(lineNumber).arg(fieldName[i])+"\n";
						}
					} else if(fieldNumber[i]==IMPORT_DEFAULT_ITEM){
						itemOfField[i].clear();
						itemOfField[i] = fieldDefaultItem[i];
						//Removed by Liviu - we may have empty default fields
						//assert(!fieldDefaultItem[i].isEmpty());
					}
				}
			}
			if(ok){
				check.clear();
				for(int i=0; i<NUMBER_OF_FIELDS; i++){
					check+=itemOfField[i]+" ";
				}
				if(checkSet.contains(check)){
					if(fieldNumber[FIELD_SPLIT_DURATION]!=DO_NOT_IMPORT||fieldNumber[FIELD_TOTAL_DURATION]!=DO_NOT_IMPORT){
						warnText+=Import::tr("Note about line %1: Data was already in a previous line. So this data will be imported once again.").arg(lineNumber)+"\n";
					} else {
						warnText+=Import::tr("Skipped line %1: Data was already in a previous line.").arg(lineNumber)+"\n";
						ok=false;
					}
				} else
					checkSet<<check;
			}
			if(ok){
				//QString tmp;
				//tmp=tr("%1").arg(lineNumber);
				//itemOfField[FIELD_LINE_NUMBER]=tmp;
				itemOfField[FIELD_LINE_NUMBER]=CustomFETString::number(lineNumber);
				for(int i=0; i<NUMBER_OF_FIELDS; i++){
					if(fieldNumber[i]!=DO_NOT_IMPORT)
						fieldList[i]<<itemOfField[i];
				}
			} else 
				warnText+="   "+Import::tr("Line %1 is: %2").arg(lineNumber).arg(line)+"\n";
		}
	}
	progress.setValue(size);
	//cout<<"progress in readFields ends"<<endl;
	int max=0;
	for(int i=0; i<NUMBER_OF_FIELDS; i++){
		if(max==0)
			max=fieldList[i].size();
		if(fieldNumber[i]>DO_NOT_IMPORT){
			assert(fieldList[i].size()==max);
		}
		else
			assert(fieldList[i].isEmpty());
	}
	file.close();
	return true;
}


int Import::showFieldsAndWarnings(QWidget* parent, QDialog* &newParent){
	newParent=((QDialog*)parent);

	int ok=true;

	int max=0;
	for(int i=0; i<NUMBER_OF_FIELDS; i++){
		if(fieldNumber[i]>DO_NOT_IMPORT){
			if(max==0)
				max=fieldList[i].size();
			assert(fieldList[i].size()==max);
		}
		else{
			if(i!=FIELD_TEACHER_NAME){		//needed for activities!
				//assert(fieldList[i].isEmpty());
				//because of bug reported 17.03.2008. Please add again?! compare add activities function
			}
		}
	}
	// Start Dialog
	newParent=new QDialog(parent);
	QDialog& addItemsDialog=(*newParent);
	addItemsDialog.setWindowTitle(Import::tr("FET import %1 question").arg(importThing));
	QVBoxLayout* addItemsMainLayout=new QVBoxLayout(&addItemsDialog);

	//Start Warnings
	QHBoxLayout* headWarnings=new QHBoxLayout();
	QLabel* headWarningsText=new QLabel();

	int tmp=fileName.lastIndexOf(FILE_SEP);
	tmp=fileName.size()-tmp-1;
	QString shortFileName=fileName.right(tmp);
	if(!warnText.isEmpty())
		headWarningsText->setText(Import::tr("There are several problems in file\n%1").arg(shortFileName));
	else
		headWarningsText->setText(Import::tr("There are no problems in file\n%1").arg(shortFileName));

//TODO
/*
tr("There are no problems in file")
+
"\n"
+
FILE_STRIPPED_NAME
*/

	headWarnings->addWidget(headWarningsText);
	headWarnings->addStretch();

	QPlainTextEdit* textOfWarnings=new QPlainTextEdit();
	textOfWarnings->setMinimumWidth(500);			//width
	textOfWarnings->setReadOnly(true);
	textOfWarnings->setWordWrapMode(QTextOption::NoWrap);
	textOfWarnings->setPlainText(warnText);

	//Start data table
	QLabel* headTableText=new QLabel();
	if(max!=0)
		headTableText->setText(Import::tr("Following data found in the file:"));
	else
		headTableText->setText(Import::tr("There is no usable data in the file."));

	QTableWidget* fieldsTable= new QTableWidget;
	
	//fieldsTable->setUpdatesEnabled(false);
	
	fieldsTable->setRowCount(max);
	QStringList fieldsTabelLabel;

	int columns=0;
	for(int i=0; i<NUMBER_OF_FIELDS; i++){
		if(fieldNumber[i]>DO_NOT_IMPORT){
			fieldsTabelLabel<<tr("%1").arg(fieldName[i]);
			columns++;
		}
	}
	fieldsTable->setColumnCount(columns);
	fieldsTable->setHorizontalHeaderLabels(fieldsTabelLabel);
	for(int i=0; i<max; i++){
		int column=0;
		for(int f=0; f<NUMBER_OF_FIELDS; f++){
			if(fieldNumber[f]>DO_NOT_IMPORT){
				QTableWidgetItem* newItem=new QTableWidgetItem(fieldList[f][i]);
				newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				fieldsTable->setItem(i, column, newItem);
				column++;
			}
		}
	}
	fieldsTable->resizeColumnsToContents();
	//fieldsTable->resizeRowsToContents(); This takes too long for many rows, and it is also useless.

	//fieldsTable->setUpdatesEnabled(true);

	//Start current data warning
	QVBoxLayout* dataWarningBox=new QVBoxLayout();
	QLabel* dataWarningText=new QLabel();
	if(dataWarning.size()==1)
		dataWarningText->setText(Import::tr("FET noticed %1 warning with the current data.").arg(dataWarning.size()));
	else
		dataWarningText->setText(Import::tr("FET noticed %1 warnings with the current data.").arg(dataWarning.size()));
	dataWarningBox->addWidget(dataWarningText);

	QListWidget* dataWarningItems=new QListWidget();
	dataWarningItems->addItems(dataWarning);
	if(dataWarning.size()>0)
		dataWarningBox->addWidget(dataWarningItems);
	else
		delete dataWarningItems;

	//Start Buttons
	QPushButton* pb1=new QPushButton(tr("&Import"));
	QPushButton* pb2=new QPushButton(tr("&Cancel"));

	//TODO: why doesn't work this?
	//if((dataWarning.size()>0&&dataWarning.size()==max)||!warnText.isEmpty())
	//	pb2->setDefault(true);
	//else
	//	 pb1->setDefault(true);
	//	 pb1->setFocus();
		// pb1->setAutoDefault(true);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb1);
	hl->addWidget(pb2);

	//Start adding all into main layout
	addItemsMainLayout->addLayout(headWarnings);
	if(!warnText.isEmpty())
		addItemsMainLayout->addWidget(textOfWarnings);
	else
		delete textOfWarnings;
	addItemsMainLayout->addWidget(headTableText);
	if(max!=0)
		addItemsMainLayout->addWidget(fieldsTable);
	else
		delete fieldsTable;
	addItemsMainLayout->addLayout(dataWarningBox);
	addItemsMainLayout->addLayout(hl);

	QObject::connect(pb1, SIGNAL(clicked()), &addItemsDialog, SLOT(accept()));
	QObject::connect(pb2, SIGNAL(clicked()), &addItemsDialog, SLOT(reject()));

	//pb1->setDefault(true);
	
	int w=chooseWidth(addItemsDialog.sizeHint().width());
	int h=chooseHeight(addItemsDialog.sizeHint().height());
	addItemsDialog.resize(w,h);
	
	QString settingsName;
	if(importThing==Import::tr("activity tags"))
		settingsName=QString("ImportActivityTagsShowFieldsAndWarningsDialog");
	else if(importThing==Import::tr("buildings and rooms"))
		settingsName=QString("ImportBuildingsRoomsShowFieldsAndWarningsDialog");
	else if(importThing==Import::tr("teachers"))
		settingsName=QString("ImportTeachersShowFieldsAndWarningsDialog");
	else if(importThing==Import::tr("subjects"))
		settingsName=QString("ImportSubjectsShowFieldsAndWarningsDialog");
	else if(importThing==Import::tr("years, groups and subgroups"))
		settingsName=QString("ImportYearsGroupsSubgroupsShowFieldsAndWarningsDialog");
	else if(importThing==Import::tr("activities"))
		settingsName=QString("ImportActivitiesShowFieldsAndWarningsDialog");
	
	pb1->setDefault(true);
	pb1->setFocus();

	centerWidgetOnScreen(&addItemsDialog);
	restoreFETDialogGeometry(&addItemsDialog, settingsName);

	ok=addItemsDialog.exec();
	saveFETDialogGeometry(&addItemsDialog, settingsName);
	
	return ok;
}

void Import::importCSVActivityTags(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_ACTIVITY_TAG_NAME]=IMPORT_DEFAULT_ITEM;
	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;

	if(fieldNumber[FIELD_ACTIVITY_TAG_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportActivityTagsChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w= chooseWidth(cfd.sizeHint().width());
		int h= chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);
		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);
		
		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
		if(!ok)	return;
	}

	ok = readFields(newParent);
	if(!ok) return;

	//check empty fields (start)
	for(int i=0; i<fieldList[FIELD_ACTIVITY_TAG_NAME].size(); i++){
		if(fieldList[FIELD_ACTIVITY_TAG_NAME][i].isEmpty())
			warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_ACTIVITY_TAG_NAME])+"\n";
	}
	//check empty fields (end)

	//check if already in memory (start)
	QSet<QString> tmpSet=fieldList[FIELD_ACTIVITY_TAG_NAME].toSet();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* a=gt.rules.activityTagsList[i];
		if(tmpSet.contains(a->name))
			dataWarning<<Import::tr("%1 is already in FET data.").arg(a->name);
	}
	//check if already in memory (end)
	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add activity tags (start) - similar to teachersform.cpp by Liviu modified by Volker
	tmpSet.clear();
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		tmpSet.insert(at->name);
	int count=0;
	for(int i=0; i<fieldList[FIELD_ACTIVITY_TAG_NAME].size(); i++){
		if(!fieldList[FIELD_ACTIVITY_TAG_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_ACTIVITY_TAG_NAME][i])){
			ActivityTag* a=new ActivityTag();
			a->name=fieldList[FIELD_ACTIVITY_TAG_NAME][i];
			if(!gt.rules.addActivityTagFast(a)){
				delete a;
				assert(0);
			} else count++;
		}
	}
	QMessageBox::information(newParent, tr("FET information"), Import::tr("%1 activity tags added. Please check activity tags form.").arg(count));
	//add activity tags (end) - similar to teachersform.cpp by Liviu modified by Volker
	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}


void Import::importCSVRoomsAndBuildings(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_ROOM_NAME]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_ROOM_CAPACITY]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_BUILDING_NAME]=IMPORT_DEFAULT_ITEM;
	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;

	if(fieldNumber[FIELD_ROOM_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportRoomsBuildingsChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w= chooseWidth(cfd.sizeHint().width());
		int h= chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);
		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);

		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
	}

	if(!ok)	return;

	ok = readFields(newParent);
	if(!ok) return;

	QStringList duplicatesCheck;
	//check duplicates of rooms in csv
	if(fieldNumber[FIELD_ROOM_NAME]!=DO_NOT_IMPORT)
		for(int i=0; i<fieldList[FIELD_ROOM_NAME].size(); i++){
			if(duplicatesCheck.contains(fieldList[FIELD_ROOM_NAME][i]))
				warnText+=Import::tr("Skipped line %1: Field '%2' is already in a previous line.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_ROOM_NAME])+"\n";
			else
				duplicatesCheck<<fieldList[FIELD_ROOM_NAME][i];
		}
	duplicatesCheck.clear();
	//check duplicates of buildings in csv. only if no room is imported.
	if(fieldNumber[FIELD_ROOM_NAME]==DO_NOT_IMPORT&&fieldNumber[FIELD_BUILDING_NAME]!=DO_NOT_IMPORT)
		for(int i=0; i<fieldList[FIELD_BUILDING_NAME].size(); i++){
			if(duplicatesCheck.contains(fieldList[FIELD_BUILDING_NAME][i]))
				warnText+=Import::tr("Skipped line %1: Field '%2' is already in a previous line.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_BUILDING_NAME])+"\n";
			else
				duplicatesCheck<<fieldList[FIELD_BUILDING_NAME][i];
		}
	duplicatesCheck.clear();
	//check empty rooms (start)
	if(fieldNumber[FIELD_ROOM_NAME!=DO_NOT_IMPORT])
		for(int i=0; i<fieldList[FIELD_ROOM_NAME].size(); i++)
			if(fieldList[FIELD_ROOM_NAME][i].isEmpty())
				warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_ROOM_NAME])+"\n";
	//check empty rooms (end)
	//check empty buildings (start)
	if((fieldNumber[FIELD_ROOM_NAME]==DO_NOT_IMPORT||fieldNumber[FIELD_ROOM_NAME]==IMPORT_DEFAULT_ITEM)&&fieldNumber[FIELD_BUILDING_NAME!=DO_NOT_IMPORT])
		for(int i=0; i<fieldList[FIELD_BUILDING_NAME].size(); i++)
			if(fieldList[FIELD_BUILDING_NAME][i].isEmpty())
				warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_BUILDING_NAME])+"\n";
	//check empty buildings (end)

	//check if rooms are already in memory (start)
	QSet<QString> tmpSet=fieldList[FIELD_ROOM_NAME].toSet();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* r=gt.rules.roomsList[i];
		if(tmpSet.contains(r->name))
			dataWarning<<Import::tr("%1 is already in FET data.").arg(r->name);
	}
	//check if rooms are already in memory (end)

	//check if buildings are already in memory (start)
	if(fieldNumber[FIELD_ROOM_NAME]<0){
		tmpSet=fieldList[FIELD_BUILDING_NAME].toSet();
		for(int i=0; i<gt.rules.buildingsList.size(); i++){
			Building* b=gt.rules.buildingsList[i];
			if(tmpSet.contains(b->name))
				dataWarning<<Import::tr("%1 is already in FET data.").arg(b->name);
		}
	}
	//check if buildings are already in memory (end)

	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add buildings (start) - similar to teachersform.cpp by Liviu modified by Volker
	tmpSet.clear();
	foreach(Building* bu, gt.rules.buildingsList)
		tmpSet.insert(bu->name);
	int count=0;
	for(int i=0; i<fieldList[FIELD_BUILDING_NAME].size(); i++){
		if(!fieldList[FIELD_BUILDING_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_BUILDING_NAME][i])){
			Building* b=new Building();
			b->name=fieldList[FIELD_BUILDING_NAME][i];
			if(!gt.rules.addBuildingFast(b)){
				delete b;
				assert(0);
			} else count++;
		}
	}
	//add buildings (end) - similar to teachersform.cpp by Liviu modified by Volker

	//add rooms (start) - similar to teachersform.cpp by Liviu modified by Volker
	tmpSet.clear();
	foreach(Room* rm, gt.rules.roomsList)
		tmpSet.insert(rm->name);
	int countroom=0;
	for(int i=0; i<fieldList[FIELD_BUILDING_NAME].size(); i++){
		if(!fieldList[FIELD_ROOM_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_ROOM_NAME][i])){
			Room* r=new Room();
			r->name=fieldList[FIELD_ROOM_NAME][i];
			if(fieldNumber[FIELD_BUILDING_NAME]!=DO_NOT_IMPORT)
				r->building=fieldList[FIELD_BUILDING_NAME][i];
			else
				r->building="";
			if(fieldNumber[FIELD_ROOM_CAPACITY]!=DO_NOT_IMPORT){
				QString tmpInt=fieldList[FIELD_ROOM_CAPACITY][i];
				r->capacity=tmpInt.toInt();
			}
			else
				assert(0==1);
			if(!gt.rules.addRoomFast(r)){
				delete r;
				assert(0);
			} else countroom++;
		}
	}
	//add rooms (end) - similar to teachersform.cpp by Liviu modified by Volker
	QMessageBox::information(newParent, tr("FET information"),
	 Import::tr("%1 buildings added. Please check buildings form.").arg(count)+"\n"+tr("%1 rooms added. Please check rooms form.").arg(countroom));

	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}

void Import::importCSVSubjects(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_SUBJECT_NAME]=IMPORT_DEFAULT_ITEM;
	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;

	if(fieldNumber[FIELD_SUBJECT_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportSubjectsChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w= chooseWidth(cfd.sizeHint().width());
		int h= chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);
		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);

		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
	}

	if(!ok)	return;

	ok = readFields(newParent);
	if(!ok) return;

	//check empty fields (start)
	for(int i=0; i<fieldList[FIELD_SUBJECT_NAME].size(); i++){
		if(fieldList[FIELD_SUBJECT_NAME][i].isEmpty())
			warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_SUBJECT_NAME])+"\n";
	}
	//check empty fields (end)

	//check if already in memory (start)
	QSet<QString> tmpSet=fieldList[FIELD_SUBJECT_NAME].toSet();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		if(tmpSet.contains(s->name))
			dataWarning<<Import::tr("%1 is already in FET data.").arg(s->name);
	}
	//check if already in memory (end)

	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add subjects (start) - similar to teachersform.cpp by Liviu modified by Volker
	tmpSet.clear();
	foreach(Subject* sbj, gt.rules.subjectsList)
		tmpSet.insert(sbj->name);
	int count=0;
	for(int i=0; i<fieldList[FIELD_SUBJECT_NAME].size(); i++){
		if(!fieldList[FIELD_SUBJECT_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_SUBJECT_NAME][i])){
			Subject* s=new Subject();
			s->name=fieldList[FIELD_SUBJECT_NAME][i];
			if(!gt.rules.addSubjectFast(s)){
				delete s;
				assert(0);
			} else count++;
		}
	}
	//add subjects (end) - similar to teachersform.cpp by Liviu modified by Volker
	QMessageBox::information(newParent, tr("FET information"), Import::tr("%1 subjects added. Please check subjects form.").arg(count));
	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}


void Import::importCSVTeachers(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_TEACHER_NAME]=IMPORT_DEFAULT_ITEM;
	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;

	if(fieldNumber[FIELD_TEACHER_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportTeachersChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w= chooseWidth(cfd.sizeHint().width());
		int h= chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);
		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);

		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
	}

	if(!ok)	return;

	ok = readFields(newParent);
	if(!ok) return;

	//check empty fields (start)
	for(int i=0; i<fieldList[FIELD_TEACHER_NAME].size(); i++){
		if(fieldList[FIELD_TEACHER_NAME][i].isEmpty())
			warnText+=Import::tr("Skipped line %1: Field '%2' is empty.").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(fieldName[FIELD_TEACHER_NAME])+"\n";
	}
	//check empty fields (end)

	//check if already in memory (start)
	QSet<QString> tmpSet=fieldList[FIELD_TEACHER_NAME].toSet();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		if(tmpSet.contains(t->name))
			dataWarning<<Import::tr("%1 is already in FET data.").arg(t->name);
	}
	//check if already in memory (end)

	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add teachers (start) - similar to teachersform.cpp by Liviu modified by Volker
	tmpSet.clear();
	foreach(Teacher* tch, gt.rules.teachersList)
		tmpSet.insert(tch->name);
	int count=0;
	for(int i=0; i<fieldList[FIELD_TEACHER_NAME].size(); i++){
		if(!fieldList[FIELD_TEACHER_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_TEACHER_NAME][i])){
			Teacher* tch=new Teacher();
			tch->name=fieldList[FIELD_TEACHER_NAME][i];
			if(!gt.rules.addTeacherFast(tch)){
				delete tch;
				assert(0);
			} else count++;
		}
	}
	QMessageBox::information(newParent, tr("FET information"), Import::tr("%1 teachers added. Please check teachers form.").arg(count));
	//add teachers (end) - similar to teachersform.cpp by Liviu modified by Volker
	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}

void Import::importCSVStudents(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_YEAR_NAME]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_YEAR_NUMBER_OF_STUDENTS]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_GROUP_NAME]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_GROUP_NUMBER_OF_STUDENTS]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_SUBGROUP_NAME]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_SUBGROUP_NUMBER_OF_STUDENTS]=IMPORT_DEFAULT_ITEM;
	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;
	
	if(fieldNumber[FIELD_YEAR_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportYearsGroupsSubgroupsChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w=chooseWidth(cfd.sizeHint().width());
		int h=chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);

		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);

		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
		if(!ok)	return;
	}

	ok = readFields(newParent);
	if(!ok) return;

	//check if already in memory (start) - similar to adding items by Liviu modified by Volker
	QString yearName;
	QString groupName;
	QString subgroupName;
	QSet<QString> usedCSVYearNames;			// this is much faster than QStringList
	QSet<QString> usedCSVGroupNames;
	QSet<QString> usedCSVSubgroupNames;

	//check csv
	QProgressDialog* _progress=new QProgressDialog(newParent);
	QProgressDialog& progress=(*_progress);
	progress.setWindowTitle(tr("Importing", "Title of a progress dialog"));
	//cout<<"progress in importCSVStudents starts, range="<<fieldList[FIELD_YEAR_NAME].size()<<endl;
	progress.setLabelText(tr("Checking CSV"));
	progress.setModal(true);
	progress.setRange(0, fieldList[FIELD_YEAR_NAME].size());
	for(int i=0; i<fieldList[FIELD_YEAR_NAME].size(); i++){
		progress.setValue(i);
		if(progress.wasCanceled()){
			progress.setValue(fieldList[FIELD_YEAR_NAME].size());
			QMessageBox::warning(newParent, "FET", Import::tr("Checking CSV canceled by user."));
			return;
		}
		if(fieldNumber[FIELD_YEAR_NAME]>=0)
			yearName=fieldList[FIELD_YEAR_NAME][i];
		else
			yearName=fieldDefaultItem[FIELD_YEAR_NAME];
		if((fieldNumber[FIELD_GROUP_NAME])>=0)
			groupName=fieldList[FIELD_GROUP_NAME][i];
		else
			groupName=fieldDefaultItem[FIELD_GROUP_NAME];
		if((fieldNumber[FIELD_SUBGROUP_NAME])>=0)
			subgroupName=fieldList[FIELD_SUBGROUP_NAME][i];
		else
			subgroupName=fieldDefaultItem[FIELD_SUBGROUP_NAME];
		if((fieldNumber[FIELD_YEAR_NAME])>=IMPORT_DEFAULT_ITEM){
			if(!yearName.isEmpty())
				if(!usedCSVYearNames.contains(yearName))
					usedCSVYearNames<<yearName;
		}
		if((fieldNumber[FIELD_GROUP_NAME])>=IMPORT_DEFAULT_ITEM){
			if(!groupName.isEmpty())
				if(!usedCSVGroupNames.contains(groupName))
					usedCSVGroupNames<<groupName;
			if(usedCSVYearNames.contains(groupName))
				warnText+=Import::tr("Problem in line %1: Group name %2 is taken for a year - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(groupName)+"\n";
			if(usedCSVGroupNames.contains(yearName))
				warnText+=Import::tr("Problem in line %1: Year name %2 is taken for a group - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(yearName)+"\n";

		}
		if((fieldNumber[FIELD_SUBGROUP_NAME])>=IMPORT_DEFAULT_ITEM){
			if(!subgroupName.isEmpty())
				if(!usedCSVSubgroupNames.contains(subgroupName))
					usedCSVSubgroupNames<<subgroupName;
			if(usedCSVYearNames.contains(subgroupName))
				warnText+=Import::tr("Problem in line %1: Subgroup name %2 is taken for a year - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(subgroupName)+"\n";
			if(usedCSVGroupNames.contains(subgroupName))
				warnText+=Import::tr("Problem in line %1: Subgroup name %2 is taken for a group - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(subgroupName)+"\n";
			if(usedCSVSubgroupNames.contains(groupName))
				warnText+=Import::tr("Problem in line %1: Group name %2 is taken for a subgroup - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(groupName)+"\n";
			if(usedCSVSubgroupNames.contains(yearName))
				warnText+=Import::tr("Problem in line %1: Year name %2 is taken for a subgroup - please consider another name").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(yearName)+"\n";
		}
	}
	progress.setValue(fieldList[FIELD_YEAR_NAME].size());
	//cout<<"progress in importCSVStudents ends"<<endl;

	//check current data
	QProgressDialog* _progress2=new QProgressDialog(newParent);
	QProgressDialog& progress2=(*_progress2);
	progress2.setWindowTitle(tr("Importing", "Title of a progress dialog"));
	progress2.setLabelText(tr("Checking data"));
	progress2.setModal(true);
	progress2.setRange(0, fieldList[FIELD_YEAR_NAME].size());
	//cout<<"progress2 in importCSVStudents starts, range="<<fieldList[FIELD_YEAR_NAME].size()<<endl;
	int kk=0;
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		progress2.setValue(kk);
		kk++;
		if(progress2.wasCanceled()){
			progress2.setValue(fieldList[FIELD_YEAR_NAME].size());
			QMessageBox::warning(newParent, "FET", Import::tr("Checking data canceled by user."));
			return;
		}
		StudentsYear* sty=gt.rules.yearsList[i];
		if(usedCSVYearNames.contains(sty->name))
			dataWarning<<Import::tr("Year %1 is already in FET data.").arg(sty->name);
		if(usedCSVGroupNames.contains(sty->name))
			dataWarning<<Import::tr("Can't import group %1. Name is already taken for a year.").arg(sty->name);
		if(usedCSVSubgroupNames.contains(sty->name))
			dataWarning<<Import::tr("Can't import subgroup %1. Name is already taken for a year.").arg(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			progress2.setValue(kk);
			kk++;
			if(progress2.wasCanceled()){
				progress2.setValue(fieldList[FIELD_YEAR_NAME].size());
				QMessageBox::warning(newParent, "FET", Import::tr("Checking data canceled by user."));
				return;
			}

			StudentsGroup* stg=sty->groupsList[j];
			if(usedCSVYearNames.contains(stg->name))
				dataWarning<<Import::tr("Can't import year %1. Name is already taken for a group.").arg(stg->name);
			if(usedCSVGroupNames.contains(stg->name))
				dataWarning<<Import::tr("Group name %1 is already in FET data (in the same or in another year).").arg(stg->name);
			if(usedCSVSubgroupNames.contains(stg->name))
				dataWarning<<Import::tr("Can't import subgroup %1. Name is already taken for a group.").arg(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				progress2.setValue(kk);
				kk++;

				if(progress2.wasCanceled()){
					progress2.setValue(fieldList[FIELD_YEAR_NAME].size());
					QMessageBox::warning(newParent, "FET", Import::tr("Checking data canceled by user."));
					return;
				}

				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(usedCSVYearNames.contains(sts->name))
					dataWarning<<Import::tr("Can't import year %1. Name is already taken for a subgroup.").arg(sts->name);
				if(usedCSVGroupNames.contains(sts->name))
					dataWarning<<Import::tr("Can't import group %1. Name is taken for a subgroup.").arg(sts->name);
				if(usedCSVSubgroupNames.contains(sts->name))
					dataWarning<<Import::tr("Subgroup name %1 is already in FET data (in the same or in another group).").arg(sts->name);
			}
		}
	}
	progress2.setValue(fieldList[FIELD_YEAR_NAME].size());
	//cout<<"progress2 in importCSVStudents ends"<<endl;

	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add students (start) - similar to adding items by Liviu modified by Volker
	lastWarning.clear();
	int addedYears=0;
	int addedGroups=0;
	int addedSubgroups=0;
	QProgressDialog* _progress3=new QProgressDialog(newParent);
	QProgressDialog& progress3=(*_progress3);
	progress3.setWindowTitle(tr("Importing", "Title of a progress dialog"));
	progress3.setLabelText(tr("Importing data"));
	progress3.setModal(true);
	progress3.setRange(0, fieldList[FIELD_YEAR_NAME].size());
	//cout<<"progress3 in importCSVStudents starts, range="<<fieldList[FIELD_YEAR_NAME].size()<<endl;
	
	QHash<QString, StudentsSet*> studentsHash;
	QSet<QPair<QString, QString> > groupsInYearSet; //first year, then group
	QSet<QPair<QString, QString> > subgroupsInGroupSet; //first group, then subgroup
	foreach(StudentsYear* year, gt.rules.yearsList){
		studentsHash.insert(year->name, year);
		foreach(StudentsGroup* group, year->groupsList){
			studentsHash.insert(group->name, group);
			groupsInYearSet.insert(QPair<QString, QString> (year->name, group->name));
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				studentsHash.insert(subgroup->name, subgroup);
				subgroupsInGroupSet.insert(QPair<QString, QString> (group->name, subgroup->name));
			}
		}
	}
	
	for(int i=0; i<fieldList[FIELD_YEAR_NAME].size(); i++){
		progress3.setValue(i);
		if(progress3.wasCanceled()){
			progress3.setValue(fieldList[FIELD_YEAR_NAME].size());
			QMessageBox::warning(newParent, "FET", Import::tr("Importing data canceled by user."));
			//return false;
			ok=false;
			goto ifUserCanceledProgress3;
		}
		ok=true;
		bool tryNext=false;
		if(fieldNumber[FIELD_YEAR_NAME]!=IMPORT_DEFAULT_ITEM)
			yearName=fieldList[FIELD_YEAR_NAME][i];
		else
			yearName=fieldDefaultItem[FIELD_YEAR_NAME];
		assert(!yearName.isEmpty());
		//StudentsSet* ss=gt.rules.searchStudentsSet(yearName);
		StudentsSet* ss=studentsHash.value(yearName, NULL);
		if(ss!=NULL){
			if(ss->type==STUDENTS_SUBGROUP)
				ok=false;
			else if(ss->type==STUDENTS_GROUP)
				ok=false;
			else if(ss->type==STUDENTS_YEAR){
				ok=false;
				tryNext=true;
			}
			else
				assert(0);
		}
		if(ok){
			StudentsYear* sy=new StudentsYear();
			sy->name=yearName;
			QString tmpString=fieldList[FIELD_YEAR_NUMBER_OF_STUDENTS][i];
			sy->numberOfStudents=tmpString.toInt();
			assert(!fieldList[FIELD_YEAR_NUMBER_OF_STUDENTS].isEmpty());
			/*if(gt.rules.searchYear(yearName) >=0 )
				delete sy;
			else {
				bool tmp=gt.rules.addYear(sy);
				assert(tmp);
				addedYears++;
			}*/
			StudentsSet* studentsSet=ss; //studentsHash.value(yearName, NULL);
			bool yearExists=false;
			if(studentsSet!=NULL){
				assert(0);
				if(studentsSet->type==STUDENTS_YEAR)
					yearExists=true;
			}
			if(yearExists){
				delete sy;
				assert(0);
			}
			else{
				bool tmp=gt.rules.addYearFast(sy);
				assert(tmp);
				addedYears++;
				studentsHash.insert(sy->name, sy);
			}
		}
		if((tryNext || ok) && fieldNumber[FIELD_GROUP_NAME]!=DO_NOT_IMPORT){
			ok=true;
			tryNext=false;
			StudentsGroup* sg;
			sg=NULL;
			if(fieldNumber[FIELD_GROUP_NAME]!=IMPORT_DEFAULT_ITEM)
				groupName=fieldList[FIELD_GROUP_NAME][i];
			else
				groupName=fieldDefaultItem[FIELD_GROUP_NAME];
			if(groupName.isEmpty())
				ok=false;
			else {
				//if(ok && gt.rules.searchGroup(yearName, groupName)>=0){
				if(ok && groupsInYearSet.contains(QPair<QString, QString> (yearName, groupName))){
					ok=false;
					tryNext=true;
				}
				//StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
				StudentsSet* ss=studentsHash.value(groupName, NULL);
				if(ss!=NULL && ss->type==STUDENTS_YEAR)
					ok=false;
				else if(ss!=NULL && ss->type==STUDENTS_SUBGROUP)
					ok=false;
				else if(ss!=NULL && ss->type==STUDENTS_GROUP){
					if(fieldNumber[FIELD_SUBGROUP_NAME]==DO_NOT_IMPORT)
						lastWarning+=Import::tr("Group name %1 exists in another year. It means that some years share the same group.").arg(groupName)+"\n";
					if(fieldNumber[FIELD_SUBGROUP_NAME]!=DO_NOT_IMPORT)
						if(fieldList[FIELD_SUBGROUP_NAME].isEmpty())
							lastWarning+=Import::tr("Group name %1 exists in another year. It means that some years share the same group.").arg(groupName)+"\n";
				}
				if(ss!=NULL&&ok){
					sg=(StudentsGroup*)ss;
				}
				else if(ss==NULL&&ok){
					sg=new StudentsGroup();
					sg->name=groupName;
					QString tmpString=fieldList[FIELD_GROUP_NUMBER_OF_STUDENTS][i];
					sg->numberOfStudents=tmpString.toInt();
					assert(ok);
					assert(!fieldList[FIELD_GROUP_NUMBER_OF_STUDENTS].isEmpty());

					studentsHash.insert(sg->name, sg);
				}
				if(ok){
					StudentsSet* tmpStudentsSet=studentsHash.value(yearName, NULL);
					assert(tmpStudentsSet->type==STUDENTS_YEAR);
					
					StudentsYear* year=(StudentsYear*)tmpStudentsSet;
					assert(year!=NULL);
					gt.rules.addGroupFast(year, sg);

					groupsInYearSet.insert(QPair<QString, QString> (yearName, sg->name));

					addedGroups++;
				}
			}
		}
		if((tryNext || ok) && fieldNumber[FIELD_SUBGROUP_NAME]!=DO_NOT_IMPORT){
			ok=true;
			if(fieldNumber[FIELD_SUBGROUP_NAME]!=IMPORT_DEFAULT_ITEM)
				subgroupName=fieldList[FIELD_SUBGROUP_NAME][i];
			else
				subgroupName=fieldDefaultItem[FIELD_SUBGROUP_NAME];
			if(subgroupName.isEmpty())
				ok=false;
			else {
				//if(ok && gt.rules.searchSubgroup(yearName, groupName, subgroupName)>=0){
				if(ok && subgroupsInGroupSet.contains(QPair<QString, QString> (groupName, subgroupName))){
					ok=false;
				}
				//StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
				StudentsSet* ss=studentsHash.value(subgroupName, NULL);
				StudentsSubgroup* sts;
				sts=NULL;
				if(ss!=NULL && ss->type==STUDENTS_YEAR){
					ok=false;
				}
				else if(ss!=NULL && ss->type==STUDENTS_GROUP){
					ok=false;
				}
				else if(ss!=NULL && ss->type==STUDENTS_SUBGROUP){
					lastWarning+=Import::tr("Subgroup name %1 exists in another group. It means that some groups share the same subgroup.").arg(subgroupName)+"\n";
				}
				if(ss!=NULL&&ok){
					sts=(StudentsSubgroup*)ss;
				}
				else if(ss==NULL&&ok) {
					sts=new StudentsSubgroup();
					sts->name=subgroupName;
					QString tmpString=fieldList[FIELD_SUBGROUP_NUMBER_OF_STUDENTS][i];
					sts->numberOfStudents=tmpString.toInt();
					assert(ok);
					assert(!fieldList[FIELD_SUBGROUP_NUMBER_OF_STUDENTS].isEmpty());

					studentsHash.insert(sts->name, sts);
				}
				if(ok){
					StudentsSet* tmpStudentsSet=studentsHash.value(yearName, NULL);
					assert(tmpStudentsSet->type==STUDENTS_YEAR);
				
					StudentsYear* year=(StudentsYear*)tmpStudentsSet;
					assert(year!=NULL);
					
					tmpStudentsSet=studentsHash.value(groupName, NULL);
					assert(tmpStudentsSet->type==STUDENTS_GROUP);
					
					StudentsGroup* group=(StudentsGroup*)tmpStudentsSet;
					assert(group!=NULL);

					gt.rules.addSubgroupFast(year, group, sts);

					subgroupsInGroupSet.insert(QPair<QString, QString> (groupName, sts->name));

					addedSubgroups++;
				}
			}
		}
	}

	progress3.setValue(fieldList[FIELD_YEAR_NAME].size());
	//cout<<"progress3 in importCSVStudents ends"<<endl;
	//add students (end) - similar to adding items by Liviu modified by Volker
	
ifUserCanceledProgress3:

	if(!lastWarning.isEmpty())
		lastWarning.insert(0,"\n"+Import::tr("Notes:")+"\n");
	lastWarning.insert(0,Import::tr("%1 subgroups added. Please check subgroups form.").arg(addedSubgroups)+"\n");
	lastWarning.insert(0,Import::tr("%1 groups added. Please check groups form.").arg(addedGroups)+"\n");
	lastWarning.insert(0,Import::tr("%1 years added. Please check years form.").arg(addedYears)+"\n");
	
	gt.rules.computePermanentStudentsHash();

	LastWarningsDialog lwd(newParent);
	int w=chooseWidth(lwd.sizeHint().width());
	int h=chooseHeight(lwd.sizeHint().height());
	lwd.resize(w,h);
	centerWidgetOnScreen(&lwd);

	ok=lwd.exec();

	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}

void Import::importCSVActivities(QWidget* parent){
	prearrangement();
	fieldNumber[FIELD_STUDENTS_SET]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_SUBJECT_NAME]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_TEACHERS_SET]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_ACTIVITY_TAGS_SET]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_TOTAL_DURATION]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_SPLIT_DURATION]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_MIN_DAYS]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_MIN_DAYS_WEIGHT]=IMPORT_DEFAULT_ITEM;
	fieldNumber[FIELD_MIN_DAYS_CONSECUTIVE]=IMPORT_DEFAULT_ITEM;

	int ok;

	QDialog* newParent;
	ok = getFileSeparatorFieldsAndHead(parent, newParent);
	//DON'T ADD THIS! newParent->deleteLater();
	if(!ok)	return;

	if(fieldNumber[FIELD_SUBJECT_NAME]==IMPORT_DEFAULT_ITEM){
		QDialog* newParent2=new ChooseFieldsDialog(newParent);
		const QString settingsName=QString("ImportActivitiesChooseFieldsDialog");
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		ChooseFieldsDialog& cfd=(*((ChooseFieldsDialog*)newParent));
		int w=chooseWidth(cfd.sizeHint().width());
		int h=chooseHeight(cfd.sizeHint().height());
		cfd.resize(w,h);
		centerWidgetOnScreen(&cfd);
		restoreFETDialogGeometry(&cfd, settingsName);

		ok=cfd.exec();
		saveFETDialogGeometry(&cfd, settingsName);
	}
	if(!ok)	return;

	if(fieldNumber[FIELD_SPLIT_DURATION]==DO_NOT_IMPORT&&fieldNumber[FIELD_TOTAL_DURATION]==DO_NOT_IMPORT){
		QMessageBox::warning(newParent, tr("FET warning"), Import::tr("FET needs to know %1 or %2 if you import %3.").arg(fieldName[FIELD_SPLIT_DURATION]).arg(fieldName[FIELD_TOTAL_DURATION]).arg(importThing));
		return;
	}

	ok = readFields(newParent);
	if(!ok) return;

	//check number of fields (start) //because of bug reported 17.03.2008
	int checkNumber=0;
	if(fieldList[FIELD_STUDENTS_SET].size()>0){
		checkNumber=fieldList[FIELD_STUDENTS_SET].size();
	}
	if(fieldList[FIELD_SUBJECT_NAME].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_SUBJECT_NAME].size());
		}
		checkNumber=fieldList[FIELD_SUBJECT_NAME].size();
	}
	if(fieldList[FIELD_TEACHERS_SET].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_TEACHERS_SET].size());
		}
		checkNumber=fieldList[FIELD_TEACHERS_SET].size();
	}
	if(fieldList[FIELD_ACTIVITY_TAGS_SET].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_ACTIVITY_TAGS_SET].size());
		}
		checkNumber=fieldList[FIELD_ACTIVITY_TAGS_SET].size();
	}
	if(fieldList[FIELD_TOTAL_DURATION].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_TOTAL_DURATION].size());
		}
		checkNumber=fieldList[FIELD_TOTAL_DURATION].size();
	}
	if(fieldList[FIELD_SPLIT_DURATION].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_SPLIT_DURATION].size());
		}
		checkNumber=fieldList[FIELD_SPLIT_DURATION].size();
	}
	if(fieldList[FIELD_MIN_DAYS].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_MIN_DAYS].size());
		}
		checkNumber=fieldList[FIELD_MIN_DAYS].size();
	}
	if(fieldList[FIELD_MIN_DAYS_WEIGHT].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_MIN_DAYS_WEIGHT].size());
		}
		checkNumber=fieldList[FIELD_MIN_DAYS_WEIGHT].size();
	}
	if(fieldList[FIELD_MIN_DAYS_CONSECUTIVE].size()>0){
		if(checkNumber>0){
			assert(checkNumber==fieldList[FIELD_MIN_DAYS_CONSECUTIVE].size());
		}
		checkNumber=fieldList[FIELD_MIN_DAYS_CONSECUTIVE].size();
	}

	if(fieldList[FIELD_STUDENTS_SET].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_STUDENTS_SET]<<"";
	}
	if(fieldList[FIELD_SUBJECT_NAME].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_SUBJECT_NAME]<<"";
	}
	if(fieldList[FIELD_TEACHERS_SET].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_TEACHERS_SET]<<"";
	}
	if(fieldList[FIELD_ACTIVITY_TAGS_SET].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_ACTIVITY_TAGS_SET]<<"";
	}
	if(fieldList[FIELD_TOTAL_DURATION].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_TOTAL_DURATION]<<"1";
	}
	if(fieldList[FIELD_SPLIT_DURATION].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_SPLIT_DURATION]<<"1";
	}
	if(fieldList[FIELD_MIN_DAYS].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_MIN_DAYS]<<"1";
	}
	if(fieldList[FIELD_MIN_DAYS_WEIGHT].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_MIN_DAYS_WEIGHT]<<"95";
	}
	if(fieldList[FIELD_MIN_DAYS_CONSECUTIVE].size()==0){
		for(int i=0; i<checkNumber; i++)
			fieldList[FIELD_MIN_DAYS_CONSECUTIVE]<<"no";
	}

	//check number of fields (end) //because of bug reported 17.03.2008

	//check if already in memory (start)
	//check if students set is in memory
	/*QHash<QString, StudentsSet*> studentsHash;
	foreach(StudentsYear* year, gt.rules.yearsList){
		studentsHash.insert(year->name, year);
		foreach(StudentsGroup* group, year->groupsList){
			studentsHash.insert(group->name, group);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				studentsHash.insert(subgroup->name, subgroup);
			}
		}
	}*/
	const QHash<QString, StudentsSet*>& studentsHash=gt.rules.permanentStudentsHash;
	
	lastWarning.clear();
	QString line;
	QStringList students;
	bool firstWarning=true;
	for(int i=0; i<fieldList[FIELD_STUDENTS_SET].size(); i++){
		line.clear();
		line=fieldList[FIELD_STUDENTS_SET][i];
		students.clear();
		students=line.split("+", QString::SkipEmptyParts);
		if(!fieldList[FIELD_STUDENTS_SET][i].isEmpty()){
			for(int s=0; s<students.size(); s++){
				//StudentsSet* ss=gt.rules.searchStudentsSet(students[s]);
				StudentsSet* ss=studentsHash.value(students[s], NULL);
				if(ss==NULL){
					if(firstWarning){
						lastWarning+=Import::tr("FET can't import activities, because FET needs to know the stucture of the "
						"students sets. You must add (or import) years, groups and subgroups first.")+"\n"+
						tr("It is recommended to import also teachers, rooms, buildings, subjects and activity tags before "
						"importing activities. It is not needed, because FET will automatically do it, but you can "
						"check the activity csv file by that.")+"\n";
						firstWarning=false;
					}
					lastWarning+=Import::tr("Students set %1 doesn't exist. You must add (or import) years, groups and subgroups first.").arg(students[s])+"\n";
				}
			}
		}
	}
	if(lastWarning.size()>0){
		QDialog* newParent2=new LastWarningsDialog(newParent);
		//DON'T ADD THIS! newParent2->deleteLater();
		newParent=newParent2;
		LastWarningsDialog& lwd=(*((LastWarningsDialog*)newParent));
		int w=chooseWidth(lwd.sizeHint().width());
		int h=chooseHeight(lwd.sizeHint().height());
		lwd.resize(w,h);
		centerWidgetOnScreen(&lwd);

		ok=lwd.exec();
		return;
	}
	//check if teacher is in memory
	assert(fieldList[FIELD_TEACHER_NAME].isEmpty());
	QStringList teachers;
	QSet<QString> tmpSet;
	tmpSet.clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		tmpSet.insert(t->name);
	}
	for(int i=0; i<fieldList[FIELD_TEACHERS_SET].size(); i++){
		line.clear();
		line=fieldList[FIELD_TEACHERS_SET][i];
		teachers.clear();
		teachers=line.split("+", QString::SkipEmptyParts);
		for(int t=0; t<teachers.size(); t++){
			bool add=true;
			if(tmpSet.contains(teachers[t]) || teachers[t]=="")
				add=false;
			if(add){
				dataWarning<<Import::tr("%1 %2 will be added.", "For instance 'Subject Math will be added', so use singular").arg(fieldName[FIELD_TEACHER_NAME]).arg(teachers[t]);
				tmpSet.insert(teachers[t]);
				fieldList[FIELD_TEACHER_NAME]<<teachers[t];
			}
		}
	}
	//check if subject is in memory
	tmpSet.clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		tmpSet.insert(s->name);
	}
	for(int sn=0; sn<fieldList[FIELD_SUBJECT_NAME].size(); sn++){
		bool add=true;
		if(tmpSet.contains(fieldList[FIELD_SUBJECT_NAME][sn]) || fieldList[FIELD_SUBJECT_NAME][sn]=="")
			add=false;
		if(add){
			dataWarning<<Import::tr("%1 %2 will be added.", "For instance 'Subject Math will be added', so use singular").arg(fieldName[FIELD_SUBJECT_NAME]).arg(fieldList[FIELD_SUBJECT_NAME][sn]);
			tmpSet.insert(fieldList[FIELD_SUBJECT_NAME][sn]);
		}
	}
	//check if activity tag is in memory
	assert(fieldList[FIELD_ACTIVITY_TAG_NAME].isEmpty());
	QStringList activityTags;
	tmpSet.clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		tmpSet.insert(at->name);
	}
	for(int i=0; i<fieldList[FIELD_ACTIVITY_TAGS_SET].size(); i++){
		line.clear();
		line=fieldList[FIELD_ACTIVITY_TAGS_SET][i];
		activityTags.clear();
		activityTags=line.split("+", QString::SkipEmptyParts);
		for(int at=0; at<activityTags.size(); at++){
			bool add=true;
			if(tmpSet.contains(activityTags[at]) || activityTags[at]=="")
				add=false;
			if(add){
				dataWarning<<Import::tr("%1 %2 will be added.", "For instance 'Subject Math will be added', so use singular").arg(fieldName[FIELD_ACTIVITY_TAG_NAME]).arg(activityTags[at]);
				tmpSet.insert(activityTags[at]);
				fieldList[FIELD_ACTIVITY_TAG_NAME]<<activityTags[at];
			}
		}
	}
	tmpSet.clear();
	//check if already in memory (end)

	QDialog* newParent2;
	ok = showFieldsAndWarnings(newParent, newParent2);
	//DON'T ADD THIS! newParent2->deleteLater();
	newParent=newParent2;
	if(!ok) return;

	//add teachers
	//maybe TODO write a function, so also import teacher csv can share this code
	tmpSet.clear();
	foreach(Teacher* tch, gt.rules.teachersList)
		tmpSet.insert(tch->name);
	int count=0;
	for(int i=0; i<fieldList[FIELD_TEACHER_NAME].size(); i++){
		if(!fieldList[FIELD_TEACHER_NAME][i].isEmpty()){
			Teacher* tch=new Teacher();
			tch->name=fieldList[FIELD_TEACHER_NAME][i];
			assert(!tmpSet.contains(tch->name));
			if(!gt.rules.addTeacherFast(tch)){
				delete tch;
				assert(0);
			} else
				count++;
			tmpSet.insert(tch->name);
		}
	}
	fieldList[FIELD_TEACHER_NAME].clear();
	if(count>0)
		lastWarning+=Import::tr("%1 teachers added. Please check teachers form.").arg(count)+"\n";
	//add subjects
	//maybe TODO write a function, so also import subjects csv can share this code
	tmpSet.clear();
	foreach(Subject* sbj, gt.rules.subjectsList)
		tmpSet.insert(sbj->name);
	count=0;
	for(int i=0; i<fieldList[FIELD_SUBJECT_NAME].size(); i++){
		if(!fieldList[FIELD_SUBJECT_NAME][i].isEmpty() && !tmpSet.contains(fieldList[FIELD_SUBJECT_NAME][i])){
			Subject* s=new Subject();
			s->name=fieldList[FIELD_SUBJECT_NAME][i];
			assert(!tmpSet.contains(s->name));
			if(!gt.rules.addSubjectFast(s)){
				delete s;
				assert(0);
			} else
				count++;
			tmpSet.insert(s->name);
		}
	}
	if(count>0)
		lastWarning+=Import::tr("%1 subjects added. Please check subjects form.").arg(count)+"\n";
	//add activity tags
	//maybe TODO write a function, so also import activity tags csv can share this code
	tmpSet.clear();
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		tmpSet.insert(at->name);
	count=0;
	for(int i=0; i<fieldList[FIELD_ACTIVITY_TAG_NAME].size(); i++){
		if(!fieldList[FIELD_ACTIVITY_TAG_NAME][i].isEmpty()){
			ActivityTag* a=new ActivityTag();
			a->name=fieldList[FIELD_ACTIVITY_TAG_NAME][i];
			assert(!tmpSet.contains(a->name));
			if(!gt.rules.addActivityTag(a)){
				delete a;
				assert(0);
			} else
				count++;
			tmpSet.insert(a->name);
		}
	}
	if(count>0)
		lastWarning+=Import::tr("%1 activity tags added. Please check activity tags form.").arg(count)+"\n";

	//add activities (start) - similar to Livius code modified by Volker
	count=0;
	int count2=0;
	int activityid=0; //We set the id of this newly added activity = (the largest existing id + 1)
	for(int i=0; i<gt.rules.activitiesList.size(); i++){	//TODO: do it the same in addactivityform.cpp (calculate activity id just one time)
		Activity* act=gt.rules.activitiesList[i];
		if(act->id > activityid)
			activityid = act->id;
	}
	activityid++;
	QProgressDialog* _progress4=new QProgressDialog(newParent);
	QProgressDialog& progress4=(*_progress4);
	progress4.setWindowTitle(tr("Importing", "Title of a progress dialog"));
	progress4.setLabelText(tr("Importing activities"));
	progress4.setModal(true);
	progress4.setRange(0, fieldList[FIELD_SUBJECT_NAME].size());

	bool incorrect_bool_consecutive=false;
	
	for(int i=0; i<fieldList[FIELD_SUBJECT_NAME].size(); i++){
		progress4.setValue(i);
		if(progress4.wasCanceled()){
			progress4.setValue(fieldList[FIELD_SUBJECT_NAME].size());
			QMessageBox::warning(newParent, "FET", Import::tr("Importing data canceled by user."));
			//return false;
			ok=false;
			goto ifUserCanceledProgress4;
		}

		bool ok2;
		QString tmpStr=fieldList[FIELD_MIN_DAYS_WEIGHT][i];
//		double weight=tmpStr.toDouble(&ok2);
		double weight=customFETStrToDouble(tmpStr, &ok2);
		assert(ok2);

		QStringList teachers_namesFromFile;
		if(!fieldList[FIELD_TEACHERS_SET][i].isEmpty())
			teachers_namesFromFile = fieldList[FIELD_TEACHERS_SET][i].split("+", QString::SkipEmptyParts);
		
		QStringList teachers_names;
		QSet<QString> _teachersSet;
		foreach(QString teacherName, teachers_namesFromFile){
			//assert(teachersHash.contains(teacherName));
			if(!_teachersSet.contains(teacherName)){
				_teachersSet.insert(teacherName);
				teachers_names<<teacherName;
			} else {
				lastWarning+=tr("Line %1: Activity contains duplicate teacher %2 - please correct that").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(teacherName)+"\n";
			}
		}
		
		QString subject_name = fieldList[FIELD_SUBJECT_NAME][i];
		
		QStringList activity_tags_namesFromFile;
		if(!fieldList[FIELD_ACTIVITY_TAGS_SET][i].isEmpty())
			activity_tags_namesFromFile = fieldList[FIELD_ACTIVITY_TAGS_SET][i].split("+", QString::SkipEmptyParts);
		
		QStringList activity_tags_names;
		QSet<QString> _activityTagsSet;
		foreach(QString activityTag, activity_tags_namesFromFile){
			//assert(activityTagsHash.contains(activityTag));
			if(!_activityTagsSet.contains(activityTag)){
				_activityTagsSet.insert(activityTag);
				activity_tags_names<<activityTag;
			} else {
				lastWarning+=tr("Line %1: Activity contains duplicate activity tag %2 - please correct that").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(activityTag)+"\n";
			}
		}
		
		QStringList students_namesFromFile;
		if(!fieldList[FIELD_STUDENTS_SET][i].isEmpty())
			students_namesFromFile = fieldList[FIELD_STUDENTS_SET][i].split("+", QString::SkipEmptyParts);

		int numberOfStudents=0;
		QStringList students_names;
		QSet<QString> _studentsSet;
		foreach(QString studentsSet, students_namesFromFile){
			assert(studentsHash.contains(studentsSet));
			if(!_studentsSet.contains(studentsSet)){
				_studentsSet.insert(studentsSet);
				students_names<<studentsSet;
			} else {
				lastWarning+=tr("Line %1: Activity contains duplicate students set %2 - please correct that").arg(fieldList[FIELD_LINE_NUMBER][i]).arg(studentsSet)+"\n";
			}
			numberOfStudents+=studentsHash.value(studentsSet)->numberOfStudents;
		}

		QStringList splitDurationList;
		splitDurationList.clear();
		assert(!fieldList[FIELD_SPLIT_DURATION][i].isEmpty());
		splitDurationList = fieldList[FIELD_SPLIT_DURATION][i].split("+", QString::SkipEmptyParts);
		int nsplit=splitDurationList.size();
		if(nsplit==1){
			int duration=fieldList[FIELD_TOTAL_DURATION][i].toInt(&ok2, 10);
			assert(ok2);
			bool active=true;
			//workaround only. Please rethink. (start)
			/*QStringList activity_tag_names;
			activity_tag_names<<activity_tag_name;*/
			//workaround only. Please rethink. (end)
			
			/*Activity a(gt.rules, activityid, 0, teachers_names, subject_name, activity_tags_names, students_names, duration, duration, active, true, -1);
	
			bool already_existing=false;
			for(int i=0; i<gt.rules.activitiesList.size(); i++){
				Activity* act=gt.rules.activitiesList[i];
				if((*act)==a)
					already_existing=true;
			}
			if(already_existing){
				lastWarning+=Import::tr("Activity %1 already exists. A duplicate activity is imported. Please check the dataset!").arg(activityid)+"\n";
			}*/
			bool tmp=gt.rules.addSimpleActivityFast(newParent, activityid, 0, teachers_names, subject_name, activity_tags_names,
				students_names, duration, duration, active, true, -1, numberOfStudents);
			activityid++;
			if(tmp){
				count++;
				count2++;
			}
			else
				QMessageBox::critical(newParent, tr("FET information"), tr("Activity NOT added - please report error"));
		}
		else{ //split activity
			int totalduration;
			int durations[MAX_SPLIT_OF_AN_ACTIVITY];
			bool active[MAX_SPLIT_OF_AN_ACTIVITY];
	
			totalduration=0;
			for(int s=0; s<nsplit; s++){
				durations[s]=splitDurationList[s].toInt(&ok2);
				assert(ok2);
				active[s]=true;
				totalduration+=durations[s];
			}
			assert(totalduration==fieldList[FIELD_TOTAL_DURATION][i].toInt(&ok2));
			assert(ok2);
	
			int minD=fieldList[FIELD_MIN_DAYS][i].toInt(&ok2);
			assert(ok2);
			bool force;
			
			if(fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="YES" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="Y" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="TRUE" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="T" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="1"
			 )
				force=true;
			else if(
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="NO" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="N" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="FALSE" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="F" ||
			 fieldList[FIELD_MIN_DAYS_CONSECUTIVE][i].toUpper()=="0"
			 )
				force=false;
			else{
				incorrect_bool_consecutive=true;
				force=true;
			}
			//workaround only. Please rethink. (start)
			/*QStringList activity_tag_names;
			activity_tag_names<<activity_tag_name;*/
			//workaround only. Please rethink. (end)
			bool tmp=gt.rules.addSplitActivityFast(newParent, activityid, activityid,
				teachers_names, subject_name, activity_tags_names, students_names,
				nsplit, totalduration, durations,
				active, minD, weight, force, true, -1, numberOfStudents);
			activityid+=nsplit;
			if(tmp){
				count++;
				count2+=nsplit;
			}
			else
				QMessageBox::critical(newParent, tr("FET information"), tr("Split activity NOT added - error???"));
		}
	}
	progress4.setValue(fieldList[FIELD_SUBJECT_NAME].size());
	//add activities (end) - similar to Livius code modified by Volker
ifUserCanceledProgress4:

	if(incorrect_bool_consecutive){
		lastWarning.insert(0, tr("Warning: found tags for the 'consecutive' field of min days which are not a valid boolean value (%1) - making them %2").arg("1, 0, yes, no, y, n, true, false, t, f").arg("true")+"\n");
	}

	if(!lastWarning.isEmpty())
		lastWarning.insert(0,Import::tr("Notes:")+"\n");
	if(count>0)
		lastWarning.insert(0,Import::tr("%1 container activities (%2 total activities) added. Please check activities form.").arg(count).arg(count2)+"\n");

	QDialog* newParent3=new LastWarningsDialog(newParent);
	//DON'T ADD THIS! newParent3->deleteLater();
	newParent=newParent3;
	LastWarningsDialog& lwd=(*((LastWarningsDialog*)newParent));
	int w=chooseWidth(lwd.sizeHint().width());
	int h=chooseHeight(lwd.sizeHint().height());
	lwd.resize(w,h);
	centerWidgetOnScreen(&lwd);

	ok=lwd.exec();

	int tmp=fileName.lastIndexOf(FILE_SEP);
	IMPORT_DIRECTORY=fileName.left(tmp);
	gt.rules.internalStructureComputed=false;
}
