/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef EDITABLETIMETABLEWIDGET_H
#define EDITABLETIMETABLEWIDGET_H

#include <QTableWidget>

#include "solution.h"

#include <QUndoStack>

enum TimetableDirection {
	DAYS_HORIZONTAL,
	TIMES_HORIZONTAL
};

class EditableTimetableWidget : public QTableWidget
{
	Q_OBJECT

public:
	EditableTimetableWidget(QWidget* parent = nullptr);
	EditableTimetableWidget(int rows, int cols, QWidget* parent = nullptr);

	virtual ~EditableTimetableWidget() override;

	void setSolution(const Rules* rules, const Solution& solution);

	void addNotPlacedActivity(int ai, int row);
	void clearNotPlacedActivities();

	const Rules* getRules() const;
	const Solution& getSolution() const;

	TimetableDirection getTimetableDirection() const;
	void setTimetableDirection(const TimetableDirection& value);

	void colorizeWherePlaceable(int ai, int row);
signals:
	void solution_changed();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event) override;

//	virtual void mousePressEvent(QMouseEvent *event) override;
//	virtual void dragEnterEvent(QDragEnterEvent *event) override;
//	virtual void dragMoveEvent(QDragMoveEvent *event) override;
//	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
//	virtual void dropEvent(QDropEvent* event) override;
private:
	const Rules* rules;
	Solution* solution;
	TimetableDirection timetableDirection;

	int getTime(int row, int column) const;
	int getDay(int row, int column) const;
	int getHour(int row, int column) const;

	enum ProposalStatus {AN_IMPOSSIBLE_PROPOSAL, A_WORSE_PROPOSAL, QUID_PRO_QUO, A_BETTER_PROPOSAL};
	ProposalStatus checkSolutionChange(Solution &proposal) const;

	QSet<int> getPlacedActivities(const QTableWidgetItem* item) const;
	QSet<int> getRemovedActivities(const QTableWidgetItem* item) const;
	QList<int> getPossibleSwaps(const QList<int>& activity_ids, int src_ai) const;

	QList<QSet<int>> tempRemovedActivityCollection;

	QUndoStack undoStack;

	void colorizePossibleActions(QTableWidgetItem *item);

	bool isActivityLocked(int ai) const;

	bool activityCanBePlacedAt(int ai, int time, int row) const;

protected slots:
	void placeActivity(const QTableWidgetItem* item, int ai);
	void removeActivity(const QTableWidgetItem* item);
	void swapActivity(const QTableWidgetItem* item, int ai);

signals:
	void activityRemoved(int ai);
	void activityPlaced(int ai);
};

#endif // EDITABLETIMETABLEWIDGET_H
