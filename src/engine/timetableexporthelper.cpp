#include "timetableexporthelper.h"

TimetableExportHelper::TimetableExportHelper(const Rules &rules, const Solution &solution)
	: rules(rules), solution(solution)
{

}

void TimetableExportHelper::getTeachersTimetable(Table &table) const
{
	int nteachers = rules.teachersList.count();
	getBasicTimetable(table, &TimetableExportHelper::getTeacherIndex, nteachers);
}

void TimetableExportHelper::getSubjectsTimetable(TimetableExportHelper::Table &table) const
{
	int nsubjects = rules.subjectsList.count();
	getBasicTimetable(table, &TimetableExportHelper::getSubjectIndex, nsubjects);
}

void TimetableExportHelper::getActivityTagsTimetable(TimetableExportHelper::Table &table) const
{
	int nactivityTags = rules.activityTagsList.count();
	getBasicTimetable(table, &TimetableExportHelper::getActivityTagIndex, nactivityTags);
}

void TimetableExportHelper::getRoomsTimetable(TimetableExportHelper::Table &table) const
{
	int nrooms = rules.roomsList.count();
	getBasicTimetable(table, &TimetableExportHelper::getRoomIndex, nrooms);
}

void TimetableExportHelper::getActivitiesTimetable(TimetableExportHelper::Table &table) const
{
	getBasicTimetable(table, &TimetableExportHelper::getActivityIndex, 1);
}

void TimetableExportHelper::getYearsTimetable(TimetableExportHelper::Table &table) const
{
	int nyears = rules.internalGroupsList.count();
	getBasicTimetable(table, &TimetableExportHelper::getYearIndex, nyears);
}

void TimetableExportHelper::getGroupsTimetable(TimetableExportHelper::Table &table) const
{
	int ngroups = rules.internalGroupsList.count();
	getBasicTimetable(table, &TimetableExportHelper::getGroupIndex, ngroups);
}

void TimetableExportHelper::getSubgroupsTimetable(TimetableExportHelper::Table &table) const
{
	int nsubgroups = rules.nInternalSubgroups;
	getBasicTimetable(table, &TimetableExportHelper::getSubgroupIndex, nsubgroups);
}

Room *TimetableExportHelper::getRoom(const Activity *activity) const
{
	QList<int> idxRooms = getRoomIndex(*activity);
	if (idxRooms.isEmpty())
		return NULL;
	int idxRoom = idxRooms[0];
	if (idxRoom == UNALLOCATED_SPACE || idxRoom == UNSPECIFIED_ROOM)
		return NULL;
	return rules.internalRoomsList[idxRoom];
}

void TimetableExportHelper::getBasicTimetable(Table &table, GetStuffIndexMethod getStuffIndex, int numStuff) const
{
	// Fill table activities
	table.data.resize(numStuff, rules.nDaysPerWeek, rules.nHoursPerDay);
	for(int idxAct=0; idxAct < rules.nInternalActivities; idxAct++) {
		const int time = solution.time(idxAct);
		if(time != UNALLOCATED_TIME) {
			int hour = time / rules.nDaysPerWeek;
			int day = time % rules.nDaysPerWeek;
			const Activity &act = rules.internalActivitiesList[idxAct];
			QList<int> indexes = (this->*getStuffIndex)(act);
			for (int h=hour; h < hour+act.duration; h++) {
				for (int idx : qAsConst(indexes))
					table.data[idx][day][h].elements << Element::SubElement(&act, hour+act.duration-h);
			}
		}
	}

	table.computeExtraInfo();
}

void TimetableExportHelper::fillTimetableForActivities(Element** timetable, const QList<int>& activities) const
{
	for(int idxAct : qAsConst(activities)) {
		const int time = solution.time(idxAct);
		if(time != UNALLOCATED_TIME) {
			int hour = time / rules.nDaysPerWeek;
			int day = time % rules.nDaysPerWeek;
			const Activity &act = rules.internalActivitiesList[idxAct];
			for (int h=hour; h < hour+act.duration; h++) {
				timetable[day][h].elements << &act;
			}
		}
	}
}

QList<int> TimetableExportHelper::getTeacherIndex(const Activity &act) const
{
	QList<int> results;
	for (const QString& teacherName : qAsConst(act.teachersNames))
		results << rules.teachersHash.value(teacherName);
	return results;
}

QList<int> TimetableExportHelper::getSubjectIndex(const Activity &act) const
{
	QList<int> results;
	results << act.subjectIndex;
	return results;
}

QList<int> TimetableExportHelper::getActivityTagIndex(const Activity &act) const
{
	QList<int> results;
	for (const QString& activityTagName : qAsConst(act.activityTagsNames))
		results << rules.activityTagsHash.value(activityTagName);
	return results;
}

QList<int> TimetableExportHelper::getRoomIndex(const Activity &act) const
{
	QList<int> results;
	int idxAct = rules.activitiesHash.value(act.id);
	int idxRoom = solution.room(idxAct);
	if (idxRoom != UNALLOCATED_SPACE && idxRoom != UNSPECIFIED_ROOM)
		results << idxRoom;
	return results;
}

QList<int> TimetableExportHelper::getActivityIndex(const Activity &act) const
{
	Q_UNUSED(act);
	QList<int> results;
	results << 0;
	return results;
}

QList<int> TimetableExportHelper::getYearIndex(const Activity &act) const
{
	QSet<int> results;
	QSet<int> subgroupsVisited;
	for (int idxSubgroup : qAsConst(act.iSubgroupsList)) {
		if (subgroupsVisited.contains(idxSubgroup))
			continue;
		subgroupsVisited << idxSubgroup;
		for (StudentsYear *year : qAsConst(rules.augmentedYearsList)) {
			for (StudentsGroup *group : qAsConst(year->groupsList)) {
				if (group->subgroupsList.contains(rules.internalSubgroupsList[idxSubgroup])) {
					results << year->indexInAugmentedYearsList;
					break;
				}
			}
		}
	}
	return QList<int>::fromSet(results);
}

QList<int> TimetableExportHelper::getGroupIndex(const Activity &act) const
{
	QSet<int> results;
	QSet<int> subgroupsVisited;
	for (int idxSubgroup : qAsConst(act.iSubgroupsList)) {
		if (subgroupsVisited.contains(idxSubgroup))
			continue;
		subgroupsVisited << idxSubgroup;
		for (StudentsGroup *group : qAsConst(rules.internalGroupsList)) {
			if (group->subgroupsList.contains(rules.internalSubgroupsList[idxSubgroup])) {
				results << group->indexInInternalGroupsList;
				break;
			}
		}
	}
	return QList<int>::fromSet(results);
}

QList<int> TimetableExportHelper::getSubgroupIndex(const Activity &act) const
{
	QList<int> results;
	results << act.iSubgroupsList;
	return results;
}

void TimetableExportHelper::Table::computeExtraInfo()
{
	// Fill table statistics
	const int numStuff = data.getD1();
	const int numDays = data.getD2();
	const int numHours = data.getD3();
	bool computeEmptyHoursSpan = true;
	maxSimultaneousActivitiesPerDay.resize(numStuff, numDays);
	for (int t=0; t < numStuff; t++) {
		for (int d=0; d < numDays; d++) {
			int maxSimultaneousActivitiesToday = 0;
			for (int h=0; h < numHours; ) {
				if (!computeEmptyHoursSpan && data[t][d][h].elements.count() == 0) {
//					data[t][d][h].timeSpan = 1;
					h++;
					continue;
				}
				if (maxSimultaneousActivitiesToday < data[t][d][h].elements.count())
					maxSimultaneousActivitiesToday = data[t][d][h].elements.count();
				int h2 = h+1;
				while (h2 < numHours) {
					if (data[t][d][h2].elements.count() != data[t][d][h].elements.count())
						break;
					bool different = false;
					for(int i=0; i < data[t][d][h2].elements.count(); i++) {
						if (data[t][d][h2].elements[i].activity != data[t][d][h].elements[i].activity) {
							different = true;
							break;
						}
					}
					if (different)
						break;
					h2++;
				}
				int span = h2-h;
				for (int i=0; i < span; i++) {
					data[t][d][h+i].timeSpan = span-i;
					data[t][d][h+i].covered = true;
				}
				data[t][d][h].covered = false;
				h += span;
			}
			maxSimultaneousActivitiesPerDay(t, d) = maxSimultaneousActivitiesToday;
		}
	}
}

bool TimetableExportHelper::Element::contains(const Activity* activity) const
{
	for (const SubElement& element : qAsConst(elements)) {
		if (element.activity == activity)
			return true;
	}
	return false;
}
