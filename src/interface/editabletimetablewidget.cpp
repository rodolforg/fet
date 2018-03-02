#include "editabletimetablewidget.h"
#include <QMouseEvent>
#include <QMimeData>
#include "rules.h"

#include "timetableexport.h"


extern bool simulation_running;
//#include "lockunlock.h"
//extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

EditableTimetableWidget::EditableTimetableWidget(QWidget *parent)
	: QTableWidget(parent), rules(NULL), solution(NULL), timetableDirection(DAYS_HORIZONTAL)
{
	connect(this, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(itemPressed(QTableWidgetItem*)));
//	connect(&communicationSpinBox, SIGNAL(valueChanged()), this, solution_update());
}

EditableTimetableWidget::EditableTimetableWidget(int rows, int cols, QWidget* parent)
	: QTableWidget(rows, cols, parent), rules(NULL), solution(NULL), timetableDirection(DAYS_HORIZONTAL)
{
	connect(this, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(itemPressed(QTableWidgetItem*)));
}

EditableTimetableWidget::~EditableTimetableWidget()
{
	delete solution;
}

void EditableTimetableWidget::setSolution(const Rules* rules, const Solution& solution)
{
	this->rules = rules;
	if (this->solution == NULL)
		this->solution = new Solution(*rules);
	this->solution->copy(*rules, solution);
}

//void EditableTimetableWidget::mousePressEvent(QMouseEvent* event)
//{
//	if (rules == NULL || solution == NULL) {
//		event->ignore();
//		return;
//	}
//	QTableWidgetItem *item = itemAt(event->pos());
//	if (!item) {
//		QTableWidget::mousePressEvent(event);
//		return;
//	}

////	QByteArray itemData;
////	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
////	dataStream << item;

////	QMimeData *mimeData = new QMimeData;
////	mimeData->setData("application/x-fet-timetable-item", itemData);
////	mimeData->setText(QString("%1, %2").arg(item->row()).arg(item->column()));
//	QTableWidget::mousePressEvent(event);
//}
#include <QDebug>
void EditableTimetableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	qWarning() << "enter drag";
	if (rules == NULL || simulation_running) {
		event->ignore();
		return;
	}
	if (event->source() != this) {
		event->ignore();
		return;
	}
	if (! event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->ignore();
		return;
	}

	int startRow = 0;
	int endRow = rowCount();

	QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	while (!stream.atEnd())
	{
		int src_row, src_column;
		QMap<int,  QVariant> roleDataMap;
		stream >> src_row >> src_column >> roleDataMap;

		const int src_ai = roleDataMap.value(Qt::UserRole).toInt();
		const int src_time = solution->time(src_ai);

		const int act_duration = rules->internalActivitiesList[src_ai].duration;

		if (timetableDirection == TIMES_HORIZONTAL) {
			startRow = src_row;
			endRow = startRow;
		}

		for (int r=startRow; r < endRow; r++) {
			for (int c=0; c < columnCount(); c++) {

				QTableWidgetItem *item = this->item(r, c);
				const int intended_h0 = getHour(r, c);
				const int intended_time = getTime(r,c);

				if (intended_h0 + act_duration > rules->nHoursPerDay) {
					QBrush brush = item->background();
					brush.setStyle(Qt::SolidPattern);
					item->setBackground(brush);

					continue;
				}

				const int dst_ai = item->data(Qt::UserRole).toInt();

				Solution proposed_solution(*rules);
				proposed_solution.copy(*rules, *solution);
				proposed_solution.resetFitness();

				proposed_solution.setTime(src_ai, intended_time);
				if (dst_ai != UNALLOCATED_ACTIVITY) {
					if (src_h0 + dst_act->duration > rules->nHoursPerDay) {
						QBrush brush = item->background();
						brush.setStyle(Qt::SolidPattern);
						item->setBackground(brush);

						continue;
					}
					proposed_solution.setTime(dst_ai, src_time);
				}

				ProposalStatus status = checkSolutionChange(proposed_solution);
				Qt::BrushStyle brushStyle = Qt::NoBrush;
				switch (status) {
				case EditableTimetableWidget::AN_IMPOSSIBLE_PROPOSAL:
					brushStyle = Qt::SolidPattern;
					break;
				case EditableTimetableWidget::A_WORSE_PROPOSAL:
					brushStyle = Qt::Dense1Pattern;
					break;
				case EditableTimetableWidget::QUID_PRO_QUO:
					brushStyle = Qt::Dense3Pattern;
					break;
				case EditableTimetableWidget::A_BETTER_PROPOSAL:
					brushStyle = Qt::Dense4Pattern;
					break;
				default:
					assert(0);
				}
				QBrush brush = item->background();
				if (dst_ai == UNALLOCATED_ACTIVITY)
					brush.setColor(Qt::black);
				brush.setStyle(brushStyle);
				item->setBackground(brush);
			}
		}

	}

	event->accept();
}

//	if (rules == NULL || solution == NULL) {
//		event->ignore();
//		return;
//	}
//	if (event->source() != this) {
//		event->ignore();
//		return;
//	}
//	if (! event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
//		event->ignore();
//		return;
//	}
//	QTableWidgetItem *item = itemAt(event->pos());
//	if (!item) {
//		event->ignore();
//		return;
//	}
//	event->setDropAction(Qt::MoveAction);
//	event->accept();
//}

#include <QDebug>

void EditableTimetableWidget::dragMoveEvent(QDragMoveEvent* event)
{
	qWarning() << "drag move";
	if (rules == NULL || simulation_running) {
		event->ignore();
		return;
	}
	if (event->source() != this) {
		event->ignore();
		return;
	}
	if (! event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->ignore();
		return;
	}
	QTableWidgetItem *item = itemAt(event->pos());
	if (!item) {
		event->ignore();
		return;
	}

	QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	while (!stream.atEnd())
	{
		int src_row, src_column;
		QMap<int,  QVariant> roleDataMap;
		stream >> src_row >> src_column >> roleDataMap;

		int src_ai = roleDataMap.value(Qt::UserRole).toInt();

		const int intended_h0 = getHour(rowAt(event->pos().y()), columnAt(event->pos().x()));
		qWarning() << "intended hour:" << intended_h0;
		const int act_duration = rules->internalActivitiesList[src_ai].duration;

		if (intended_h0 + act_duration > rules->nHoursPerDay) {
			qWarning() << "ignore 1";
			event->ignore();
			return;
		}

		QTableWidgetItem *item = itemAt(event->pos());
		int dst_ai = item->data(Qt::UserRole).toInt();

		if (dst_ai != UNALLOCATED_ACTIVITY) {
			qWarning() << "ingnore 2";
			event->ignore();
			return;
		}
	}

	qWarning() << "accept";
	event->setDropAction(Qt::MoveAction);
	event->accept();
}

void EditableTimetableWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
	qWarning() << "drag leave";

	Qt::BrushStyle brushStyle = Qt::SolidPattern;
	for (int r=0; r < rowCount(); r++) {
		for (int c=0; c < columnCount(); c++) {
			QTableWidgetItem *item = this->item(r, c);
			int dst_ai = item->data(Qt::UserRole).toInt();

			QBrush brush = item->background();
			if (dst_ai == UNALLOCATED_ACTIVITY) {
				brush = this->palette().base();
			}
			brush.setStyle(brushStyle);
			item->setBackground(brush);
		}
	}
}


#include <QDebug>

static int filterUnallocatedActivities(QStringList &conflictList)
{
	int counter = 0;
	for (QStringList::iterator it = conflictList.begin(); it != conflictList.end(); ) {
		if (it->startsWith(ConstraintBasicCompulsoryTime::tr("Time constraint basic compulsory broken: unallocated activity with id="))) {
			it = conflictList.erase(it);
			counter++;
		}
		else
			++it;
	}
	return counter;
}

void EditableTimetableWidget::dropEvent(QDropEvent* event)
{
	qWarning() << "drop";
	if (rules == NULL) {// || index < 0) {
		event->ignore();
		return;
	}
	const char *mime_type = "application/x-qabstractitemmodeldatalist";
	if (!event->mimeData()->hasFormat(mime_type) || event->source() != this) {
		event->ignore();
		return;
	}

	bool success = true;

	Solution tmp_solution(*rules);
	tmp_solution.copy(*rules, *solution);
	solution->resetFitness();
	QString conflicts;
	const double original_fitness = solution->fitness(*rules, &conflicts);
	int original_unallocated = filterUnallocatedActivities(solution->severeConflictList);

	QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	while (!stream.atEnd())
	{
		int src_row, src_column;
		QMap<int,  QVariant> roleDataMap;
		stream >> src_row >> src_column >> roleDataMap;

		int src_ai = roleDataMap.value(Qt::UserRole).toInt();

		qWarning() << "from:" << solution->time(src_ai);

		QTableWidgetItem *item = itemAt(event->pos());
		int dst_ai = item->data(Qt::UserRole).toInt();
		qWarning() << "to:" << getTime(rowAt(event->pos().y()), columnAt(event->pos().x())) << (dst_ai != UNALLOCATED_ACTIVITY);

		const int intended_h0 = getHour(rowAt(event->pos().y()), columnAt(event->pos().x()));
		qWarning() << "intended hour:" << intended_h0;
		const int act_duration = rules->internalActivitiesList[src_ai].duration;

		if (intended_h0 + act_duration > rules->nHoursPerDay) {
			event->ignore();
			return;
		}

		if (dst_ai != UNALLOCATED_ACTIVITY) {
			event->ignore();
			return;
		}

		tmp_solution.setTime(src_ai, getTime(item->row(), item->column()));
		tmp_solution.resetFitness();

		ProposalStatus status = checkSolutionChange(tmp_solution);
		if (status == AN_IMPOSSIBLE_PROPOSAL) {
			qWarning() << "impossible";
			success = false;
			break;
		}
//		double modified_fitness = tmp_solution.fitness(*rules, &conflicts);
//		QStringList modified_conflicts = tmp_solution.severeConflictList;
//		int modified_unallocated = filterUnallocatedActivities(modified_conflicts);
//		qWarning() << solution->severeConflictList;
//		qWarning() << "-----------------------------------\n" << tmp_solution.severeConflictList.join("\n");

//		if (original_fitness != modified_fitness || solution->severeConflictList != modified_conflicts) {
//			success = false;
//			qWarning() << "divergiu" << solution->severeConflictList.count() << modified_conflicts.count();
//			foreach (QString s, modified_conflicts.toSet().subtract(solution->severeConflictList.toSet())) {
//				qWarning() << s;
//			}
//			break;
//		}
	}
	if (success) {
		clearSelection();
		solution->copy(*rules, tmp_solution);
		CachedSchedule::update(*solution);
		emit solution_changed();
	}
}

const Rules* EditableTimetableWidget::getRules() const
{
	return rules;
}

const Solution& EditableTimetableWidget::getSolution() const
{
	return *solution;
}

TimetableDirection EditableTimetableWidget::getTimetableDirection() const
{
	return timetableDirection;
}

void EditableTimetableWidget::setTimetableDirection(const TimetableDirection& value)
{
	timetableDirection = value;
}

int EditableTimetableWidget::getTime(int row, int column) const
{
	switch (timetableDirection) {
	case DAYS_HORIZONTAL:
		return row * rules->nDaysPerWeek + column;
	case TIMES_HORIZONTAL:
		return (column % rules->nHoursPerDay) * rules->nDaysPerWeek + column/rules->nHoursPerDay;
	default:
		return -1;
	}
}

int EditableTimetableWidget::getDay(int row, int column) const
{
	Q_UNUSED(row);
	switch (timetableDirection) {
	case DAYS_HORIZONTAL:
		return column;
	case TIMES_HORIZONTAL:
		return column/rules->nHoursPerDay;
	default:
		return -1;
	}
}

int EditableTimetableWidget::getHour(int row, int column) const
{
	switch (timetableDirection) {
	case DAYS_HORIZONTAL:
		return row;
	case TIMES_HORIZONTAL:
		return column % rules->nHoursPerDay;
	default:
		return -1;
	}
}

EditableTimetableWidget::ProposalStatus EditableTimetableWidget::checkSolutionChange(Solution& proposal) const
{
	double current_fitness = solution->fitness(*rules);

	QString proposed_conflicts;
	double proposed_fitness = proposal.fitness(*rules, &proposed_conflicts);
	int num_unallocated_activities_on_proposal = filterUnallocatedActivities(proposal.severeConflictList);
//	QStringList severeConflictsSolved = solution->severeConflictList.toSet().subtract(proposal.severeConflictList.toSet()).toList();
//	QStringList severeConflictsCreated = proposal.severeConflictList.toSet().subtract(solution->severeConflictList.toSet()).toList();
//	QStringList softConflictsSolved = proposed_conflicts.split("\n").toSet().subtract(current_conflicts.split("\n").toSet()).toList();
//	QStringList softConflictsCreated = current_conflicts.split("\n").toSet().subtract(proposed_conflicts.split("\n").toSet()).toList();

	bool proposalHasLessSevereConflicts = false;

	if (solution->severeConflictList != proposal.severeConflictList) {
//			qWarning() << "divergiu" << solution->severeConflictList.count() << proposed_solution.severeConflictList.count();
//			foreach (QString s, proposed_solution.severeConflictList.toSet().subtract(solution->severeConflictList.toSet())) {
//				qWarning() << s;
//			}
		if (solution->severeConflictList.count() < proposal.severeConflictList.count()) {
//			qWarning() << "\t\tignoring [severe conflicts]" << dst_act->subjectName << "-" << dst_act->studentsNames;
			return AN_IMPOSSIBLE_PROPOSAL;
		}
		if (solution->severeConflictList.count() > proposal.severeConflictList.count()) {
//			qWarning() << "\t\t-- solving some severe conflicts" << dst_act->subjectName << "-" << dst_act->studentsNames;
			proposalHasLessSevereConflicts = true;
		} else {
//			qWarning() << "\t\t-- changing some severe conflicts" << dst_act->subjectName << "-" << dst_act->studentsNames;
		}
	}

	if (current_fitness < proposed_fitness && !proposalHasLessSevereConflicts)
		return A_WORSE_PROPOSAL;
	else if (current_fitness > proposed_fitness)
		return A_BETTER_PROPOSAL;

	if (proposalHasLessSevereConflicts)
		return A_BETTER_PROPOSAL;
	return QUID_PRO_QUO;
}

void EditableTimetableWidget::itemPressed(QTableWidgetItem* item)
{
	if (simulation_running)
		return;
	if (!rules || !solution)
		return;
	if (!item->data(Qt::UserRole).isValid())
		return;

	int src_time = getTime(item->row(), item->column());
	int src_ai = item->data(Qt::UserRole).toInt();
	if (src_ai == UNALLOCATED_ACTIVITY)
		return;

	const Activity *src_act  = &rules->internalActivitiesList[src_ai];

	QString current_conflicts;
	double current_fitness = solution->fitness(*rules, &current_conflicts);
	int num_unallocated_activities_on_current = filterUnallocatedActivities(solution->severeConflictList);

	qWarning() << "searching possible swaps for" << src_act->subjectName << "-" << src_act->studentsNames;

	QSet<int> activity_ids;
	if (timetableDirection == DAYS_HORIZONTAL) {
		for (int c=0; c < columnCount(); c++) {
			for (int r=0; r < rowCount(); ) {
				QVariant value = model()->itemData(model()->index(r, c)).value(Qt::UserRole);
				int dst_ai = value.toInt();
				if (!value.isValid() || dst_ai == UNALLOCATED_ACTIVITY || src_ai == dst_ai) {
					if (!value.isValid())
					{
						qWarning() << "INVALIDO" << r << c;
					}
					r++;
					continue;
				}
				activity_ids << dst_ai;
				const Activity * act = &rules->internalActivitiesList[dst_ai];
				r += act->duration;
			}
		}
	} else if (timetableDirection == TIMES_HORIZONTAL) {
		int row = item->row();
		int column = 0;
		while(column < columnCount()) {
			QVariant value = model()->itemData(model()->index(row, column)).value(Qt::UserRole);
			int dst_ai = value.toInt();
			if (!value.isValid() || dst_ai == UNALLOCATED_ACTIVITY || src_ai == dst_ai) {
				column++;
				continue;
			}
			activity_ids << dst_ai;
			const Activity * act = &rules->internalActivitiesList[dst_ai];
			column += act->duration;
		}
	}

	QList<int> possible_swaps;

	foreach (int dst_ai, activity_ids) {
		assert(src_ai != dst_ai);

		const Activity * dst_act = &rules->internalActivitiesList[dst_ai];

		if (src_act->activityGroupId != 0 && src_act->activityGroupId == dst_act->activityGroupId) {
			if (src_act->teachersNames == dst_act->teachersNames
				&& src_act->studentsNames == dst_act->studentsNames
				&& src_act->subjectName == dst_act->subjectName
				&& src_act->activityTagsNames == dst_act->activityTagsNames) {
				continue;
			}
		}

		if (src_act->duration != dst_act->duration) {
			qWarning() << "\t\tignoring [different length]" << dst_act->subjectName << "-" << dst_act->studentsNames;
			continue;
		}

		Solution proposed_solution(*rules);
		proposed_solution.copy(*rules, *solution);
		proposed_solution.resetFitness();

		proposed_solution.setTime(src_ai, proposed_solution.time(dst_ai));
		proposed_solution.setTime(dst_ai, src_time);

		ProposalStatus status = checkSolutionChange(proposed_solution);
		if (status == AN_IMPOSSIBLE_PROPOSAL) {
			qWarning() << "\t\t Impossible: " << dst_act->subjectName << "-" << dst_act->studentsNames;
			continue;
		}

		qWarning() << "\t" << dst_act->subjectName << "-" << dst_act->studentsNames;
		possible_swaps << dst_ai;
	}
}
