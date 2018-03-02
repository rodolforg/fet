#ifndef EDITABLETIMETABLEWIDGET_H
#define EDITABLETIMETABLEWIDGET_H

#include <QTableWidget>

#include "solution.h"

enum TimetableDirection {
	DAYS_HORIZONTAL,
	TIMES_HORIZONTAL
};

class EditableTimetableWidget : public QTableWidget
{
	Q_OBJECT

public:
	EditableTimetableWidget(QWidget* parent = NULL);
	EditableTimetableWidget(int rows, int cols, QWidget* parent = NULL);

	virtual ~EditableTimetableWidget();

	void setSolution(const Rules* rules, const Solution& solution);

	const Rules* getRules() const;
	const Solution& getSolution() const;

	TimetableDirection getTimetableDirection() const;
	void setTimetableDirection(const TimetableDirection& value);

signals:
	void solution_changed();

protected:
	//	virtual void mousePressEvent(QMouseEvent *event) override;
		virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dragMoveEvent(QDragMoveEvent *event) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
private:
	const Rules* rules;
	Solution* solution;
	TimetableDirection timetableDirection;

	int getTime(int row, int column) const;
	int getDay(int row, int column) const;
	int getHour(int row, int column) const;

	enum ProposalStatus {AN_IMPOSSIBLE_PROPOSAL, A_WORSE_PROPOSAL, QUID_PRO_QUO, A_BETTER_PROPOSAL};
	ProposalStatus checkSolutionChange(Solution &proposal) const;

protected slots:
	void itemPressed(QTableWidgetItem *item);
};

#endif // EDITABLETIMETABLEWIDGET_H
