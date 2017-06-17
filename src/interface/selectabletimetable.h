#ifndef SELECTABLETIMETABLE_H
#define SELECTABLETIMETABLE_H

#include <QTableWidget>
#include <QUndoStack>
#include "rules.h"

class SelectableTimeTable : public QTableWidget
{
	Q_OBJECT
public:
	SelectableTimeTable(QWidget *parent);

	void setHeaders(const Rules &rules);
	void setAllUnmarked();
	void setAllMarked();

	void setMarked(int row, int col, bool isMarked);
	bool isMarked(int row, int col) const;

public slots:
	void verticalHeaderPressed(int row);
	void verticalHeaderClicked(int clickedRow);
	void horizontalHeaderClicked(int col);
	void itemClicked(QTableWidgetItem *item);
	void itemPressed(QTableWidgetItem *item);
	void contiguousCellsSelected(QTableWidgetItem *itemOnRelease);

private:
	int pressedRow;
	int pressedColumn;

	void setMarked(QTableWidgetItem *item, bool isMarked);
	void colorItem(QTableWidgetItem *item);

	virtual void keyPressEvent(QKeyEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	QUndoStack undoStack;
};

#endif // SELECTABLETIMETABLE_H
