#include "selectabletimetable.h"

#include <QHeaderView>
#include <QApplication>
#include <QKeyEvent>

#include "tablewidgetupdatebug.h"

class ChangeRowCommand : public QUndoCommand
{
public:
	ChangeRowCommand(SelectableTimeTable *table, int row, bool mark, QUndoCommand *parent = NULL);
	virtual void undo();
	virtual void redo();
private:
	SelectableTimeTable *tableWidget;
	int row;
	bool mark;
	QList<bool> rowData;
};

class ChangeColumnCommand : public QUndoCommand
{
public:
	ChangeColumnCommand(SelectableTimeTable *table, int column, bool mark, QUndoCommand *parent = NULL);
	virtual void undo();
	virtual void redo();
private:
	SelectableTimeTable *tableWidget;
	int column;
	bool mark;
	QList<bool> columnData;
};

class ToggleCellCommand : public QUndoCommand
{
public:
	ToggleCellCommand(SelectableTimeTable *table, int row, int column, QUndoCommand *parent = NULL);
	virtual void undo();
	virtual void redo();
private:
	void toggle();
	SelectableTimeTable *tableWidget;
	int row;
	int column;
};

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
		ChangeColumnCommand * command = new ChangeColumnCommand(this, col, !marked);
		undoStack.push(command);
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

	QUndoCommand *changeRows = new QUndoCommand();

	for (int row = firstRow; row <= lastRow; row++) {
		bool marked = isMarked(row, 0);
		new ChangeRowCommand(this, row, !marked, changeRows);
	}
	undoStack.push(changeRows);
}

void SelectableTimeTable::verticalHeaderPressed(int row)
{
	if(row>=0 && row<rowCount()){
		pressedRow = row;
	}
}

void SelectableTimeTable::itemClicked(QTableWidgetItem* item)
{
	undoStack.push(new ToggleCellCommand(this, item->row(), item->column()));
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

void SelectableTimeTable::keyPressEvent(QKeyEvent *event)
{
	if (event->matches(QKeySequence::Undo)) {
		if (!undoStack.canUndo())
			QApplication::beep();
		else
			undoStack.undo();
	}
	else if (event->matches(QKeySequence::Redo)) {
		if (!undoStack.canRedo())
			QApplication::beep();
		else
			undoStack.redo();
	}
	else
		QTableWidget::keyPressEvent(event);
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
	QUndoCommand *unmarkAll = new QUndoCommand();
	for(int j=0; j<columnCount(); j++){
		new ChangeColumnCommand(this, j, false, unmarkAll);
	}
	undoStack.push(unmarkAll);
}

void SelectableTimeTable::setAllMarked()
{
	QUndoCommand *markAll = new QUndoCommand();
	for(int j=0; j<columnCount(); j++){
		new ChangeColumnCommand(this, j, true, markAll);
	}
	undoStack.push(markAll);
}

ChangeRowCommand::ChangeRowCommand(SelectableTimeTable *table, int row, bool mark, QUndoCommand *parent)
	: QUndoCommand(parent), tableWidget(table), row(row), mark(mark)
{
}

void ChangeRowCommand::undo()
{
	int column = 0;
	foreach(bool mark, rowData){
		tableWidget->setMarked(row, column, mark);
		column++;
	}
	tableWidgetUpdateBug(tableWidget);
}

void ChangeRowCommand::redo()
{
	if (rowData.count() != tableWidget->columnCount()) {
		for(int col=0; col<tableWidget->columnCount(); col++){
			rowData << tableWidget->isMarked(row, col);
		}
	}
	for(int column=0; column<tableWidget->columnCount(); column++){
		tableWidget->setMarked(row, column, mark);
	}
	tableWidgetUpdateBug(tableWidget);
}

ChangeColumnCommand::ChangeColumnCommand(SelectableTimeTable *table, int column, bool mark, QUndoCommand *parent)
	: QUndoCommand(parent), tableWidget(table), column(column), mark(mark)
{
}

void ChangeColumnCommand::undo()
{
	int row = 0;
	foreach(bool mark, columnData){
		tableWidget->setMarked(row, column, mark);
		row++;
	}
	tableWidgetUpdateBug(tableWidget);
}

void ChangeColumnCommand::redo()
{
	if (columnData.count() != tableWidget->rowCount()) {
		for(int row=0; row<tableWidget->rowCount(); row++){
			columnData << tableWidget->isMarked(row, column);
		}
	}
	for(int row=0; row<tableWidget->rowCount(); row++){
		tableWidget->setMarked(row, column, mark);
	}
	tableWidgetUpdateBug(tableWidget);
}

ToggleCellCommand::ToggleCellCommand(SelectableTimeTable *table, int row, int column, QUndoCommand *parent)
	: QUndoCommand(parent), tableWidget(table), row(row), column(column)
{
}

void ToggleCellCommand::undo()
{
	toggle();
}

void ToggleCellCommand::redo()
{
	toggle();
}

void ToggleCellCommand::toggle()
{
	bool marked = tableWidget->isMarked(row, column);
	tableWidget->setMarked(row, column, !marked);
	tableWidgetUpdateBug(tableWidget);
}
