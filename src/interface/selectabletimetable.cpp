#include "selectabletimetable.h"

#include <QHeaderView>

#include "tablewidgetupdatebug.h"

#define MARK		(QString("X"))
#define BLANK		(QString())

SelectableTimeTable::SelectableTimeTable(QWidget *parent)
	: QTableWidget(parent)
{
}

void SelectableTimeTable::setHeaders(const Rules &rules)
{
	setRowCount(rules.nHoursPerDay);
	setColumnCount(rules.nDaysPerWeek);

	for(int j=0; j<rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(rules.daysOfTheWeek[j]);
		setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(rules.hoursOfTheDay[i]);
		setVerticalHeaderItem(i, item);
	}

	for(int i=0; i<rules.nHoursPerDay; i++)
		for(int j=0; j<rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsEnabled);
			colorItem(item);
			if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
				item->setToolTip(rules.daysOfTheWeek[j]+QString("\n")+rules.hoursOfTheDay[i]);
			setItem(i, j, item);
		}


	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(horizontalHeaderClicked(int)));
	connect(verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(verticalHeaderClicked(int)));
	connect(verticalHeader(), SIGNAL(sectionPressed(int)), this, SLOT(verticalHeaderPressed(int)));
	connect(this, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));

	setSelectionMode(QAbstractItemView::NoSelection);

	tableWidgetUpdateBug(this);

	setStretchAvailabilityTableNicely(this);
}

void SelectableTimeTable::horizontalHeaderClicked(int col)
{
	if(col>=0 && col<columnCount()){
		bool marked = isMarked(0, col);

		for(int row=0; row<rowCount(); row++){
			setMarked(row, col, !marked);
		}
		tableWidgetUpdateBug(this);
	}
}

void SelectableTimeTable::verticalHeaderClicked(int clickedRow)
{
	if(clickedRow<0 && clickedRow>=rowCount())
		return;

	int firstRow, lastRow;
	if (pressedRow < clickedRow) {
		firstRow = pressedRow;
		lastRow = clickedRow;
	} else {
		lastRow = pressedRow;
		firstRow = clickedRow;
	}
	for (int row = firstRow; row <= lastRow; row++) {
		bool marked = isMarked(row, 0);

		for(int col=0; col<columnCount(); col++){
			setMarked(row, col, !marked);
		}
	}
	tableWidgetUpdateBug(this);
}

void SelectableTimeTable::verticalHeaderPressed(int row)
{
	if(row>=0 && row<rowCount()){
		pressedRow = row;
	}
}

void SelectableTimeTable::itemClicked(QTableWidgetItem* item)
{
	QString s=item->text() == MARK? BLANK : MARK;
	item->setText(s);
	colorItem(item);

	tableWidgetUpdateBug(this);
}

void SelectableTimeTable::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
		if(item->text()==BLANK)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
	}
}

void SelectableTimeTable::setMarked(int row, int col, bool isMarked)
{
	QTableWidgetItem *item = this->item(row, col);
	item->setText(isMarked ? MARK : BLANK);
	colorItem(item);
}

void SelectableTimeTable::setMarked(QTableWidgetItem *item, bool isMarked)
{
	item->setText(isMarked ? MARK : BLANK);
	colorItem(item);
}

bool SelectableTimeTable::isMarked(int row, int col) const
{
	return item(row, col)->text() == MARK;
}

void SelectableTimeTable::setAllUnmarked()
{
	for(int i=0; i<rowCount(); i++)
		for(int j=0; j<columnCount(); j++){
			setMarked(i, j, false);
		}
	tableWidgetUpdateBug(this);
}

void SelectableTimeTable::setAllMarked()
{
	for(int i=0; i<rowCount(); i++)
		for(int j=0; j<columnCount(); j++){
			setMarked(i, j, true);
		}
	tableWidgetUpdateBug(this);
}

