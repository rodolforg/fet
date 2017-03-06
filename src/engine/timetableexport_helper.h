#ifndef TIMETABLEEXPORTHELPER_H
#define TIMETABLEEXPORTHELPER_H

#include <QObject>
#include <QList>

#include "rules.h"
#include "solution.h"
#include "matrix.h"
#include "activity.h"

class TimetableExportHelper : public QObject
{
	Q_OBJECT
public:
	TimetableExportHelper(const Rules& rules, const Solution& solution);

	/// Depicts a timetable element
	struct Element {
		QList<const Activity*> activities;
		int timeSpan = 1;
	};
	struct Table {
		/// Indexes: Teacher/Student/Room/Subject/etc. , Day, Hour
		Matrix3D<Element> data;

		QList<int> maxSimultaneousActivitiesPerDay;
	};

	void getTeachersTimetable(Table &table) const;
	void getSubjectsTimetable(Table &table) const;
	void getActivityTagsTimetable(Table &table) const;
	void getRoomsTimetable(Table &table) const;
	void getActivitiesTimetable(Table &table) const;
	void getYearsTimetable(Table &table) const;
	void getGroupsTimetable(Table &table) const;
	void getSubgroupsTimetable(Table &table) const;

	Room *getRoom(const Activity *activity) const;

private:
	typedef QList<int> (TimetableExportHelper::*GetStuffIndexMethod)(const Activity &) const;
	const Rules &rules;
	const Solution &solution;
	void getBasicTimetable(Table &table, GetStuffIndexMethod getStuffIndex, int nStuff) const;
	void fillTimetableForActivities(Element **timetable, const QList<int>& activities) const;
	void computeExtraInfoTimeTable(Table &table, int nStuff) const;

	QList<int> getTeacherIndex(const Activity &act) const;
	QList<int> getSubjectIndex(const Activity &act) const;
	QList<int> getActivityTagIndex(const Activity &act) const;
	QList<int> getRoomIndex(const Activity &act) const;
	QList<int> getActivityIndex(const Activity &act) const;
	QList<int> getYearIndex(const Activity &act) const;
	QList<int> getGroupIndex(const Activity &act) const;
	QList<int> getSubgroupIndex(const Activity &act) const;
};

#endif // TIMETABLEEXPORTHELPER_H
