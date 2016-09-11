/***************************************************************************
                                FET
                          -------------------
   copyright             : (C) by Liviu Lalescu, Volker Dirr
    email                : Liviu Lalescu: see http://lalescu.ro/liviu/ , Volker Dirr: see http://www.timetabling.de/
 ***************************************************************************
                          statisticsprintform.cpp  -  description
                             -------------------
    begin                : November 2013
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

#include <QtGlobal>

#include "statisticsprintform.h"

#include "timetable.h"
#include "timetable_defs.h"
//#include "timetableexport.h"		//TODO: remove?!

#include "longtextmessagebox.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QString>
#include <QStringList>
#include <QSet>
#include <QList>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#endif

extern Timetable gt;

extern QString generationLocalizedTime;

extern const QString COMPANY;
extern const QString PROGRAM;

//static int numberOfPlacedActivities1;

#ifdef QT_NO_PRINTER
static QMap<QString, int> paperSizesMap;
#else
static QMap<QString, QPrinter::PaperSize> paperSizesMap;
#endif

//const QString CBTablesState="/timetables-combo-box-state";

const QString studentSubjectRBState="/student-subject-state-radio-button-state";
const QString studentTeacherRBState="/student-teacher-state-radio-button-state";
const QString teacherSubjectRBState="/teacher-subject-state-radio-button-state";
const QString teacherStudentRBState="/teacher-student-state-radio-button-state";
const QString subjectStudentRBState="/subject-student-state-radio-button-state";
const QString subjectTeacherRBState="/subject-teacher-state-radio-button-state";

const QString CBBreakState="/page-break-combo-box-state";
const QString CBWhiteSpaceState="/white-space-combo-box-state";
const QString CBprinterModeState="/printer-mode-combo-box-state";
const QString CBpaperSizeState="/paper-size-combo-box-state";
const QString CBorientationModeState="/orientation-mode-combo-box-state";

const QString printDetailedTablesState="/print-detailed-tables-check-box-state";
const QString printActivityTagsState="/print-activity-tags-check-box-state";

const QString activitiesPaddingState="/activity-padding-spin-box-value-state";
const QString tablePaddingState="/table-padding-spin-box-value-state";
const QString fontSizeTableState="/font-size-spin-box-value-state";
const QString maxNamesState="/max-names-spin-box-value-state";
const QString leftPageMarginState="/left-page-margin-spin-box-value-state";
const QString topPageMarginState="/top-page-margin-spin-box-value-state";
const QString rightPageMarginState="/right-page-margin-spin-box-value-state";
const QString bottomPageMarginState="/bottom-page-margin-spin-box-value-state";

StartStatisticsPrint::StartStatisticsPrint()
{
}

StartStatisticsPrint::~StartStatisticsPrint()
{
}

void StartStatisticsPrint::startStatisticsPrint(QWidget* parent)
{
	if(gt.rules.initialized){	//TODO: is this needed?
		//prepare calculation
		StatisticsPrintForm tpfd(parent);
		tpfd.exec();
	}
	else{
		QMessageBox::warning(parent, tr("FET warning"),
		 tr("Printing is currently not possible."));
	}
}

StatisticsPrintForm::StatisticsPrintForm(QWidget *parent): QDialog(parent){
	StatisticsExport::computeHashForIDsStatistics(&statisticValues);
	StatisticsExport::getNamesAndHours(&statisticValues);
	
	this->setWindowTitle(tr("Print statistics matrix dialog"));
	
	QHBoxLayout* wholeDialog=new QHBoxLayout(this);
	
	QVBoxLayout* leftDialog=new QVBoxLayout();

	namesList = new QListWidget();
	namesList->setSelectionMode(QAbstractItemView::MultiSelection);

	QHBoxLayout* selectUnselect=new QHBoxLayout();
	pbSelectAll=new QPushButton(tr("All", "Refers to a list of items, select all. Please keep translation short"));
	//pbSelectAll->setAutoDefault(false);
	pbUnselectAll=new QPushButton(tr("None", "Refers to a list of items, select none. Please keep translation short"));
	selectUnselect->addWidget(pbSelectAll);
	selectUnselect->addWidget(pbUnselectAll);

	leftDialog->addWidget(namesList);
	leftDialog->addLayout(selectUnselect);
	
	QVBoxLayout* rightDialog=new QVBoxLayout();
	
	/*QGroupBox**/ actionsBox=new QGroupBox(tr("Print"));
	QGridLayout* actionsBoxGrid=new QGridLayout();
	studentSubjectRB= new QRadioButton(tr("students-subjects"));
	studentTeacherRB= new QRadioButton(tr("students-teachers"));
	teacherSubjectRB= new QRadioButton(tr("teachers-subjects"));
	teacherStudentRB= new QRadioButton(tr("teachers-students"));
	subjectStudentRB= new QRadioButton(tr("subjects-students"));
	subjectTeacherRB= new QRadioButton(tr("subjects-teachers"));

	actionsBoxGrid->addWidget(studentSubjectRB,0,0);
	actionsBoxGrid->addWidget(studentTeacherRB,0,1);
	actionsBoxGrid->addWidget(teacherSubjectRB,1,0);
	actionsBoxGrid->addWidget(teacherStudentRB,1,1);
	actionsBoxGrid->addWidget(subjectStudentRB,2,0);
	actionsBoxGrid->addWidget(subjectTeacherRB,2,1);
	studentSubjectRB->setChecked(true);
	actionsBox->setLayout(actionsBoxGrid);
	
	/*QGroupBox**/ optionsBox=new QGroupBox(tr("Options"));
	QGridLayout* optionsBoxGrid=new QGridLayout();
	
	QStringList breakStrings;
	breakStrings<<tr("Page-break: none", "No page-break between statistics. Please keep translation short")
		<<tr("Page-break: always", "Page-break after each statistic. Please keep translation short")
		<<tr("Page-break: even", "Page-break after each even statistic. Please keep translation short");
	CBBreak=new QComboBox();
	CBBreak->addItems(breakStrings);
	CBBreak->setCurrentIndex(1);
	CBBreak->setSizePolicy(QSizePolicy::Expanding, CBBreak->sizePolicy().verticalPolicy());
	
	QStringList whiteSpaceStrings;
	whiteSpaceStrings<<QString("normal")<<QString("pre")<<QString("nowrap")<<QString("pre-wrap");	//don't translate these strings, because they are css parameters!
	CBWhiteSpace=new QComboBox();
	CBWhiteSpace->addItems(whiteSpaceStrings);
	CBWhiteSpace->setCurrentIndex(0);
	CBWhiteSpace->setSizePolicy(QSizePolicy::Expanding, CBWhiteSpace->sizePolicy().verticalPolicy());
	
	QStringList printerOrientationStrings;
	printerOrientationStrings<<tr("Portrait")<<tr("Landscape");
	CBorientationMode=new QComboBox();
	CBorientationMode->addItems(printerOrientationStrings);
	CBorientationMode->setCurrentIndex(0);
	//CBorientationMode->setDisabled(true);
	CBorientationMode->setSizePolicy(QSizePolicy::Expanding, CBorientationMode->sizePolicy().verticalPolicy());
	
/*	QStringList printerModeStrings;
	printerModeStrings<<tr("ScreenResolution")<<tr("PrinterResolution")<<tr("HighResolution");
	CBprinterMode=new QComboBox();
	CBprinterMode->addItems(printerModeStrings);
	CBprinterMode->setCurrentIndex(2);
	CBprinterMode->setDisabled(true);
	CBprinterMode->setSizePolicy(QSizePolicy::Expanding, CBprinterMode->sizePolicy().verticalPolicy());
*/	
	paperSizesMap.clear();
#ifdef QT_NO_PRINTER
	paperSizesMap.insert(tr("Custom", "Type of paper size"), 30);
#else
	paperSizesMap.insert(tr("A0", "Type of paper size"), QPrinter::A0);
	paperSizesMap.insert(tr("A1", "Type of paper size"), QPrinter::A1);
	paperSizesMap.insert(tr("A2", "Type of paper size"), QPrinter::A2);
	paperSizesMap.insert(tr("A3", "Type of paper size"), QPrinter::A3);
	paperSizesMap.insert(tr("A4", "Type of paper size"), QPrinter::A4);
	paperSizesMap.insert(tr("A5", "Type of paper size"), QPrinter::A5);
	paperSizesMap.insert(tr("A6", "Type of paper size"), QPrinter::A6);
	paperSizesMap.insert(tr("A7", "Type of paper size"), QPrinter::A7);
	paperSizesMap.insert(tr("A8", "Type of paper size"), QPrinter::A8);
	paperSizesMap.insert(tr("A9", "Type of paper size"), QPrinter::A9);
	paperSizesMap.insert(tr("B0", "Type of paper size"), QPrinter::B0);
	paperSizesMap.insert(tr("B1", "Type of paper size"), QPrinter::B1);
	paperSizesMap.insert(tr("B2", "Type of paper size"), QPrinter::B2);
	paperSizesMap.insert(tr("B3", "Type of paper size"), QPrinter::B3);
	paperSizesMap.insert(tr("B4", "Type of paper size"), QPrinter::B4);
	paperSizesMap.insert(tr("B5", "Type of paper size"), QPrinter::B5);
	paperSizesMap.insert(tr("B6", "Type of paper size"), QPrinter::B6);
	paperSizesMap.insert(tr("B7", "Type of paper size"), QPrinter::B7);
	paperSizesMap.insert(tr("B8", "Type of paper size"), QPrinter::B8);
	paperSizesMap.insert(tr("B9", "Type of paper size"), QPrinter::B9);
	paperSizesMap.insert(tr("B10", "Type of paper size"), QPrinter::B10);
	paperSizesMap.insert(tr("C5E", "Type of paper size"), QPrinter::C5E);
	paperSizesMap.insert(tr("Comm10E", "Type of paper size"), QPrinter::Comm10E);
	paperSizesMap.insert(tr("DLE", "Type of paper size"), QPrinter::DLE);
	paperSizesMap.insert(tr("Executive", "Type of paper size"), QPrinter::Executive);
	paperSizesMap.insert(tr("Folio", "Type of paper size"), QPrinter::Folio);
	paperSizesMap.insert(tr("Ledger", "Type of paper size"), QPrinter::Ledger);
	paperSizesMap.insert(tr("Legal", "Type of paper size"), QPrinter::Legal);
	paperSizesMap.insert(tr("Letter", "Type of paper size"), QPrinter::Letter);
	paperSizesMap.insert(tr("Tabloid", "Type of paper size"), QPrinter::Tabloid);
#endif

	CBpaperSize=new QComboBox();
	CBpaperSize->addItems(paperSizesMap.keys());
	if(CBpaperSize->count()>=5)
		CBpaperSize->setCurrentIndex(4);
	else if(CBpaperSize->count()>=1)
		CBpaperSize->setCurrentIndex(0);
	CBpaperSize->setSizePolicy(QSizePolicy::Expanding, CBpaperSize->sizePolicy().verticalPolicy());
	
//	markNotAvailable=new QCheckBox(tr("Mark not available"));
//	markNotAvailable->setChecked(true);
	
//	markBreak=new QCheckBox(tr("Mark break"));
//	markBreak->setChecked(true);
	
//	printSameStartingTime=new QCheckBox(tr("Print same starting time"));
//	printSameStartingTime->setChecked(false);

//	printDetailedTables=new QCheckBox(tr("Detailed tables"));
//	printDetailedTables->setChecked(true);
	
	printActivityTags=new QCheckBox(tr("Activity tags"));
	printActivityTags->setChecked(false);
	
	fontSizeTable=new QSpinBox;
	fontSizeTable->setRange(4, 20);
	fontSizeTable->setValue(8);

	/*QString str, left, right;
	str=tr("Font size: %1 pt");
	dividePrefixSuffix(str, left, right);
	fontSizeTable->setPrefix(left);
	fontSizeTable->setSuffix(right);*/
	
	QString s=tr("Font size: %1 pt", "pt means points for font size, when printing the statistics");
	QStringList sl=s.split("%1");
	QString prefix=sl.at(0);
	QString suffix;
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	fontSizeTable->setPrefix(prefix);
	fontSizeTable->setSuffix(suffix);
	//fontSizeTable->setPrefix(tr("Font size:")+QString(" "));
	//fontSizeTable->setSuffix(QString(" ")+tr("pt", "Means points for font size, when printing the statistics"));

	fontSizeTable->setSizePolicy(QSizePolicy::Expanding, fontSizeTable->sizePolicy().verticalPolicy());
	
	activitiesPadding=new QSpinBox;
	activitiesPadding->setRange(0, 25);
	activitiesPadding->setValue(0);

	/*str=tr("Activities padding: %1 px");
	dividePrefixSuffix(str, left, right);
	activitiesPadding->setPrefix(left);
	activitiesPadding->setSuffix(right);*/

	s=tr("Activities padding: %1 px", "px means pixels, when printing the statistics");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	activitiesPadding->setPrefix(prefix);
	activitiesPadding->setSuffix(suffix);
	//activitiesPadding->setPrefix(tr("Activities padding:")+QString(" "));
	//activitiesPadding->setSuffix(QString(" ")+tr("px", "Means pixels, when printing the statistics"));

	activitiesPadding->setSizePolicy(QSizePolicy::Expanding, activitiesPadding->sizePolicy().verticalPolicy());
	
	tablePadding=new QSpinBox;
	tablePadding->setRange(1, 99);
	tablePadding->setValue(1);

	/*str=tr("Space after table: +%1 px");
	dividePrefixSuffix(str, left, right);
	tablePadding->setPrefix(left);
	tablePadding->setSuffix(right);*/

	s=tr("Space after table: +%1 px", "px means pixels, when printing the statistics");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	tablePadding->setPrefix(prefix);
	tablePadding->setSuffix(suffix);
	//tablePadding->setPrefix(tr("Space after table:")+QString(" +"));
	//tablePadding->setSuffix(QString(" ")+tr("px", "Means pixels, when printing the statistics"));

	tablePadding->setSizePolicy(QSizePolicy::Expanding, tablePadding->sizePolicy().verticalPolicy());
	
	maxNames=new QSpinBox;
	maxNames->setRange(1, 999);
	maxNames->setValue(10);
	
	/*str=tr("Split after %1 names");
	dividePrefixSuffix(str, left, right);
	maxNames->setPrefix(left);
	maxNames->setSuffix(right);*/

	s=tr("Split after: %1 names");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	maxNames->setPrefix(prefix);
	maxNames->setSuffix(suffix);
	//maxNames->setPrefix(tr("Split after:", "When printing, the whole phrase is 'Split after ... names'")+QString(" "));
	//maxNames->setSuffix(QString(" ")+tr("names", "When printing, the whole phrase is 'Split after ... names'"));
	
	maxNames->setSizePolicy(QSizePolicy::Expanding, maxNames->sizePolicy().verticalPolicy());

	leftPageMargin=new QSpinBox;
	leftPageMargin->setRange(0, 50);
	leftPageMargin->setValue(10);

	/*str=tr("Left margin: %1 mm");
	dividePrefixSuffix(str, left, right);
	leftPageMargin->setPrefix(left);
	leftPageMargin->setSuffix(right);*/

	s=tr("Left margin: %1 mm", "mm means millimeters");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	leftPageMargin->setPrefix(prefix);
	leftPageMargin->setSuffix(suffix);
	//leftPageMargin->setPrefix(tr("Left margin:")+QString(" "));
	//leftPageMargin->setSuffix(QString(" ")+tr("mm", "Means milimeter, when setting page margin"));

	leftPageMargin->setSizePolicy(QSizePolicy::Expanding, leftPageMargin->sizePolicy().verticalPolicy());
	
	topPageMargin=new QSpinBox;
	topPageMargin->setRange(0, 50);
	topPageMargin->setValue(10);

	/*str=tr("Top margin: %1 mm");
	dividePrefixSuffix(str, left, right);
	topPageMargin->setPrefix(left);
	topPageMargin->setSuffix(right);*/

	s=tr("Top margin: %1 mm", "mm means millimeters");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	topPageMargin->setPrefix(prefix);
	topPageMargin->setSuffix(suffix);
	//topPageMargin->setPrefix(tr("Top margin:")+QString(" "));
	//topPageMargin->setSuffix(QString(" ")+tr("mm", "Means milimeter, when setting page margin"));

	topPageMargin->setSizePolicy(QSizePolicy::Expanding, topPageMargin->sizePolicy().verticalPolicy());
	
	rightPageMargin=new QSpinBox;
	rightPageMargin->setRange(0, 50);
	rightPageMargin->setValue(10);

	/*str=tr("Right margin: %1 mm");
	dividePrefixSuffix(str, left, right);
	rightPageMargin->setPrefix(left);
	rightPageMargin->setSuffix(right);*/

	s=tr("Right margin: %1 mm", "mm means millimeters");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	rightPageMargin->setPrefix(prefix);
	rightPageMargin->setSuffix(suffix);
	//rightPageMargin->setPrefix(tr("Right margin:")+QString(" "));
	//rightPageMargin->setSuffix(QString(" ")+tr("mm", "Means milimeter, when setting page margin"));

	rightPageMargin->setSizePolicy(QSizePolicy::Expanding, rightPageMargin->sizePolicy().verticalPolicy());
	
	bottomPageMargin=new QSpinBox;
	bottomPageMargin->setRange(0, 50);
	bottomPageMargin->setValue(10);

	/*str=tr("Bottom margin: %1 mm");
	dividePrefixSuffix(str, left, right);
	bottomPageMargin->setPrefix(left);
	bottomPageMargin->setSuffix(right);*/

	s=tr("Bottom margin: %1 mm", "mm means millimeters");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	bottomPageMargin->setPrefix(prefix);
	bottomPageMargin->setSuffix(suffix);
	//bottomPageMargin->setPrefix(tr("Bottom margin:")+QString(" "));
	//bottomPageMargin->setSuffix(QString(" ")+tr("mm", "Means milimeter, when setting page margin"));

	bottomPageMargin->setSizePolicy(QSizePolicy::Expanding, bottomPageMargin->sizePolicy().verticalPolicy());
	
	pbPrintPreviewSmall=new QPushButton(tr("Teaser", "Small print preview. Please keep translation short"));
	pbPrintPreviewFull=new QPushButton(tr("Preview", "Full print preview. Please keep translation short"));
	pbPrint=new QPushButton(tr("Print", "Please keep translation short"));

	pbClose=new QPushButton(tr("Close", "Please keep translation short"));
	pbClose->setAutoDefault(false);
	
	optionsBoxGrid->addWidget(leftPageMargin,0,0);
	optionsBoxGrid->addWidget(rightPageMargin,1,0);
	optionsBoxGrid->addWidget(topPageMargin,2,0);
	optionsBoxGrid->addWidget(bottomPageMargin,3,0);
	
	optionsBoxGrid->addWidget(fontSizeTable,0,1);
	optionsBoxGrid->addWidget(maxNames,1,1);
	optionsBoxGrid->addWidget(activitiesPadding,2,1);
	optionsBoxGrid->addWidget(tablePadding,3,1);
	
	optionsBoxGrid->addWidget(CBpaperSize,4,0);
	optionsBoxGrid->addWidget(CBWhiteSpace,4,1);
	optionsBoxGrid->addWidget(CBorientationMode,5,0);
	optionsBoxGrid->addWidget(CBBreak,5,1);
//	optionsBoxGrid->addWidget(CBprinterMode,5,0);
	optionsBoxGrid->addWidget(printActivityTags,6,0);
//	optionsBoxGrid->addWidget(printDetailedTables,6,1);

	optionsBox->setLayout(optionsBoxGrid);
	optionsBox->setSizePolicy(QSizePolicy::Expanding, optionsBox->sizePolicy().verticalPolicy());

	QHBoxLayout* previewPrintClose=new QHBoxLayout();
	previewPrintClose->addStretch();
	previewPrintClose->addWidget(pbPrintPreviewSmall);
	previewPrintClose->addWidget(pbPrintPreviewFull);
	previewPrintClose->addWidget(pbPrint);
	previewPrintClose->addStretch();
	previewPrintClose->addWidget(pbClose);

	rightDialog->addWidget(actionsBox);
	rightDialog->addWidget(optionsBox);
	rightDialog->addStretch();
	rightDialog->addLayout(previewPrintClose);

	//wholeDialog->addWidget(textDocument);
	wholeDialog->addLayout(leftDialog);
	wholeDialog->addLayout(rightDialog);
	
	updateNamesList();
	
	connect(pbSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
	connect(pbUnselectAll, SIGNAL(clicked()), this, SLOT(unselectAll()));
	connect(pbPrint, SIGNAL(clicked()), this, SLOT(print()));
	connect(pbPrintPreviewSmall, SIGNAL(clicked()), this, SLOT(printPreviewSmall()));
	connect(pbPrintPreviewFull, SIGNAL(clicked()), this, SLOT(printPreviewFull()));
	connect(pbClose, SIGNAL(clicked()), this, SLOT(close()));
	
	connect(studentSubjectRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));
	connect(studentTeacherRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));
	connect(teacherSubjectRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));
	connect(teacherStudentRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));
	connect(subjectStudentRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));
	connect(subjectTeacherRB, SIGNAL(toggled(bool)), this, SLOT(updateNamesList()));

	int ww=this->sizeHint().width();
	if(ww>900)
		ww=900;
	if(ww<700)
		ww=700;

	int hh=this->sizeHint().height();
	if(hh>650)
		hh=650;
	if(hh<500)
		hh=500;
	
	this->resize(ww, hh);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
		
	if(settings.contains(this->metaObject()->className()+studentSubjectRBState))
		studentSubjectRB->setChecked(settings.value(this->metaObject()->className()+studentSubjectRBState).toBool());
	if(settings.contains(this->metaObject()->className()+studentTeacherRBState))
		studentTeacherRB->setChecked(settings.value(this->metaObject()->className()+studentTeacherRBState).toBool());
	if(settings.contains(this->metaObject()->className()+teacherSubjectRBState))
		teacherSubjectRB->setChecked(settings.value(this->metaObject()->className()+teacherSubjectRBState).toBool());
	if(settings.contains(this->metaObject()->className()+teacherStudentRBState))
		teacherStudentRB->setChecked(settings.value(this->metaObject()->className()+teacherStudentRBState).toBool());
	if(settings.contains(this->metaObject()->className()+subjectStudentRBState))
		subjectStudentRB->setChecked(settings.value(this->metaObject()->className()+subjectStudentRBState).toBool());
	if(settings.contains(this->metaObject()->className()+subjectTeacherRBState))
		subjectTeacherRB->setChecked(settings.value(this->metaObject()->className()+subjectTeacherRBState).toBool());
	//
	if(settings.contains(this->metaObject()->className()+CBBreakState))
		CBBreak->setCurrentIndex(settings.value(this->metaObject()->className()+CBBreakState).toInt());
	if(settings.contains(this->metaObject()->className()+CBWhiteSpaceState))
		CBWhiteSpace->setCurrentIndex(settings.value(this->metaObject()->className()+CBWhiteSpaceState).toInt());
	//if(settings.contains(this->metaObject()->className()+CBprinterModeState))
	//	CBprinterMode->setCurrentIndex(settings.value(this->metaObject()->className()+CBprinterModeState).toInt());
	if(settings.contains(this->metaObject()->className()+CBpaperSizeState))
		CBpaperSize->setCurrentIndex(settings.value(this->metaObject()->className()+CBpaperSizeState).toInt());
	if(settings.contains(this->metaObject()->className()+CBorientationModeState))
		CBorientationMode->setCurrentIndex(settings.value(this->metaObject()->className()+CBorientationModeState).toInt());
//	if(settings.contains(this->metaObject()->className()+printDetailedTablesState))
//		printDetailedTables->setChecked(settings.value(this->metaObject()->className()+printDetailedTablesState).toBool());
	if(settings.contains(this->metaObject()->className()+printActivityTagsState))
		printActivityTags->setChecked(settings.value(this->metaObject()->className()+printActivityTagsState).toBool());
	//
	if(settings.contains(this->metaObject()->className()+activitiesPaddingState))
		activitiesPadding->setValue(settings.value(this->metaObject()->className()+activitiesPaddingState).toInt());
	if(settings.contains(this->metaObject()->className()+tablePaddingState))
		tablePadding->setValue(settings.value(this->metaObject()->className()+tablePaddingState).toInt());
	if(settings.contains(this->metaObject()->className()+fontSizeTableState))
		fontSizeTable->setValue(settings.value(this->metaObject()->className()+fontSizeTableState).toInt());
	if(settings.contains(this->metaObject()->className()+maxNamesState))
		maxNames->setValue(settings.value(this->metaObject()->className()+maxNamesState).toInt());
	if(settings.contains(this->metaObject()->className()+leftPageMarginState))
		leftPageMargin->setValue(settings.value(this->metaObject()->className()+leftPageMarginState).toInt());
	if(settings.contains(this->metaObject()->className()+topPageMarginState))
		topPageMargin->setValue(settings.value(this->metaObject()->className()+topPageMarginState).toInt());
	if(settings.contains(this->metaObject()->className()+CBorientationModeState))
		rightPageMargin->setValue(settings.value(this->metaObject()->className()+rightPageMarginState).toInt());
	if(settings.contains(this->metaObject()->className()+bottomPageMarginState))
		bottomPageMargin->setValue(settings.value(this->metaObject()->className()+bottomPageMarginState).toInt());
}

StatisticsPrintForm::~StatisticsPrintForm(){
	saveFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
	//save other settings

	settings.setValue(this->metaObject()->className()+studentSubjectRBState, studentSubjectRB->isChecked());
	settings.setValue(this->metaObject()->className()+studentTeacherRBState, studentTeacherRB->isChecked());
	settings.setValue(this->metaObject()->className()+teacherSubjectRBState, teacherSubjectRB->isChecked());
	settings.setValue(this->metaObject()->className()+teacherStudentRBState, teacherStudentRB->isChecked());
	settings.setValue(this->metaObject()->className()+subjectStudentRBState, subjectStudentRB->isChecked());
	settings.setValue(this->metaObject()->className()+subjectTeacherRBState, subjectTeacherRB->isChecked());
	//
	settings.setValue(this->metaObject()->className()+CBBreakState, CBBreak->currentIndex());
	settings.setValue(this->metaObject()->className()+CBWhiteSpaceState, CBWhiteSpace->currentIndex());
	//settings.setValue(this->metaObject()->className()+CBprinterModeState, CBprinterMode->currentIndex());
	settings.setValue(this->metaObject()->className()+CBpaperSizeState, CBpaperSize->currentIndex());
	settings.setValue(this->metaObject()->className()+CBorientationModeState, CBorientationMode->currentIndex());
	//
//	settings.setValue(this->metaObject()->className()+printDetailedTablesState, printDetailedTables->isChecked());
	settings.setValue(this->metaObject()->className()+printActivityTagsState, printActivityTags->isChecked());
	//
	settings.setValue(this->metaObject()->className()+activitiesPaddingState, activitiesPadding->value());
	settings.setValue(this->metaObject()->className()+tablePaddingState, tablePadding->value());
	settings.setValue(this->metaObject()->className()+fontSizeTableState, fontSizeTable->value());
	settings.setValue(this->metaObject()->className()+maxNamesState, maxNames->value());
	settings.setValue(this->metaObject()->className()+leftPageMarginState, leftPageMargin->value());
	settings.setValue(this->metaObject()->className()+topPageMarginState, topPageMargin->value());
	settings.setValue(this->metaObject()->className()+rightPageMarginState, rightPageMargin->value());
	settings.setValue(this->metaObject()->className()+bottomPageMarginState, bottomPageMargin->value());
}

void StatisticsPrintForm::selectAll(){
	namesList->selectAll();
}

void StatisticsPrintForm::unselectAll(){
	namesList->clearSelection();
}

void StatisticsPrintForm::updateNamesList(){
	namesList->clear();
	
	if(studentSubjectRB->isChecked() || studentTeacherRB->isChecked()){
		int count=0;
		foreach(QString student, statisticValues.allStudentsNames){
			namesList->addItem(student);
			QListWidgetItem* tmpItem=namesList->item(count);
			tmpItem->setSelected(true);
			count++;
		}	
	}
	if(teacherSubjectRB->isChecked() || teacherStudentRB->isChecked()){
		int count=0;
		foreach(QString teacher, statisticValues.allTeachersNames){
			namesList->addItem(teacher);
			QListWidgetItem* tmpItem=namesList->item(count);
			tmpItem->setSelected(true);
			count++;
		}	
	}
	if(subjectStudentRB->isChecked() || subjectTeacherRB->isChecked()){
	int count=0;
		foreach(QString subject, statisticValues.allSubjectsNames){
			namesList->addItem(subject);
			QListWidgetItem* tmpItem=namesList->item(count);
			tmpItem->setSelected(true);
			count++;
		}	
	}	
}

QString StatisticsPrintForm::updateHTMLprintString(bool printAll){
	QString saveTime=generationLocalizedTime;

	QString tmp;
	tmp+="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tmp+="  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";
	
	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\">\n";
	else
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\" dir=\"rtl\">\n";

	//QTBUG-9438
	//QTBUG-2730
	tmp+="  <head>\n";
	tmp+="    <title>"+protect2(gt.rules.institutionName)+"</title>\n";
	tmp+="    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	tmp+="    <style type=\"text/css\">\n";
	
	//this variant doesn't need the "back" stuff, but there will be an empty last page!
	//but you need to care about correct odd and even like in the groups tables
/*	tmp+="      table.even_table {\n";
	if(CBBreak->currentIndex()==1 || CBBreak->currentIndex()==2){
		tmp+="        page-break-after: always;";
	} //else {
	//tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug. (*1*)
	//tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug. (*1*)
	//}
	tmp+="      }\n";
	tmp+="      table.odd_table {\n";
	if(CBBreak->currentIndex()==1){
		tmp+="        page-break-after: always;";
	} //else {
	//tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug. (*1*)
	//tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug. (*1*)
	//}
	tmp+="      }\n";
*/
	
	//start. the "back" stuff is needed because of an qt bug (*1*). it also solve the last empty page problem.
	tmp+="      p.back0 {\n";	//i can't to that with a class in table, because of a qt bug
	if(CBBreak->currentIndex()==0)
		tmp+="        font-size: "+QString::number(tablePadding->value())+"pt;\n";	//i can't do that in table, because it will also effect detailed table cells. it is not possible with a class, because of a qt bug.
	else
		tmp+="        font-size: 1pt;\n";	//font size 0 is not possible.
//	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
//	tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
	if(CBBreak->currentIndex()==1 || CBBreak->currentIndex()==2)
		tmp+="        page-break-after: always;";
	tmp+="      }\n";
	tmp+="      p.back1 {\n";	//i can't to that with a class in table, because of a qt bug
	if(CBBreak->currentIndex()==0 || CBBreak->currentIndex()==2)
		tmp+="        font-size: "+QString::number(tablePadding->value())+"pt;\n";	//i can't do that in table, because it will also effect detailed table cells. it is not possible with a class, because of a qt bug.
	else
		tmp+="        font-size: 1pt;\n";	//font size 0 is not possible.
//	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
//	tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
	if(CBBreak->currentIndex()==1)
		tmp+="        page-break-after: always;";
	tmp+="      }\n";
	//end. the "back" stuff is only needed because of an qt bug (*1*). delete this as soon as bug is solved
	
	tmp+="      table {\n";
	tmp+="        font-size: "+QString::number(fontSizeTable->value())+"pt;\n";
	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";
	tmp+="      }\n";
	tmp+="      th {\n";
	tmp+="        text-align: center;\n"; //currently no effect because of an qt bug (compare http://bugreports.qt.nokia.com/browse/QTBUG-2730 )
	tmp+="        vertical-align: middle;\n";
	tmp+="        white-space: "+CBWhiteSpace->currentText()+";\n";
	tmp+="      }\n";
	tmp+="      td {\n";
	tmp+="        text-align: center;\n"; //currently no effect because of an qt bug (compare http://bugreports.qt.nokia.com/browse/QTBUG-2730 )
	tmp+="        vertical-align: middle;\n";
	tmp+="        white-space: "+CBWhiteSpace->currentText()+";\n";
	tmp+="        padding-left: "+QString::number(activitiesPadding->value())+"px;\n";
	tmp+="        padding-right: "+QString::number(activitiesPadding->value())+"px;\n";
	tmp+="      }\n";
	tmp+="      td.detailed {\n";
//	tmp+="        padding-left: 4px;\n";
//	tmp+="        padding-right: 4px;\n";
	tmp+="      }\n";
	tmp+="      th.xAxis {\n";	//need level 2
//	tmp+="        padding-left: 4px;\n";
//	tmp+="        padding-right: 4px;\n";
	tmp+="      }\n";
	tmp+="      th.yAxis {\n";	//need level 2
//	tmp+="        padding-top: 4px;\n";
//	tmp+="        padding-bottom: 4px;\n";
	tmp+="      }\n";
	tmp+="      tr.line0, div.line0 {\n";	//need level 3
	tmp+="        /*font-size: 12pt;*/\n";
	tmp+="        color: gray;\n";
	tmp+="      }\n";
	tmp+="      tr.line1, div.line1 {\n";	//need level 3
	tmp+="        /*font-size: 12pt;*/\n";
	tmp+="      }\n";
	tmp+="      tr.line2, div.line2 {\n";	//need level 3
	tmp+="        /*font-size: 12pt;*/\n";
	tmp+="        color: gray;\n";
	tmp+="      }\n";
	tmp+="      tr.line3, div.line3 {\n";	//need level 3
	tmp+="        /*font-size: 12pt;*/\n";
	tmp+="        color: silver;\n";
	tmp+="      }\n";
	tmp+="    </style>\n";
	tmp+="  </head>\n\n";
	tmp+="  <body id=\"top\">\n";

	//if(numberOfPlacedActivities1!=gt.rules.nInternalActivities)
	//	tmp+="    <h1>"+tr("Warning! Only %1 out of %2 activities placed!").arg(numberOfPlacedActivities1).arg(gt.rules.nInternalActivities)+"</h1>\n";

	//QList<int> includedNamesIndex;
	QSet<int> excludedNamesIndex;
	for(int nameIndex=0; nameIndex<namesList->count(); nameIndex++){
		QListWidgetItem* tmpItem=namesList->item(nameIndex);
		if(tmpItem->isSelected()){
			//includedNamesIndex<<nameIndex;
		} else {
			excludedNamesIndex<<nameIndex;
		}
	}
	
	//maybe TODO: do the pagebreak similar in timetableexport. (so remove the odd and even table tag and use only back1 and back2 (maybe rename to odd and even))
	//            check the GroupsTimetableDaysHorizontalHtml and Year parameter then (iNi%2) isn't needed anymore then.
	
	if(studentSubjectRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsStudentsSubjectsHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	if(studentTeacherRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsStudentsTeachersHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	if(teacherSubjectRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsTeachersSubjectsHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	if(teacherStudentRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsTeachersStudentsHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	if(subjectStudentRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsSubjectsStudentsHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	if(subjectTeacherRB->isChecked()){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			tmp+=StatisticsExport::exportStatisticsSubjectsTeachersHtml(NULL/*parent*/, saveTime, statisticValues, 3, printActivityTags->isChecked(), maxNames->value(), &excludedNamesIndex);
			if(excludedNamesIndex.size()<namesList->count()){
				if(count%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
				count++;
			}
		}
	}
	// end
	
	tmp+="  </body>\n";
	tmp+="</html>\n\n";
	return tmp;
}

/*void StatisticsPrintForm::updateCBDivideTimeAxisByDay()
{
	CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
}*/

void StatisticsPrintForm::print(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please export and open the HTML statistics from the results directory"));
#else
	QPrinter printer(QPrinter::HighResolution);

	assert(paperSizesMap.contains(CBpaperSize->currentText()));
	printer.setPaperSize(paperSizesMap.value(CBpaperSize->currentText()));

	switch(CBorientationMode->currentIndex()){
		case 0: printer.setOrientation(QPrinter::Portrait); break;
		case 1: printer.setOrientation(QPrinter::Landscape); break;
		default: assert(0==1);
	}
#if QT_VERSION >= 0x050300
	QMarginsF printerMargins;
	printerMargins.setLeft(leftPageMargin->value());
	printerMargins.setRight(rightPageMargin->value());
	printerMargins.setBottom(bottomPageMargin->value());
	printerMargins.setTop(topPageMargin->value());
	if(!printer.setPageMargins(printerMargins, QPageLayout::Millimeter)){
		printerMargins=printer.pageLayout().minimumMargins();
		QMessageBox::warning(this, tr("FET warning"), tr("No margins set, because at least one value is too small. "
		"You need to enter at least:\nLeft: %1\nRight: %2\nTop: %3\nBottom: %4")
		.arg(printerMargins.left()).arg(printerMargins.right()).arg(printerMargins.top()).arg(printerMargins.bottom()));
	}
#else
	printer.setPageMargins(leftPageMargin->value(), topPageMargin->value(), rightPageMargin->value(), bottomPageMargin->value(), QPrinter::Millimeter);
#endif
	//QPrintDialog *printDialog = new QPrintDialog(&printer, this);
	QPrintDialog printDialog(&printer, this);
	printDialog.setWindowTitle(tr("Print statistics"));
	if (printDialog.exec() == QDialog::Accepted) {
		QTextDocument textDocument;
		textDocument.documentLayout()->setPaintDevice(&printer);
		textDocument.setPageSize(QSizeF(printer.pageRect().size()));
		textDocument.setHtml(updateHTMLprintString(true));
		textDocument.print(&printer);
	}
	//delete printDialog;
#endif
}

void StatisticsPrintForm::printPreviewFull(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please export and open the HTML statistics from the results directory"));
#else
	QPrinter printer(QPrinter::HighResolution);

	assert(paperSizesMap.contains(CBpaperSize->currentText()));
	printer.setPaperSize(paperSizesMap.value(CBpaperSize->currentText()));

	switch(CBorientationMode->currentIndex()){
		case 0: printer.setOrientation(QPrinter::Portrait); break;
		case 1: printer.setOrientation(QPrinter::Landscape); break;
		default: assert(0==1);
	}
#if QT_VERSION >= 0x050300
	QMarginsF printerMargins;
	printerMargins.setLeft(leftPageMargin->value());
	printerMargins.setRight(rightPageMargin->value());
	printerMargins.setBottom(bottomPageMargin->value());
	printerMargins.setTop(topPageMargin->value());
	if(!printer.setPageMargins(printerMargins, QPageLayout::Millimeter)){
		printerMargins=printer.pageLayout().minimumMargins();
		QMessageBox::warning(this, tr("FET warning"), tr("No margins set, because at least one value is too small. "
		"You need to enter at least:\nLeft: %1\nRight: %2\nTop: %3\nBottom: %4")
		.arg(printerMargins.left()).arg(printerMargins.right()).arg(printerMargins.top()).arg(printerMargins.bottom()));
	}
#else
	printer.setPageMargins(leftPageMargin->value(), topPageMargin->value(), rightPageMargin->value(), bottomPageMargin->value(), QPrinter::Millimeter);
#endif
	QPrintPreviewDialog printPreviewFull(&printer, this);
	connect(&printPreviewFull, SIGNAL(paintRequested(QPrinter*)), SLOT(updatePreviewFull(QPrinter*)));
	printPreviewFull.exec();
#endif
}

void StatisticsPrintForm::updatePreviewFull(QPrinter* printer){
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);

	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please export and open the HTML statistics from the results directory"));
#else
	QTextDocument textDocument;
	textDocument.documentLayout()->setPaintDevice(printer);
	textDocument.setPageSize(QSizeF(printer->pageRect().size()));
	textDocument.setHtml(updateHTMLprintString(true));
	textDocument.print(printer);
#endif
}

void StatisticsPrintForm::printPreviewSmall(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please export and open the HTML statistics from the results directory"));
#else
	QPrinter printer(QPrinter::HighResolution);

	assert(paperSizesMap.contains(CBpaperSize->currentText()));
	printer.setPaperSize(paperSizesMap.value(CBpaperSize->currentText()));

	switch(CBorientationMode->currentIndex()){
		case 0: printer.setOrientation(QPrinter::Portrait); break;
		case 1: printer.setOrientation(QPrinter::Landscape); break;
		default: assert(0==1);
	}
#if QT_VERSION >= 0x050300
	QMarginsF printerMargins;
	printerMargins.setLeft(leftPageMargin->value());
	printerMargins.setRight(rightPageMargin->value());
	printerMargins.setBottom(bottomPageMargin->value());
	printerMargins.setTop(topPageMargin->value());
	if(!printer.setPageMargins(printerMargins, QPageLayout::Millimeter)){
		printerMargins=printer.pageLayout().minimumMargins();
		QMessageBox::warning(this, tr("FET warning"), tr("No margins set, because at least one value is too small. "
		"You need to enter at least:\nLeft: %1\nRight: %2\nTop: %3\nBottom: %4")
		.arg(printerMargins.left()).arg(printerMargins.right()).arg(printerMargins.top()).arg(printerMargins.bottom()));
	}
#else
	printer.setPageMargins(leftPageMargin->value(), topPageMargin->value(), rightPageMargin->value(), bottomPageMargin->value(), QPrinter::Millimeter);
#endif
	QPrintPreviewDialog printPreviewSmall(&printer, this);
	connect(&printPreviewSmall, SIGNAL(paintRequested(QPrinter*)), SLOT(updatePreviewSmall(QPrinter*)));
	printPreviewSmall.exec();
#endif
}

void StatisticsPrintForm::updatePreviewSmall(QPrinter* printer){
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);

	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please export and open the HTML statistics from the results directory"));
#else
	QTextDocument textDocument;
	textDocument.documentLayout()->setPaintDevice(printer);
	textDocument.setPageSize(QSizeF(printer->pageRect().size()));
	textDocument.setHtml(updateHTMLprintString(false));
	textDocument.print(printer);
#endif
}
