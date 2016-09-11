/***************************************************************************
                                FET
                          -------------------
   copyright             : (C) by Liviu Lalescu, Volker Dirr
    email                : Liviu Lalescu: see http://lalescu.ro/liviu/ , Volker Dirr: see http://www.timetabling.de/
 ***************************************************************************
                          timetableprintform.cpp  -  description
                             -------------------
    begin                : March 2010
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

//BE CAREFUL: work ONLY with INTERNAL data in this source!!!

//maybe TODO: maybe use only HTML level 1 instead of 3? advantage: a bit speedup. disadvantage: no coloring

#include <QtGlobal>

#include "timetableprintform.h"

#include "timetable.h"
#include "timetable_defs.h"
#include "timetableexport.h"
#include "solution.h"

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
extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubject;
extern QHash<int, int> hashActivityColorBySubjectAndStudents;
extern QList<int> activeHashActivityColorBySubjectAndStudents;

extern QString generationLocalizedTime;

extern const QString COMPANY;
extern const QString PROGRAM;

static int numberOfPlacedActivities1;

#ifdef QT_NO_PRINTER
static QMap<QString, int> paperSizesMap;
#else
static QMap<QString, QPrinter::PaperSize> paperSizesMap;
#endif

const QString CBTablesState="/timetables-combo-box-state";

const QString RBDaysHorizontalState="/days-horizontal-radio-button-state";
const QString RBDaysVerticalState="/days-vertical-radio-button-state";
const QString RBTimeHorizontalState="/time-horizontal-radio-button-state";
const QString RBTimeVerticalState="/time-vertical-radio-button-state";
//By Liviu Lalescu - not used anymore
//const QString CBDivideTimeAxisByDayState="/divide-time-axis-timetables-by-days";
const QString RBTimeHorizontalDayState="/time-horizontal-per-day-radio-button-state";
const QString RBTimeVerticalDayState="/time-vertical-per-day-radio-button-state";

const QString CBBreakState="/page-break-combo-box-state";
const QString CBWhiteSpaceState="/white-space-combo-box-state";
const QString CBprinterModeState="/printer-mode-combo-box-state";
const QString CBpaperSizeState="/paper-size-combo-box-state";
const QString CBorientationModeState="/orientation-mode-combo-box-state";

//const QString markNotAvailableState="/mark-not-available-check-box-state";
//const QString markBreakState="/mark-break-check-box-state";
//const QString printSameStartingTimeState="/print-same-starting-time-box-state";
const QString printDetailedTablesState="/print-detailed-tables-check-box-state";
const QString printActivityTagsState="/print-activity-tags-check-box-state";
const QString printRepeatNamesState="/print-repeat-names-check-box-state";
const QString printAutomaticColorsState="/print-automatic-colors-check-box-state";

const QString activitiesPaddingState="/activity-padding-spin-box-value-state";
const QString tablePaddingState="/table-padding-spin-box-value-state";
const QString fontSizeTableState="/font-size-spin-box-value-state";
const QString maxNamesState="/max-names-spin-box-value-state";
const QString leftPageMarginState="/left-page-margin-spin-box-value-state";
const QString topPageMarginState="/top-page-margin-spin-box-value-state";
const QString rightPageMarginState="/right-page-margin-spin-box-value-state";
const QString bottomPageMarginState="/bottom-page-margin-spin-box-value-state";

//by Liviu Lalescu - unused anymore
/*static bool dividePrefixSuffix(const QString& str, QString& left, QString& right)
{
	QStringList list=str.split("%1");
	assert(list.count()>=1);
	left=list.at(0);
	if(list.count()>=2){
		right=list.at(1);
		return true;
	}
	else{
		right=QString("");
		return false;
	}
}*/

StartTimetablePrint::StartTimetablePrint()
{
}

StartTimetablePrint::~StartTimetablePrint()
{
}

void StartTimetablePrint::startTimetablePrint(QWidget* parent)
{
	if(gt.rules.initialized
		&& students_schedule_ready
		&& teachers_schedule_ready
		&& rooms_schedule_ready
		&& gt.rules.nInternalTeachers==gt.rules.teachersList.count()
		&& gt.rules.nInternalRooms==gt.rules.roomsList.count()
		&& gt.rules.internalStructureComputed){
	
		//prepare calculation
		numberOfPlacedActivities1=0;
		int numberOfPlacedActivities2=0;
		TimetableExport::getNumberOfPlacedActivities(numberOfPlacedActivities1, numberOfPlacedActivities2);
		
		TimetablePrintForm tpfd(parent);
		tpfd.exec();

	} else {
		QMessageBox::warning(parent, tr("FET warning"),
		 tr("Printing is currently not possible, because you modified the dataset. Please generate a new timetable before printing."));
	}
}

// this is very similar to statisticsexport.cpp. so please also check there if you change something here!
TimetablePrintForm::TimetablePrintForm(QWidget *parent): QDialog(parent){
	this->setWindowTitle(tr("Print timetable dialog"));
	
	QHBoxLayout* wholeDialog=new QHBoxLayout(this);
	
	QVBoxLayout* leftDialog=new QVBoxLayout();

	QStringList timetableNames;
	timetableNames<<tr("Subgroups")
		<<tr("Groups")
		<<tr("Years")
		<<tr("Teachers")
		<<tr("Teachers Free Periods")
		<<tr("Rooms")
		<<tr("Subjects")
		<<tr("Activity Tags")
		<<tr("All activities")
		<<tr("Students Statistics")
		<<tr("Teachers Statistics");
	CBTables=new QComboBox();
	CBTables->addItems(timetableNames);

	namesList = new QListWidget();
	namesList->setSelectionMode(QAbstractItemView::MultiSelection);

	QHBoxLayout* selectUnselect=new QHBoxLayout();
	pbSelectAll=new QPushButton(tr("All", "Refers to a list of items, select all. Please keep translation short"));
	//pbSelectAll->setAutoDefault(false);
	pbUnselectAll=new QPushButton(tr("None", "Refers to a list of items, select none. Please keep translation short"));
	selectUnselect->addWidget(pbSelectAll);
	selectUnselect->addWidget(pbUnselectAll);

	leftDialog->addWidget(CBTables);
	leftDialog->addWidget(namesList);
	leftDialog->addLayout(selectUnselect);
	
	QVBoxLayout* rightDialog=new QVBoxLayout();
	
	/*QGroupBox**/ actionsBox=new QGroupBox(tr("Print"));
	QGridLayout* actionsBoxGrid=new QGridLayout();
	RBDaysHorizontal= new QRadioButton(tr("Days horizontal"));
	RBDaysVertical= new QRadioButton(tr("Days vertical"));
	RBTimeHorizontal= new QRadioButton(tr("Time horizontal"));
	RBTimeVertical= new QRadioButton(tr("Time vertical"));
	//By Liviu Lalescu - not used anymore
	//CBDivideTimeAxisByDay=new QCheckBox(tr("Divide by days", "Refers to dividing time axis timetables by days"));
	//CBDivideTimeAxisByDay->setChecked(false);
	RBTimeHorizontalDay= new QRadioButton(tr("Time horizontal per day"));
	RBTimeVerticalDay= new QRadioButton(tr("Time vertical per day"));

	actionsBoxGrid->addWidget(RBDaysHorizontal,0,0);
	actionsBoxGrid->addWidget(RBDaysVertical,0,1);
	actionsBoxGrid->addWidget(RBTimeHorizontal,1,0);
	actionsBoxGrid->addWidget(RBTimeVertical,1,1);
	//actionsBoxGrid->addWidget(CBDivideTimeAxisByDay, 2, 0, 1, 2);
	actionsBoxGrid->addWidget(RBTimeHorizontalDay,2,0);
	actionsBoxGrid->addWidget(RBTimeVerticalDay,2,1);
	RBDaysHorizontal->setChecked(true);
	//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
	actionsBox->setLayout(actionsBoxGrid);
	
	/*QGroupBox**/ optionsBox=new QGroupBox(tr("Options"));
	QGridLayout* optionsBoxGrid=new QGridLayout();
	
	QStringList breakStrings;
	//strings by Liviu Lalescu
	breakStrings<<tr("Page-break: none", "No page-break between timetables. Please keep translation short")
		<<tr("Page-break: always", "Page-break after each timetable. Please keep translation short")
		<<tr("Page-break: even", "Page-break after each even timetable. Please keep translation short");
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

	printDetailedTables=new QCheckBox(tr("Detailed tables"));
	printDetailedTables->setChecked(true);
	
	printActivityTags=new QCheckBox(tr("Activity tags"));
	printActivityTags->setChecked(false);
	
	repeatNames=new QCheckBox(tr("Repeat vertical names"));
	repeatNames->setChecked(false);
	
	automaticColors=new QCheckBox(tr("Colors"));
	automaticColors->setChecked(false);
	
	fontSizeTable=new QSpinBox;
	fontSizeTable->setRange(4, 20);
	fontSizeTable->setValue(8);

	/*QString str, left, right;
	str=tr("Font size: %1 pt");
	dividePrefixSuffix(str, left, right);
	fontSizeTable->setPrefix(left);
	fontSizeTable->setSuffix(right);*/
	
	QString s=tr("Font size: %1 pt", "pt means points for font size, when printing the timetable");
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
	//fontSizeTable->setSuffix(QString(" ")+tr("pt", "Means points for font size, when printing the timetable"));

	fontSizeTable->setSizePolicy(QSizePolicy::Expanding, fontSizeTable->sizePolicy().verticalPolicy());
	
	activitiesPadding=new QSpinBox;
	activitiesPadding->setRange(0, 25);
	activitiesPadding->setValue(0);

	/*str=tr("Activities padding: %1 px");
	dividePrefixSuffix(str, left, right);
	activitiesPadding->setPrefix(left);
	activitiesPadding->setSuffix(right);*/

	s=tr("Activities padding: %1 px", "px means pixels, when printing the timetable");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	activitiesPadding->setPrefix(prefix);
	activitiesPadding->setSuffix(suffix);
	//activitiesPadding->setPrefix(tr("Activities padding:")+QString(" "));
	//activitiesPadding->setSuffix(QString(" ")+tr("px", "Means pixels, when printing the timetable"));

	activitiesPadding->setSizePolicy(QSizePolicy::Expanding, activitiesPadding->sizePolicy().verticalPolicy());
	
	tablePadding=new QSpinBox;
	tablePadding->setRange(1, 99);
	tablePadding->setValue(1);

	/*str=tr("Space after table: +%1 px");
	dividePrefixSuffix(str, left, right);
	tablePadding->setPrefix(left);
	tablePadding->setSuffix(right);*/

	s=tr("Space after table: +%1 px", "px means pixels, when printing the timetable");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	tablePadding->setPrefix(prefix);
	tablePadding->setSuffix(suffix);
	//tablePadding->setPrefix(tr("Space after table:")+QString(" +"));
	//tablePadding->setSuffix(QString(" ")+tr("px", "Means pixels, when printing the timetable"));

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
	optionsBoxGrid->addWidget(printDetailedTables,6,1);
	
	optionsBoxGrid->addWidget(repeatNames,7,0);
	optionsBoxGrid->addWidget(automaticColors,7,1);

	optionsBox->setLayout(optionsBoxGrid);
	optionsBox->setSizePolicy(QSizePolicy::Expanding, optionsBox->sizePolicy().verticalPolicy());
	
// maybe TODO: be careful. the form is pretty full already!
// be careful: these are global settings, so it will also change html output setting?! so it need parameter in each function!
//	optionsBoxVertical->addWidget(markNotAvailable);
//	optionsBoxVertical->addWidget(markBreak);
//	optionsBoxVertical->addWidget(printSameStartingTime);
// maybe TODO: select font, select color, select them also for line 0-4!

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

	wholeDialog->addLayout(leftDialog);
	wholeDialog->addLayout(rightDialog);
	
	updateNamesList();
	
	connect(CBTables, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNamesList()));
	connect(pbSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
	connect(pbUnselectAll, SIGNAL(clicked()), this, SLOT(unselectAll()));
	connect(pbPrint, SIGNAL(clicked()), this, SLOT(print()));
	connect(pbPrintPreviewSmall, SIGNAL(clicked()), this, SLOT(printPreviewSmall()));
	connect(pbPrintPreviewFull, SIGNAL(clicked()), this, SLOT(printPreviewFull()));
	connect(pbClose, SIGNAL(clicked()), this, SLOT(close()));
	
	//connect(RBDaysHorizontal, SIGNAL(toggled(bool)), this, SLOT(updateCBDivideTimeAxisByDay()));
	//connect(RBDaysVertical, SIGNAL(toggled(bool)), this, SLOT(updateCBDivideTimeAxisByDay()));
	//connect(RBTimeHorizontal, SIGNAL(toggled(bool)), this, SLOT(updateCBDivideTimeAxisByDay()));
	//connect(RBTimeVertical, SIGNAL(toggled(bool)), this, SLOT(updateCBDivideTimeAxisByDay()));

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
	
	if(settings.contains(this->metaObject()->className()+CBTablesState))
		CBTables->setCurrentIndex(settings.value(this->metaObject()->className()+CBTablesState).toInt());
	
	if(settings.contains(this->metaObject()->className()+RBDaysHorizontalState))
		RBDaysHorizontal->setChecked(settings.value(this->metaObject()->className()+RBDaysHorizontalState).toBool());
	if(settings.contains(this->metaObject()->className()+RBDaysVerticalState))
		RBDaysVertical->setChecked(settings.value(this->metaObject()->className()+RBDaysVerticalState).toBool());
	if(settings.contains(this->metaObject()->className()+RBTimeHorizontalState))
		RBTimeHorizontal->setChecked(settings.value(this->metaObject()->className()+RBTimeHorizontalState).toBool());
	if(settings.contains(this->metaObject()->className()+RBTimeVerticalState))
		RBTimeVertical->setChecked(settings.value(this->metaObject()->className()+RBTimeVerticalState).toBool());
	//if(settings.contains(this->metaObject()->className()+CBDivideTimeAxisByDayState))
	//	CBDivideTimeAxisByDay->setChecked(settings.value(this->metaObject()->className()+CBDivideTimeAxisByDayState).toBool());
	if(settings.contains(this->metaObject()->className()+RBTimeHorizontalDayState))
		RBTimeHorizontalDay->setChecked(settings.value(this->metaObject()->className()+RBTimeHorizontalDayState).toBool());
	if(settings.contains(this->metaObject()->className()+RBTimeVerticalDayState))
		RBTimeVerticalDay->setChecked(settings.value(this->metaObject()->className()+RBTimeVerticalDayState).toBool());
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
	//
		//if(settings.contains(this->metaObject()->className()+markNotAvailableState))
	//	markNotAvailable->setChecked(settings.value(this->metaObject()->className()+markNotAvailableState).toBool());
			//if(settings.contains(this->metaObject()->className()+markBreakState))
	//	markBreak->setChecked(settings.value(this->metaObject()->className()+markBreakState).toBool());
			//if(settings.contains(this->metaObject()->className()+printSameStartingTimeState))
	//	printSameStartingTime->setChecked(settings.value(this->metaObject()->className()+printSameStartingTimeState).toBool());
	if(settings.contains(this->metaObject()->className()+printDetailedTablesState))
		printDetailedTables->setChecked(settings.value(this->metaObject()->className()+printDetailedTablesState).toBool());
	if(settings.contains(this->metaObject()->className()+printActivityTagsState))
		printActivityTags->setChecked(settings.value(this->metaObject()->className()+printActivityTagsState).toBool());
	if(settings.contains(this->metaObject()->className()+printRepeatNamesState))
		repeatNames->setChecked(settings.value(this->metaObject()->className()+printRepeatNamesState).toBool());
	if(settings.contains(this->metaObject()->className()+printAutomaticColorsState))
		automaticColors->setChecked(settings.value(this->metaObject()->className()+printAutomaticColorsState).toBool());
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

TimetablePrintForm::~TimetablePrintForm(){
	saveFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
	//save other settings
	settings.setValue(this->metaObject()->className()+CBTablesState, CBTables->currentIndex());
	
	settings.setValue(this->metaObject()->className()+RBDaysHorizontalState, RBDaysHorizontal->isChecked());
	settings.setValue(this->metaObject()->className()+RBDaysVerticalState, RBDaysVertical->isChecked());
	settings.setValue(this->metaObject()->className()+RBTimeHorizontalState, RBTimeHorizontal->isChecked());
	settings.setValue(this->metaObject()->className()+RBTimeVerticalState, RBTimeVertical->isChecked());
	//settings.setValue(this->metaObject()->className()+CBDivideTimeAxisByDayState, CBDivideTimeAxisByDay->isChecked());
	settings.setValue(this->metaObject()->className()+RBTimeHorizontalDayState, RBTimeHorizontalDay->isChecked());
	settings.setValue(this->metaObject()->className()+RBTimeVerticalDayState, RBTimeVerticalDay->isChecked());
	//
	settings.setValue(this->metaObject()->className()+CBBreakState, CBBreak->currentIndex());
	settings.setValue(this->metaObject()->className()+CBWhiteSpaceState, CBWhiteSpace->currentIndex());
	//settings.setValue(this->metaObject()->className()+CBprinterModeState, CBprinterMode->currentIndex());
	settings.setValue(this->metaObject()->className()+CBpaperSizeState, CBpaperSize->currentIndex());
	settings.setValue(this->metaObject()->className()+CBorientationModeState, CBorientationMode->currentIndex());
	//
	//settings.setValue(this->metaObject()->className()+markNotAvailableState, markNotAvailable->isChecked());
	//settings.setValue(this->metaObject()->className()+markBreakState, markBreak->isChecked());
	//settings.setValue(this->metaObject()->className()+printSameStartingTimeState, printSameStartingTime->isChecked());
	settings.setValue(this->metaObject()->className()+printDetailedTablesState, printDetailedTables->isChecked());
	settings.setValue(this->metaObject()->className()+printActivityTagsState, printActivityTags->isChecked());
	settings.setValue(this->metaObject()->className()+printRepeatNamesState, repeatNames->isChecked());
	settings.setValue(this->metaObject()->className()+printAutomaticColorsState, automaticColors->isChecked());
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

void TimetablePrintForm::selectAll(){
	namesList->selectAll();
}

void TimetablePrintForm::unselectAll(){
	namesList->clearSelection();
}

void TimetablePrintForm::updateNamesList(){
	namesList->clear();
	
	if(CBTables->currentIndex()==0){
		for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
			QString name = gt.rules.internalSubgroupsList[subgroup]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(subgroup);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==1){
		for(int group=0; group<gt.rules.internalGroupsList.size(); group++){
			QString name = gt.rules.internalGroupsList[group]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(group);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(false); //this one is changed
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==2){
		for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
			QString name = gt.rules.augmentedYearsList[year]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(year);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(false); //this one is changed
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==3){
		for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
			QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
			namesList->addItem(teacher_name);
			QListWidgetItem* tmpItem=namesList->item(teacher);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==4){
		QString name = tr("All teachers");
		namesList->addItem(name);
		QListWidgetItem* tmpItem=namesList->item(0);
		tmpItem->setSelected(true);

		printActivityTags->setDisabled(true);
		printDetailedTables->setDisabled(false);
	
		if(!RBDaysVertical->isChecked())
			RBDaysHorizontal->setChecked(true);

		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(true);
		RBTimeVertical->setDisabled(true);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(true);
		RBTimeVerticalDay->setDisabled(true);
	} else if(CBTables->currentIndex()==5){
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString name = gt.rules.internalRoomsList[room]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(room);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==6){
		for(int subject=0; subject<gt.rules.nInternalSubjects; subject++){
			QString name = gt.rules.internalSubjectsList[subject]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(subject);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==7){
		for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags; activityTag++){
			QString name = gt.rules.internalActivityTagsList[activityTag]->name;
			namesList->addItem(name);
			QListWidgetItem* tmpItem=namesList->item(activityTag);
			tmpItem->setSelected(true);
		}
		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==8){
		QString name = tr("All activities");
		namesList->addItem(name);
		QListWidgetItem* tmpItem=namesList->item(0);
		tmpItem->setSelected(true);

		printActivityTags->setDisabled(false);
		printDetailedTables->setDisabled(true);
	
		RBDaysHorizontal->setDisabled(false);
		RBDaysVertical->setDisabled(false);
		RBTimeHorizontal->setDisabled(false);
		RBTimeVertical->setDisabled(false);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(false);
		RBTimeVerticalDay->setDisabled(false);
	} else if(CBTables->currentIndex()==9){
		QString name = tr("All activities");
		namesList->addItem(name);
		QListWidgetItem* tmpItem=namesList->item(0);
		tmpItem->setSelected(true);

		printActivityTags->setDisabled(true);
		printDetailedTables->setDisabled(false);
		
		if(!RBDaysVertical->isChecked())
			RBDaysHorizontal->setChecked(true);
	
		RBDaysHorizontal->setDisabled(true);
		RBDaysVertical->setDisabled(true);
		RBTimeHorizontal->setDisabled(true);
		RBTimeVertical->setDisabled(true);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(true);
		RBTimeVerticalDay->setDisabled(true);
	} else if(CBTables->currentIndex()==10){
		QString name = tr("All activities");
		namesList->addItem(name);
		QListWidgetItem* tmpItem=namesList->item(0);
		tmpItem->setSelected(true);

		printActivityTags->setDisabled(true);
		printDetailedTables->setDisabled(false);
		
		if(!RBDaysVertical->isChecked())
			RBDaysHorizontal->setChecked(true);
	
		RBDaysHorizontal->setDisabled(true);
		RBDaysVertical->setDisabled(true);
		RBTimeHorizontal->setDisabled(true);
		RBTimeVertical->setDisabled(true);
		//CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
		RBTimeHorizontalDay->setDisabled(true);
		RBTimeVerticalDay->setDisabled(true);
	} else assert(0==1);
}

QString TimetablePrintForm::updateHtmlPrintString(bool printAll){
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
		tmp+="        font-size: "+QString::number(tablePadding->value())+"pt;\n";	//i can't do that in table, because it will also affect detailed table cells. it is not possible with a class, because of a qt bug.
	else
		tmp+="        font-size: 1pt;\n";	//font size 0 is not possible.
//	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
//	tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
	if(CBBreak->currentIndex()==1 || CBBreak->currentIndex()==2)
		tmp+="        page-break-after: always;";
	tmp+="      }\n";
	tmp+="      p.back1 {\n";	//i can't to that with a class in table, because of a qt bug
	if(CBBreak->currentIndex()==0 || CBBreak->currentIndex()==2)
		tmp+="        font-size: "+QString::number(tablePadding->value())+"pt;\n";	//i can't do that in table, because it will also affect detailed table cells. it is not possible with a class, because of a qt bug.
	else
		tmp+="        font-size: 1pt;\n";	//font size 0 is not possible.
//	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
//	tmp+="        padding-bottom: "+QString::number(tablePadding->value())+"px;\n";	//not possible: qt bug.
	if(CBBreak->currentIndex()==1)
		tmp+="        page-break-after: always;";
	tmp+="      }\n";
	//end. the "back" stuff is only needed because of a qt bug (*1*). delete this as soon as bug is solved
	
	int htmlLevel=3;
	if(automaticColors->isChecked()){
		htmlLevel=7;
	}
	if(htmlLevel==7){	 // must be written before LEVEL 3, because LEVEL 3 should have higher priority
		TimetableExport::computeHashActivityColorBySubject();
		TimetableExport::computeHashActivityColorBySubjectAndStudents();
		
		int cnt=0;
		foreach(int i, activeHashActivityColorBySubject){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName;
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			TimetableExport::stringToColor(tmpString, &r, &g, &b);
			tmp += "td.c_"+QString::number(cnt+1)+" { /* Activity id: "+QString::number(act->id)+" (subject) */\n ";
			tmp+="background-color: rgb("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+");\n";
			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
			if (brightness<127.5)
				tmp+=" color: white;\n";
			else
				tmp+=" color: black;\n";
			tmp+="}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
		foreach(int i, activeHashActivityColorBySubjectAndStudents){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName+" "+act->studentsNames.join(" ,");
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			TimetableExport::stringToColor(tmpString, &r, &g, &b);
			tmp += "td.c_"+QString::number(cnt+1)+" { /* Activity id: "+QString::number(act->id)+" (subject+students) */\n ";
			tmp+="background-color: rgb("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+");\n";
			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
			if (brightness<127.5)
				tmp+=" color: white;\n";
			else
				tmp+=" color: black;\n";
			tmp+="}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
		
//		QHashIterator<QString, QString> i(hashColorStringIDsTimetable);
//		qWarning("prepare css file");
//		while(i.hasNext()) {
//			qWarning("add color in css file");
//			i.next();
//			tmp+="td.c_"+i.value()+" { /* "+i.key()+" */\n ";
//			
//			//similar to the coloring by Marco Vassura (start)
//			int r,g,b;
//			TimetableExport::stringToColor(i.key(), &r, &g, &b);
//			tmp+="background-color: rgb("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+");\n";
//			double brightness = (double)r*0.299 + (double)g*0.587 + (double)b*0.114;
//			if (brightness<127.5)
//				tmp+=" color: white;\n";
//			else
//				tmp+=" color: black;\n";
//			//similar to the coloring by Marco Vassura (end)
//			tmp+="}\n\n";
//		}
	}

	tmp+="      table {\n";
	tmp+="        font-size: "+QString::number(fontSizeTable->value())+"pt;\n";
	tmp+="        padding-top: "+QString::number(tablePadding->value())+"px;\n";
	tmp+="        page-break-inside: avoid;\n";
	tmp+="      }\n";
	tmp+="      th {\n";
	tmp+="        text-align: center;\n"; //currently no effect because of a qt bug (compare http://bugreports.qt.nokia.com/browse/QTBUG-2730 )
	tmp+="        vertical-align: middle;\n";
	tmp+="        white-space: "+CBWhiteSpace->currentText()+";\n";
	tmp+="      }\n";
	tmp+="      td {\n";
	tmp+="        text-align: center;\n"; //currently no effect because of a qt bug (compare http://bugreports.qt.nokia.com/browse/QTBUG-2730 )
	tmp+="        vertical-align: middle;\n";
	tmp+="        white-space: "+CBWhiteSpace->currentText()+";\n";
	tmp+="        padding-left: "+QString::number(activitiesPadding->value())+"px;\n";
	tmp+="        padding-right: "+QString::number(activitiesPadding->value())+"px;\n";
	tmp+="      }\n";
	tmp+="      td.detailed {\n";
//	tmp+="        padding-left: 4px;\n";
//	tmp+="        padding-right: 4px;\n";
	tmp+="      }\n";
	tmp+="      th.xAxis {\n";	//needs level 2
//	tmp+="        padding-left: 4px;\n";
//	tmp+="        padding-right: 4px;\n";
	tmp+="      }\n";
	tmp+="      th.yAxis {\n";	//needs level 2
//	tmp+="        padding-top: 4px;\n";
//	tmp+="        padding-bottom: 4px;\n";
	tmp+="      }\n";
	tmp+="      tr.line0, div.line0 {\n";	//needs level 3
	tmp+="        /*font-size: 12pt;*/\n";
	if(htmlLevel!=7){
		tmp+="        color: gray;\n";
	}
	tmp+="      }\n";
	tmp+="      tr.line1, div.line1 {\n";	//needs level 3
	tmp+="        /*font-size: 12pt;*/\n";
	tmp+="      }\n";
	tmp+="      tr.line2, div.line2 {\n";	//needs level 3
	tmp+="        /*font-size: 12pt;*/\n";
	if(htmlLevel!=7){
		tmp+="        color: gray;\n";
	}
	tmp+="      }\n";
	tmp+="      tr.line3, div.line3 {\n";	//needs level 3
	tmp+="        /*font-size: 12pt;*/\n";
	if(htmlLevel!=7){
		tmp+="        color: silver;\n";
	}
	tmp+="      }\n";
	tmp+="    </style>\n";
	tmp+="  </head>\n\n";
	tmp+="  <body id=\"top\">\n";

	if(numberOfPlacedActivities1!=gt.rules.nInternalActivities)
		tmp+="    <h1>"+tr("Warning! Only %1 out of %2 activities placed!").arg(numberOfPlacedActivities1).arg(gt.rules.nInternalActivities)+"</h1>\n";

	QList<int> includedNamesIndex;
	QSet<int> excludedNamesIndex;
	for(int nameIndex=0; nameIndex<namesList->count(); nameIndex++){
		QListWidgetItem* tmpItem=namesList->item(nameIndex);
		if(tmpItem->isSelected()){
			includedNamesIndex<<nameIndex;
		} else {
			excludedNamesIndex<<nameIndex;
		}
	}
	
	//maybe TODO: do the pagebreak similar in timetableexport. (so remove the odd and even table tag and use only back1 and back2 (maybe rename to odd and even))
	//            check the GroupsTimetableDaysHorizontalHtml and Year parameter then (iNi%2) isn't needed anymore then.
	if(RBDaysHorizontal->isChecked()){
		for(int iNi=0; iNi<includedNamesIndex.size(); iNi++){
			switch(CBTables->currentIndex()){
				case 0: tmp+=TimetableExport::singleSubgroupsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 1: tmp+=TimetableExport::singleGroupsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 2: tmp+=TimetableExport::singleYearsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 3: tmp+=TimetableExport::singleTeachersTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 4: tmp+=TimetableExport::singleTeachersFreePeriodsTimetableDaysHorizontalHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 5: tmp+=TimetableExport::singleRoomsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 6: tmp+=TimetableExport::singleSubjectsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 7: tmp+=TimetableExport::singleActivityTagsTimetableDaysHorizontalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 8: tmp+=TimetableExport::singleAllActivitiesTimetableDaysHorizontalHtml(htmlLevel, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 9: tmp+=TimetableExport::singleStudentsStatisticsHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked(), printAll); break;
				case 10: tmp+=TimetableExport::singleTeachersStatisticsHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked(), printAll); break;
				default: assert(0==1);
			}
			if(iNi<includedNamesIndex.size()-1){
				if(iNi%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
			}
		}
	}
	if(RBDaysVertical->isChecked()){
		for(int iNi=0; iNi<includedNamesIndex.size(); iNi++){
			switch(CBTables->currentIndex()){
				case 0: tmp+=TimetableExport::singleSubgroupsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 1: tmp+=TimetableExport::singleGroupsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 2: tmp+=TimetableExport::singleYearsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 3: tmp+=TimetableExport::singleTeachersTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 4: tmp+=TimetableExport::singleTeachersFreePeriodsTimetableDaysVerticalHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 5: tmp+=TimetableExport::singleRoomsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 6: tmp+=TimetableExport::singleSubjectsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 7: tmp+=TimetableExport::singleActivityTagsTimetableDaysVerticalHtml(htmlLevel, includedNamesIndex.at(iNi), saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 8: tmp+=TimetableExport::singleAllActivitiesTimetableDaysVerticalHtml(htmlLevel, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				default: assert(0==1);
			}
			if(iNi<includedNamesIndex.size()-1){
				if(iNi%2==0){
					tmp+="    <p class=\"back1\"><br /></p>\n\n";
				} else {
					if(!printAll) break;
					tmp+="    <p class=\"back0\"><br /></p>\n\n";
				}
			}
		}
	}
	if(RBTimeHorizontal->isChecked() /*&& !CBDivideTimeAxisByDay->isChecked()*/){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			switch(CBTables->currentIndex()){
				case 0: tmp+=TimetableExport::singleSubgroupsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 1: tmp+=TimetableExport::singleGroupsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 2: tmp+=TimetableExport::singleYearsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 3: tmp+=TimetableExport::singleTeachersTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 4: /*tmp+=TimetableExport::singleTeachersFreePeriodsTimetableTimeHorizontalHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked());*/ break;
				case 5: tmp+=TimetableExport::singleRoomsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 6: tmp+=TimetableExport::singleSubjectsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 7: tmp+=TimetableExport::singleActivityTagsTimetableTimeHorizontalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 8: tmp+=TimetableExport::singleAllActivitiesTimetableTimeHorizontalHtml(htmlLevel, saveTime, printActivityTags->isChecked(), repeatNames->isChecked());
						excludedNamesIndex<<-1; break;
				default: assert(0==1);
			}
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
	if(RBTimeVertical->isChecked() /*&& !CBDivideTimeAxisByDay->isChecked()*/){
		int count=0;
		while(excludedNamesIndex.size()<namesList->count()){
			switch(CBTables->currentIndex()){
				case 0: tmp+=TimetableExport::singleSubgroupsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 1: tmp+=TimetableExport::singleGroupsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 2: tmp+=TimetableExport::singleYearsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
				case 3: tmp+=TimetableExport::singleTeachersTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 4: /*tmp+=TimetableExport::singleTeachersFreePeriodsTimetableTimeVerticalHtml(htmlLevel, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked());*/ break;
				case 5: tmp+=TimetableExport::singleRoomsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 6: tmp+=TimetableExport::singleSubjectsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 7: tmp+=TimetableExport::singleActivityTagsTimetableTimeVerticalHtml(htmlLevel, maxNames->value(), excludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
				case 8: tmp+=TimetableExport::singleAllActivitiesTimetableTimeVerticalHtml(htmlLevel, saveTime, printActivityTags->isChecked(), repeatNames->isChecked());
						excludedNamesIndex<<-1; break;
				default: assert(0==1);
			}
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
	if(RBTimeHorizontalDay->isChecked() /*&& CBDivideTimeAxisByDay->isChecked()*/){
		int count=0;
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QSet<int> tmpExcludedNamesIndex;
			tmpExcludedNamesIndex=excludedNamesIndex;
			while(tmpExcludedNamesIndex.size()<namesList->count()){
				switch(CBTables->currentIndex()){
					case 0: tmp+=TimetableExport::singleSubgroupsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 1: tmp+=TimetableExport::singleGroupsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
					case 2: tmp+=TimetableExport::singleYearsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
					case 3: tmp+=TimetableExport::singleTeachersTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 4: /*tmp+=TimetableExport::singleTeachersFreePeriodsTimetableTimeHorizontalDailyHtml(htmlLevel, day, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked());*/ break;
					case 5: tmp+=TimetableExport::singleRoomsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 6: tmp+=TimetableExport::singleSubjectsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 7: tmp+=TimetableExport::singleActivityTagsTimetableTimeHorizontalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 8: tmp+=TimetableExport::singleAllActivitiesTimetableTimeHorizontalDailyHtml(htmlLevel, day, saveTime, printActivityTags->isChecked(), repeatNames->isChecked());
							tmpExcludedNamesIndex<<-1; break;
					default: assert(0==1);
				}
				if(!(tmpExcludedNamesIndex.size()==namesList->count() && day==gt.rules.nDaysPerWeek-1)){
					if(count%2==0){
						tmp+="    <p class=\"back1\"><br /></p>\n\n";
					} else {
						if(!printAll) break;
						tmp+="    <p class=\"back0\"><br /></p>\n\n";
					}
					count++;
				}
			}
			if(!printAll) break;
		}
	}
	if(RBTimeVerticalDay->isChecked() /*&& CBDivideTimeAxisByDay->isChecked()*/){
		int count=0;
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			QSet<int> tmpExcludedNamesIndex;
			tmpExcludedNamesIndex=excludedNamesIndex;
			while(tmpExcludedNamesIndex.size()<namesList->count()){
				switch(CBTables->currentIndex()){
					case 0: tmp+=TimetableExport::singleSubgroupsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 1: tmp+=TimetableExport::singleGroupsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
					case 2: tmp+=TimetableExport::singleYearsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), printDetailedTables->isChecked(), repeatNames->isChecked()); break;
					case 3: tmp+=TimetableExport::singleTeachersTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 4: /*tmp+=TimetableExport::singleTeachersFreePeriodsTimetableTimeVerticalDailyHtml(htmlLevel, day, saveTime, printDetailedTables->isChecked(), repeatNames->isChecked());*/ break;
					case 5: tmp+=TimetableExport::singleRoomsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 6: tmp+=TimetableExport::singleSubjectsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 7: tmp+=TimetableExport::singleActivityTagsTimetableTimeVerticalDailyHtml(htmlLevel, day, maxNames->value(), tmpExcludedNamesIndex, saveTime, printActivityTags->isChecked(), repeatNames->isChecked()); break;
					case 8: tmp+=TimetableExport::singleAllActivitiesTimetableTimeVerticalDailyHtml(htmlLevel, day, saveTime, printActivityTags->isChecked(), repeatNames->isChecked());
							tmpExcludedNamesIndex<<-1; break;
					default: assert(0==1);
				}
				if(!(tmpExcludedNamesIndex.size()==namesList->count() && day==gt.rules.nDaysPerWeek-1)){
					if(count%2==0){
						tmp+="    <p class=\"back1\"><br /></p>\n\n";
					} else {
						if(!printAll) break;
						tmp+="    <p class=\"back0\"><br /></p>\n\n";
					}
					count++;
				}
			}
			if(!printAll) break;
		}
	}
	// end
	
	tmp+="  </body>\n";
	tmp+="</html>\n\n";
	
	return tmp;
}

/*void TimetablePrintForm::updateCBDivideTimeAxisByDay()
{
	CBDivideTimeAxisByDay->setDisabled(RBDaysHorizontal->isChecked() || RBDaysVertical->isChecked());
}*/

void TimetablePrintForm::print(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please open the HTML timetables from the results directory"));
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
	printDialog.setWindowTitle(tr("Print timetable"));
	if (printDialog.exec() == QDialog::Accepted) {
		QTextDocument textDocument;
		textDocument.documentLayout()->setPaintDevice(&printer);
		textDocument.setPageSize(QSizeF(printer.pageRect().size()));
		textDocument.setHtml(updateHtmlPrintString(true));
		textDocument.print(&printer);
	}
	//delete printDialog;
#endif
}

void TimetablePrintForm::printPreviewFull(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please open the HTML timetables from the results directory"));
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

void TimetablePrintForm::updatePreviewFull(QPrinter* printer){
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);

	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please open the HTML timetables from the results directory"));
#else
	QTextDocument textDocument;
	textDocument.documentLayout()->setPaintDevice(printer);
	textDocument.setPageSize(QSizeF(printer->pageRect().size()));
	textDocument.setHtml(updateHtmlPrintString(true));
	textDocument.print(printer);
#endif
}

void TimetablePrintForm::printPreviewSmall(){
#ifdef QT_NO_PRINTER
	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please open the HTML timetables from the results directory"));
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

void TimetablePrintForm::updatePreviewSmall(QPrinter* printer){
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);

	QMessageBox::warning(this, tr("FET warning"), tr("FET is compiled without printer support "
	 "- it is impossible to print from this dialog. Please open the HTML timetables from the results directory"));
#else
	QTextDocument textDocument;
	textDocument.documentLayout()->setPaintDevice(printer);
	textDocument.setPageSize(QSizeF(printer->pageRect().size()));
	textDocument.setHtml(updateHtmlPrintString(false));
	textDocument.print(printer);
#endif
}
