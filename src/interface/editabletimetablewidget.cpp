/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "editabletimetablewidget.h"

#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

#include "rules.h"

#include "timetableexport.h"

#include <QMenu>
#include <QAction>

extern bool simulation_running;

#include <QDebug>

EditableTimetableWidget::EditableTimetableWidget(QWidget *parent)
	: QTableWidget(parent), rules(NULL), solution(NULL), timetableDirection(DAYS_HORIZONTAL)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	connect(this, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* item, QTableWidgetItem*) {
		this->colorizePossibleActions(item);
	});
}

EditableTimetableWidget::EditableTimetableWidget(int rows, int cols, QWidget* parent)
	: QTableWidget(rows, cols, parent), rules(NULL), solution(NULL), timetableDirection(DAYS_HORIZONTAL)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	connect(this, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* item, QTableWidgetItem*) {
		this->colorizePossibleActions(item);
	});
}

EditableTimetableWidget::~EditableTimetableWidget()
{
	delete solution;
	solution = NULL;
}

void EditableTimetableWidget::setSolution(const Rules* rules, const Solution& solution)
{
	this->rules = rules;
	if (this->solution == NULL)
		this->solution = new Solution();
	else if (solution == *this->solution)
		return;
	undoStack.clear();
	this->solution->copy(*rules, solution);
	this->tempRemovedActivities.clear();
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

	tempRemovedActivities.clear();
}

void EditableTimetableWidget::contextMenuEvent(QContextMenuEvent* event)
{
	if (simulation_running)
		return;
	if (!rules || !solution)
		return;

	QTableWidgetItem *item = itemAt(event->pos());

	if (item == NULL)
		return;

	const int src_ai = item->data(Qt::UserRole).toInt();

	const int h0 = getHour(item->row(), item->column());
	const int time = getTime(item->row(), item->column());

	bool src_act_is_locked = isActivityLocked(src_ai);

	QMenu contextMenu(tr("Context menu"), this);

	QSet<int> placed_activity_ids_but_clicked = getPlacedActivities(item);
	if (item->data(Qt::UserRole).isValid()) {
		if (src_ai != UNALLOCATED_ACTIVITY)
			placed_activity_ids_but_clicked.remove(src_ai);
	}
	QSet<int> temp_removed_activities = getRemovedActivities();
	QList<int> temp_removed_activities_list = temp_removed_activities.toList();
	std::sort(temp_removed_activities_list.begin(), temp_removed_activities_list.end());
	QList<int> placed_activity_ids_but_clicked_list = placed_activity_ids_but_clicked.toList();
	std::sort(placed_activity_ids_but_clicked_list.begin(), placed_activity_ids_but_clicked_list.end());

	QMenu* placeMenu = contextMenu.addMenu(tr("Place Activity..."));
	QList<int> all_placeable = temp_removed_activities_list + placed_activity_ids_but_clicked_list;
	bool changed_to_not_removed = false;
	for (int ai : qAsConst(all_placeable)) {
		if (h0 + rules->internalActivitiesList[ai].duration > rules->nHoursPerDay)
			continue;
		if (!changed_to_not_removed && !temp_removed_activities.contains(ai)) {
			placeMenu->addSeparator();
			changed_to_not_removed = true;
		}
		QAction* action = new QAction(rules->internalActivitiesList[ai].getDescription(), this);
		placeMenu->addAction(action);
		connect(action, &QAction::triggered, [this,item, ai](){ placeActivity(item, ai); });
		ConstraintBasicCompulsoryTime tctr(100);
		Solution tmpSolution;
		tmpSolution.copy(*rules, *solution);
		if (src_ai != UNALLOCATED_ACTIVITY)
			tmpSolution.unsetTime(src_ai);
		tmpSolution.setTime(ai, time);
		ConflictInfo conflictInfo;
		if (tctr.fitness(tmpSolution, *rules, true, &conflictInfo) > 0) {
			action->setEnabled(false);
			action->setText(action->text() + " (" + conflictInfo.descriptions.join("; ")+ ")");
		}
	}
	int num_all_activity_ids_but_clicked = temp_removed_activities.count() + placed_activity_ids_but_clicked.count();
	if (num_all_activity_ids_but_clicked < 1)
		placeMenu->setEnabled(false);

	QAction actionRemove(tr("Remove Activity"), this);
	connect(&actionRemove, &QAction::triggered, [this,item](){ removeActivity(item); });
	contextMenu.addAction(&actionRemove);
	if (src_ai == UNALLOCATED_ACTIVITY)
		actionRemove.setEnabled(false);
	else if (src_act_is_locked) {
		actionRemove.setEnabled(false);
		actionRemove.setText(actionRemove.text() + " " + tr("(locked)"));
	}

	QMenu* swapMenu = contextMenu.addMenu(tr("Swap Activity..."));
	if (src_ai != UNALLOCATED_ACTIVITY) {
		QList<int> possibleSwaps = getPossibleSwaps(placed_activity_ids_but_clicked.toList(), src_ai);
		std::sort(possibleSwaps.begin(), possibleSwaps.end());
		for (int ai : possibleSwaps) {
			if (h0 + rules->internalActivitiesList[ai].duration > rules->nHoursPerDay)
				continue;

			QAction* action = new QAction(rules->internalActivitiesList[ai].getDescription(), this);
			swapMenu->addAction(action);
			connect(action, &QAction::triggered, [this,item, ai](){ swapActivity(item, ai); });
		}
		if (possibleSwaps.count() < 1)
			swapMenu->setEnabled(false);
	} else
		swapMenu->setEnabled(false);

	contextMenu.exec(event->globalPos());
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
//	int num_unallocated_activities_on_proposal = filterUnallocatedActivities(proposal.severeConflictList);
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

QSet<int> EditableTimetableWidget::getPlacedActivities(const QTableWidgetItem* item) const
{
	QSet<int> activity_ids;
	if (timetableDirection == DAYS_HORIZONTAL) {
		for (int c=0; c < columnCount(); c++) {
			for (int r=0; r < rowCount(); ) {
				QVariant value = model()->itemData(model()->index(r, c)).value(Qt::UserRole);
				int dst_ai = value.toInt();
				if (!value.isValid() || dst_ai == UNALLOCATED_ACTIVITY) {
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
			if (!value.isValid() || dst_ai == UNALLOCATED_ACTIVITY) {
				column++;
				continue;
			}
			activity_ids << dst_ai;
			const Activity * act = &rules->internalActivitiesList[dst_ai];
			column += act->duration;
		}
	}
	return activity_ids;
}

QSet<int> EditableTimetableWidget::getRemovedActivities() const
{
	return tempRemovedActivities;
}

QList<int> EditableTimetableWidget::getPossibleSwaps(const QList<int> &activity_ids, int src_ai) const
{
	assert(src_ai>=0 && src_ai<MAX_ACTIVITIES);

	const Activity * src_act = &rules->internalActivitiesList[src_ai];
	const int src_time = solution->time(src_ai);

	QList<int> possible_swaps;

	for (int dst_ai : qAsConst(activity_ids)) {
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
			continue;
		}

		Solution proposed_solution;
		proposed_solution.copy(*rules, *solution);
		proposed_solution.resetFitness();

		proposed_solution.setTime(src_ai, proposed_solution.time(dst_ai));
		proposed_solution.setTime(dst_ai, src_time);

		ProposalStatus status = checkSolutionChange(proposed_solution);
		if (status == AN_IMPOSSIBLE_PROPOSAL) {
			continue;
		}

		possible_swaps << dst_ai;
	}
	return possible_swaps;
}

void EditableTimetableWidget::colorizePossibleActions(QTableWidgetItem* item)
{
	if (!item->data(Qt::UserRole).isValid())
		return;

	const int src_ai = item->data(Qt::UserRole).toInt();

	QSet<int> placed_activity_ids_but_clicked = getPlacedActivities(item);
	if (item->data(Qt::UserRole).isValid()) {
		if (src_ai != UNALLOCATED_ACTIVITY)
			placed_activity_ids_but_clicked.remove(src_ai);
	}

	const int time = getTime(item->row(), item->column());
	const int h0 = getHour(item->row(), item->column());

	QList<int> all_placeable = tempRemovedActivities.toList() + placed_activity_ids_but_clicked.toList();
	for (int ai : all_placeable) {
		if (h0 + rules->internalActivitiesList[ai].duration > rules->nHoursPerDay) {
			all_placeable.removeOne(ai);
			continue;
		}
		ConstraintBasicCompulsoryTime tctr(100);
		Solution tmpSolution;
		tmpSolution.copy(*rules, *solution);
		if (src_ai != UNALLOCATED_ACTIVITY)
			tmpSolution.unsetTime(src_ai);
		tmpSolution.setTime(ai, time);
		ConflictInfo conflictInfo;
		if (tctr.fitness(tmpSolution, *rules, true, &conflictInfo) > 0) {
			all_placeable.removeOne(ai);
		}
	}

	QList<int> swaps;
	if (src_ai != UNALLOCATED_ACTIVITY) {
		swaps = getPossibleSwaps(placed_activity_ids_but_clicked.toList(), src_ai);
	}

	QList<QTableWidgetItem*> allItems = findItems("*", Qt::MatchWildcard);
	for (QTableWidgetItem* tmp_item : qAsConst(allItems)) {
		int tmp_ai = tmp_item->data(Qt::UserRole).toInt();
		if (tmp_ai == UNALLOCATED_ACTIVITY)
			continue;
		if (swaps.contains(tmp_ai)) {
			tmp_item->setBackground(QBrush(Qt::darkGreen));
			tmp_item->setForeground(QBrush(Qt::white));
		} else if (all_placeable.contains(tmp_ai)) {
			tmp_item->setBackground(QBrush(Qt::darkBlue));
			tmp_item->setForeground(QBrush(Qt::white));
		} else {
			tmp_item->setBackground(QBrush(Qt::gray));
			tmp_item->setForeground(QBrush(Qt::black));
		}
	}
}

bool EditableTimetableWidget::isActivityLocked(int ai) const
{
	bool is_locked = false;
	const int id = rules->internalActivitiesList[ai].id;
	if (rules->apstHash.contains(id)) {
		QSet<ConstraintActivityPreferredStartingTime*> cs = rules->apstHash.value(id);
		for (ConstraintActivityPreferredStartingTime* ctr : qAsConst(cs)) {
			if (ctr->active && ctr->weightPercentage >= 100) {
				is_locked = true;
				break;
			}
		}
	}
	return is_locked;
}

void EditableTimetableWidget::placeActivity(const QTableWidgetItem* item, int ai)
{
	QSet<int> removedNow;
	int duration = rules->internalActivitiesList[ai].duration;
	int day = getDay(item->row(), item->column());
	int h0 = getHour(item->row(), item->column());
	if (h0 + duration > rules->nHoursPerDay)
		return;

	for(int h=h0; h < h0+duration; h++) {
		QVariant value = model()->itemData(model()->index(h, day)).value(Qt::UserRole);
		int removableActIdx = value.toInt();

		if (removableActIdx == UNALLOCATED_ACTIVITY)
			continue;

		solution->unsetTime(removableActIdx);
		tempRemovedActivities.insert(removableActIdx);
		removedNow.insert(removableActIdx);
	}

	tempRemovedActivities.remove(ai);

	solution->setTime(ai, getTime(item->row(), item->column()));

	QString useless;
	solution->fitness(*rules, &useless);
	CachedSchedule::update(*solution);
	emit solution_changed();

	clearSelection();

	emit activityPlaced(ai);
	for (int remAi : qAsConst(removedNow)) {
		emit activityRemoved(remAi);
	}
}

void EditableTimetableWidget::removeActivity(const QTableWidgetItem* item)
{
	if (!item->data(Qt::UserRole).isValid())
		return;

	int src_ai = item->data(Qt::UserRole).toInt();
	if (src_ai == UNALLOCATED_ACTIVITY)
		return;

	solution->unsetTime(src_ai);
	tempRemovedActivities.insert(src_ai);

	QString useless;
	solution->fitness(*rules, &useless);
	CachedSchedule::update(*solution);
	emit solution_changed();

	clearSelection();

	emit activityRemoved(src_ai);
}

void EditableTimetableWidget::swapActivity(const QTableWidgetItem* item, int ai)
{
	if (!item->data(Qt::UserRole).isValid())
		return;
	int src_ai = item->data(Qt::UserRole).toInt();
	int src_time = getTime(item->row(), item->column());
	assert(solution->time(src_ai) == src_time);

	solution->setTime(src_ai, solution->time(ai));
	solution->setTime(ai, src_time);

	QString useless;
	solution->fitness(*rules, &useless);
	CachedSchedule::update(*solution);
	emit solution_changed();

	clearSelection();
}
