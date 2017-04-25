#include <QtTest>

#include "generate_pre.h"

Timetable gt; // Just to please generate_pre.cpp that yet requires it. It must be not used in tests

class GeneratePreTest : public QObject
{
	Q_OBJECT

public:
	GeneratePreTest();

private:
	struct MockRules3Activities
	{
		MockRules3Activities();
		Rules rules;
		void addTeachers(int n);
	};

private slots:
	void MinDays_ReturnOkIfNoMinDaysConstraint();
	void MinDays_InactiveConstraintIsIgnored();
	void MinDays_ConstraintWithInactiveActivityIsIgnored();
	void MinDays_ComputedSize();
	void MinDays_CheckValues();
	void MinDays_ActivityHasMoreThanOneOfThisConstraint();
	void MinDays_FailIfActivityMinDaysToItself();
	void MinDays_FailIfActivityMinDaysToItself_v2();
	void MinDays_NumErrorMsgs_WhenPreparationOfMinDaysFails();
	void MinDays_ThreeActivitiesAtOnce();

	void MaxDays_ReturnOkIfNoMaxDaysConstraint();
	void MaxDays_InactiveConstraintIsIgnored();
	void MaxDays_ConstraintWithInactiveActivityIsIgnored();
	void MaxDays_ComputedSize();
	void MaxDays_CheckValues();
	void MaxDays_ActivityHasMoreThanOneOfThisConstraint();
	void MaxDays_FailIfActivityMaxDaysToItself();
	void MaxDays_FailIfActivityMaxDaysToItself_v2();
	void MaxDays_NumErrorMsgs_WhenPreparationOfMaxDaysFails();
	void MaxDays_ThreeActivitiesAtOnce();

	void MinGaps_ReturnOkIfNoMinGapsConstraint();
	void MinGaps_InactiveConstraintIsIgnored();
	void MinGaps_ConstraintWithInactiveActivityIsIgnored();
	void MinGaps_ComputedSize();
	void MinGaps_CheckValues();
	void MinGaps_ActivityHasMoreThanOneOfThisConstraint();
	void MinGaps_FailIfActivityMinGapsToItself();
	void MinGaps_FailIfActivityMinGapsToItself_v2();
	void MinGaps_NumErrorMsgs_WhenPreparationOfMinGapsFails();
	void MinGaps_ThreeActivitiesAtOnce();

	void TeachersMinContinuousGapInInterval_ReturnOkIfNoTeachersMinContinuousGapInIntervalConstraint();
	void TeachersMinContinuousGapInInterval_InactiveConstraintIsIgnored();
	void TeachersMinContinuousGapInInterval_TeachersAffectAll();
	void TeachersMinContinuousGapInInterval_TeacherAffectOnlyOne();
	void TeachersMinContinuousGapInInterval_TeacherHasMoreThanOneOfThisConstraint();
//	void TeachersMinContinuousGapInInterval_FailsWithNegativeMinGap();
//	void TeachersMinContinuousGapInInterval_FailsWithMinGapGreaterThanInterval();
	void TeachersMinContinuousGapInInterval_NumErrorMsgs_WhenPreparationOfTeachersMinContinuousGapInIntervalFails();
//	void TeachersMinContinuousGapInInterval_MoreThanOneToSameTeacherStoresBoth();
	void TeachersMinContinuousGapInInterval_MoreThanOneToSameTeacherWithSameData_StoresOnlyMostRestrictiveOne();
	void TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_StoresMostRestrictiveCase();
	void TeachersMinContinuousGapInInterval_WithSameIntervalOfSameTeacher_WithWeight100_StoresLeastGap();
	void TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_WithWeightNot100_StoresBoth();
	void TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_WithDiffMinGap_StoresBoth();
};

GeneratePreTest::GeneratePreTest()
{
}

void GeneratePreTest::MinDays_ReturnOkIfNoMinDaysConstraint()
{
	MockRules3Activities mock;

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");
}

void GeneratePreTest::MinDays_InactiveConstraintIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr->active = false;
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 0);
	QCOMPARE(mdba.minDays[0].count(), 0);
	QCOMPARE(mdba.consecutiveIfSameDay[0].count(), 0);
	QCOMPARE(mdba.weightPercentages[0].count(), 0);

	QCOMPARE(mdba.activities[1].count(), 0);
	QCOMPARE(mdba.minDays[1].count(), 0);
	QCOMPARE(mdba.consecutiveIfSameDay[1].count(), 0);
	QCOMPARE(mdba.weightPercentages[1].count(), 0);
}

void GeneratePreTest::MinDays_ConstraintWithInactiveActivityIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << mock.rules.activitiesList[0]->id << mock.rules.activitiesList[1]->id;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(85.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);

	mock.rules.activitiesList[0]->active = false;

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.getErrors().count(), 0);
	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 0);
	QCOMPARE(mdba.activities[1].count(), 0);
}

void GeneratePreTest::MinDays_ComputedSize()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 1);
	QCOMPARE(mdba.minDays[0].count(), 1);
	QCOMPARE(mdba.consecutiveIfSameDay[0].count(), 1);
	QCOMPARE(mdba.weightPercentages[0].count(), 1);

	QCOMPARE(mdba.activities[1].count(), 1);
	QCOMPARE(mdba.minDays[1].count(), 1);
	QCOMPARE(mdba.consecutiveIfSameDay[1].count(), 1);
	QCOMPARE(mdba.weightPercentages[1].count(), 1);
}

void GeneratePreTest::MinDays_CheckValues()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.minDays[0][0], 5);
	QCOMPARE(mdba.consecutiveIfSameDay[0][0], false);
	QCOMPARE(mdba.weightPercentages[0][0], 50.0);

	QCOMPARE(mdba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.minDays[1][0], 5);
	QCOMPARE(mdba.consecutiveIfSameDay[1][0], false);
	QCOMPARE(mdba.weightPercentages[1][0], 50.0);
}

void GeneratePreTest::MinDays_ActivityHasMoreThanOneOfThisConstraint()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	acts.clear();
	acts << 23456 << 34567;
	ConstraintMinDaysBetweenActivities *ctr2 = new ConstraintMinDaysBetweenActivities(75.0, true, 2, acts, 4);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[1].count(), 2);
	QCOMPARE(mdba.minDays[1].count(), 2);
	QCOMPARE(mdba.consecutiveIfSameDay[1].count(), 2);
	QCOMPARE(mdba.weightPercentages[1].count(), 2);

	QCOMPARE(mdba.activities[2].count(), 1);
	QCOMPARE(mdba.minDays[2].count(), 1);
	QCOMPARE(mdba.consecutiveIfSameDay[2].count(), 1);
	QCOMPARE(mdba.weightPercentages[2].count(), 1);

	QCOMPARE(mdba.activities[2][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.minDays[2][0], 4);
	QCOMPARE(mdba.consecutiveIfSameDay[2][0], true);
	QCOMPARE(mdba.weightPercentages[2][0], 75.0);
}

void GeneratePreTest::MinDays_FailIfActivityMinDaysToItself()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;

	QSKIP("Disable test due to usage of assert in Rules::addTimeConstraint()");

	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, false, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MinDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinDays_FailIfActivityMinDaysToItself_v2()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, false, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MinDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinDays_NumErrorMsgs_WhenPreparationOfMinDaysFails()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, false, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	mdba.prepare(mock.rules);
	QCOMPARE(mdba.getErrors().count(), 1);

	acts.clear();
	acts << 23456 << 23456;
	ConstraintMinDaysBetweenActivities *ctr2 = new ConstraintMinDaysBetweenActivities(100.0, false, 2, acts, 3);
	mock.rules.timeConstraintsList.append(ctr2);
	mock.rules.computeInternalStructure(NULL);

	mdba.prepare(mock.rules);
	QCOMPARE(mdba.getErrors().count(), 2);
}

void GeneratePreTest::MinDays_ThreeActivitiesAtOnce()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456 << 34567;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, true, 3, acts, 2);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 2);

	QCOMPARE(mdba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.minDays[0][0], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[0][0], true);
	QCOMPARE(mdba.weightPercentages[0][0], 80.0);

	QCOMPARE(mdba.activities[0][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mdba.minDays[0][1], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[0][1], true);
	QCOMPARE(mdba.weightPercentages[0][1], 80.0);

	QCOMPARE(mdba.activities[1].count(), 2);

	QCOMPARE(mdba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.minDays[1][0], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[1][0], true);
	QCOMPARE(mdba.weightPercentages[1][0], 80.0);

	QCOMPARE(mdba.activities[1][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mdba.minDays[1][1], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[1][1], true);
	QCOMPARE(mdba.weightPercentages[1][1], 80.0);

	QCOMPARE(mdba.activities[2].count(), 2);

	QCOMPARE(mdba.activities[2][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.minDays[2][0], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[2][0], true);
	QCOMPARE(mdba.weightPercentages[2][0], 80.0);

	QCOMPARE(mdba.activities[2][1], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.minDays[2][1], 2);
	QCOMPARE(mdba.consecutiveIfSameDay[2][1], true);
	QCOMPARE(mdba.weightPercentages[2][1], 80.0);
}

void GeneratePreTest::MaxDays_ReturnOkIfNoMaxDaysConstraint()
{
	MockRules3Activities mock;

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");
}

void GeneratePreTest::MaxDays_InactiveConstraintIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMaxDaysBetweenActivities *ctr = new ConstraintMaxDaysBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr->active = false;
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 0);
	QCOMPARE(mdba.maxDays[0].count(), 0);
	QCOMPARE(mdba.weightPercentages[0].count(), 0);

	QCOMPARE(mdba.activities[1].count(), 0);
	QCOMPARE(mdba.maxDays[1].count(), 0);
	QCOMPARE(mdba.weightPercentages[1].count(), 0);
}


void GeneratePreTest::MaxDays_ConstraintWithInactiveActivityIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << mock.rules.activitiesList[0]->id << mock.rules.activitiesList[1]->id;
	ConstraintMaxDaysBetweenActivities *ctr = new ConstraintMaxDaysBetweenActivities(85.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);

	mock.rules.activitiesList[0]->active = false;

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.getErrors().count(), 0);
	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 0);
	QCOMPARE(mdba.activities[1].count(), 0);
}

void GeneratePreTest::MaxDays_ComputedSize()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMaxDaysBetweenActivities *ctr = new ConstraintMaxDaysBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 1);
	QCOMPARE(mdba.maxDays[0].count(), 1);
	QCOMPARE(mdba.weightPercentages[0].count(), 1);

	QCOMPARE(mdba.activities[1].count(), 1);
	QCOMPARE(mdba.maxDays[1].count(), 1);
	QCOMPARE(mdba.weightPercentages[1].count(), 1);
}

void GeneratePreTest::MaxDays_CheckValues()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMaxDaysBetweenActivities *ctr = new ConstraintMaxDaysBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.maxDays[0][0], 5);
	QCOMPARE(mdba.weightPercentages[0][0], 50.0);

	QCOMPARE(mdba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.maxDays[1][0], 5);
	QCOMPARE(mdba.weightPercentages[1][0], 50.0);
}

void GeneratePreTest::MaxDays_ActivityHasMoreThanOneOfThisConstraint()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMaxDaysBetweenActivities *ctr1 = new ConstraintMaxDaysBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	acts.clear();
	acts << 23456 << 34567;
	ConstraintMaxDaysBetweenActivities *ctr2 = new ConstraintMaxDaysBetweenActivities(75.0, 2, acts, 4);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[1].count(), 2);
	QCOMPARE(mdba.maxDays[1].count(), 2);
	QCOMPARE(mdba.weightPercentages[1].count(), 2);

	QCOMPARE(mdba.activities[2].count(), 1);
	QCOMPARE(mdba.maxDays[2].count(), 1);
	QCOMPARE(mdba.weightPercentages[2].count(), 1);

	QCOMPARE(mdba.activities[2][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.maxDays[2][0], 4);
	QCOMPARE(mdba.weightPercentages[2][0], 75.0);
}

void GeneratePreTest::MaxDays_FailIfActivityMaxDaysToItself()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;

	QSKIP("Disable test due to usage of assert in Rules::addTimeConstraint()");

	ConstraintMaxDaysBetweenActivities *ctr1 = new ConstraintMaxDaysBetweenActivities(80.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MaxDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MaxDays_FailIfActivityMaxDaysToItself_v2()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMaxDaysBetweenActivities *ctr1 = new ConstraintMaxDaysBetweenActivities(80.0, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MaxDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MaxDays_NumErrorMsgs_WhenPreparationOfMaxDaysFails()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMaxDaysBetweenActivities *ctr1 = new ConstraintMaxDaysBetweenActivities(80.0, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	mdba.prepare(mock.rules);
	QCOMPARE(mdba.getErrors().count(), 1);

	acts.clear();
	acts << 23456 << 23456;
	ConstraintMaxDaysBetweenActivities *ctr2 = new ConstraintMaxDaysBetweenActivities(100.0, 2, acts, 3);
	mock.rules.timeConstraintsList.append(ctr2);
	mock.rules.computeInternalStructure(NULL);

	mdba.prepare(mock.rules);
	QCOMPARE(mdba.getErrors().count(), 2);
}

void GeneratePreTest::MaxDays_ThreeActivitiesAtOnce()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456 << 34567;
	ConstraintMaxDaysBetweenActivities *ctr1 = new ConstraintMaxDaysBetweenActivities(80.0, 3, acts, 2);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");

	QCOMPARE(mdba.activities.getD1(), 3);

	QCOMPARE(mdba.activities[0].count(), 2);

	QCOMPARE(mdba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.maxDays[0][0], 2);
	QCOMPARE(mdba.weightPercentages[0][0], 80.0);

	QCOMPARE(mdba.activities[0][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mdba.maxDays[0][1], 2);
	QCOMPARE(mdba.weightPercentages[0][1], 80.0);

	QCOMPARE(mdba.activities[1].count(), 2);

	QCOMPARE(mdba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.maxDays[1][0], 2);
	QCOMPARE(mdba.weightPercentages[1][0], 80.0);

	QCOMPARE(mdba.activities[1][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mdba.maxDays[1][1], 2);
	QCOMPARE(mdba.weightPercentages[1][1], 80.0);

	QCOMPARE(mdba.activities[2].count(), 2);

	QCOMPARE(mdba.activities[2][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mdba.maxDays[2][0], 2);
	QCOMPARE(mdba.weightPercentages[2][0], 80.0);

	QCOMPARE(mdba.activities[2][1], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mdba.maxDays[2][1], 2);
	QCOMPARE(mdba.weightPercentages[2][1], 80.0);
}

void GeneratePreTest::MinGaps_ReturnOkIfNoMinGapsConstraint()
{
	MockRules3Activities mock;

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");
}

void GeneratePreTest::MinGaps_InactiveConstraintIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinGapsBetweenActivities *ctr = new ConstraintMinGapsBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr->active = false;
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[0].count(), 0);
	QCOMPARE(mgba.minGaps[0].count(), 0);
	QCOMPARE(mgba.weightPercentages[0].count(), 0);

	QCOMPARE(mgba.activities[1].count(), 0);
	QCOMPARE(mgba.minGaps[1].count(), 0);
	QCOMPARE(mgba.weightPercentages[1].count(), 0);
}


void GeneratePreTest::MinGaps_ConstraintWithInactiveActivityIsIgnored()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << mock.rules.activitiesList[0]->id << mock.rules.activitiesList[1]->id;
	ConstraintMinGapsBetweenActivities *ctr = new ConstraintMinGapsBetweenActivities(85.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);

	mock.rules.activitiesList[0]->active = false;

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.getErrors().count(), 0);
	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[0].count(), 0);
	QCOMPARE(mgba.activities[1].count(), 0);
}

void GeneratePreTest::MinGaps_ComputedSize()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinGapsBetweenActivities *ctr = new ConstraintMinGapsBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[0].count(), 1);
	QCOMPARE(mgba.minGaps[0].count(), 1);
	QCOMPARE(mgba.weightPercentages[0].count(), 1);

	QCOMPARE(mgba.activities[1].count(), 1);
	QCOMPARE(mgba.minGaps[1].count(), 1);
	QCOMPARE(mgba.weightPercentages[1].count(), 1);
}

void GeneratePreTest::MinGaps_CheckValues()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinGapsBetweenActivities *ctr = new ConstraintMinGapsBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mgba.minGaps[0][0], 5);
	QCOMPARE(mgba.weightPercentages[0][0], 50.0);

	QCOMPARE(mgba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mgba.minGaps[1][0], 5);
	QCOMPARE(mgba.weightPercentages[1][0], 50.0);
}

void GeneratePreTest::MinGaps_ActivityHasMoreThanOneOfThisConstraint()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinGapsBetweenActivities *ctr1 = new ConstraintMinGapsBetweenActivities(50.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	acts.clear();
	acts << 23456 << 34567;
	ConstraintMinGapsBetweenActivities *ctr2 = new ConstraintMinGapsBetweenActivities(75.0, 2, acts, 4);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[1].count(), 2);
	QCOMPARE(mgba.minGaps[1].count(), 2);
	QCOMPARE(mgba.weightPercentages[1].count(), 2);

	QCOMPARE(mgba.activities[2].count(), 1);
	QCOMPARE(mgba.minGaps[2].count(), 1);
	QCOMPARE(mgba.weightPercentages[2].count(), 1);

	QCOMPARE(mgba.activities[2][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mgba.minGaps[2][0], 4);
	QCOMPARE(mgba.weightPercentages[2][0], 75.0);
}

void GeneratePreTest::MinGaps_FailIfActivityMinGapsToItself()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;

	QSKIP("Disable test due to usage of assert in Rules::addTimeConstraint()");

	ConstraintMinGapsBetweenActivities *ctr1 = new ConstraintMinGapsBetweenActivities(80.0, 2, acts, 5);
	mock.rules.addTimeConstraint(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MinGapsBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinGaps_FailIfActivityMinGapsToItself_v2()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMinGapsBetweenActivities *ctr1 = new ConstraintMinGapsBetweenActivities(80.0, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);
	QVERIFY2(result == false, "Should not accept constraint MinGapsBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinGaps_NumErrorMsgs_WhenPreparationOfMinGapsFails()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMinGapsBetweenActivities *ctr1 = new ConstraintMinGapsBetweenActivities(80.0, 2, acts, 5);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	mgba.prepare(mock.rules);
	QCOMPARE(mgba.getErrors().count(), 1);

	acts.clear();
	acts << 23456 << 23456;
	ConstraintMinGapsBetweenActivities *ctr2 = new ConstraintMinGapsBetweenActivities(100.0, 2, acts, 3);
	mock.rules.timeConstraintsList.append(ctr2);
	mock.rules.computeInternalStructure(NULL);

	mgba.prepare(mock.rules);
	QCOMPARE(mgba.getErrors().count(), 2);
}

void GeneratePreTest::MinGaps_ThreeActivitiesAtOnce()
{
	MockRules3Activities mock;

	QList<int> acts;
	acts << 12345 << 23456 << 34567;
	ConstraintMinGapsBetweenActivities *ctr1 = new ConstraintMinGapsBetweenActivities(80.0, 3, acts, 2);
	mock.rules.timeConstraintsList.append(ctr1);
	mock.rules.computeInternalStructure(NULL);

	MinGapsBetweenActivities mgba;

	bool result = mgba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinGaps constraint list");

	QCOMPARE(mgba.activities.getD1(), 3);

	QCOMPARE(mgba.activities[0].count(), 2);

	QCOMPARE(mgba.activities[0][0], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mgba.minGaps[0][0], 2);
	QCOMPARE(mgba.weightPercentages[0][0], 80.0);

	QCOMPARE(mgba.activities[0][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mgba.minGaps[0][1], 2);
	QCOMPARE(mgba.weightPercentages[0][1], 80.0);

	QCOMPARE(mgba.activities[1].count(), 2);

	QCOMPARE(mgba.activities[1][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mgba.minGaps[1][0], 2);
	QCOMPARE(mgba.weightPercentages[1][0], 80.0);

	QCOMPARE(mgba.activities[1][1], mock.rules.activitiesHash.value(34567, -1));
	QCOMPARE(mgba.minGaps[1][1], 2);
	QCOMPARE(mgba.weightPercentages[1][1], 80.0);

	QCOMPARE(mgba.activities[2].count(), 2);

	QCOMPARE(mgba.activities[2][0], mock.rules.activitiesHash.value(12345, -1));
	QCOMPARE(mgba.minGaps[2][0], 2);
	QCOMPARE(mgba.weightPercentages[2][0], 80.0);

	QCOMPARE(mgba.activities[2][1], mock.rules.activitiesHash.value(23456, -1));
	QCOMPARE(mgba.minGaps[2][1], 2);
	QCOMPARE(mgba.weightPercentages[2][1], 80.0);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_ReturnOkIfNoTeachersMinContinuousGapInIntervalConstraint()
{
	MockRules3Activities mock;

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_InactiveConstraintIsIgnored()
{
	MockRules3Activities mock;
	mock.addTeachers(2);

	ConstraintTeachersMinContinuousGapInInterval *ctr = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 1, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr->active = false;
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[1][0].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][2].weightPercentage, -1.0);

	ConstraintTeacherMinContinuousGapInInterval *ctr2 = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t2", 1, 5);
	mock.rules.addTimeConstraint(ctr2);
	ctr2->active = false;
	mock.rules.computeInternalStructure(NULL);

	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[1][0].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][2].weightPercentage, -1.0);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_TeachersAffectAll()
{
	MockRules3Activities mock;
	mock.addTeachers(2);

	ConstraintTeachersMinContinuousGapInInterval *ctr = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 1, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[1][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[1][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][2].weightPercentage, -1.0);

//	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
//	QCOMPARE(mcgi.data[0][0].startHour, 1);
//	QCOMPARE(mcgi.data[0][0].endHour, 5);

//	QCOMPARE(mcgi.data[1][0].minGapDuration, 2);
//	QCOMPARE(mcgi.data[1][0].startHour, 1);
//	QCOMPARE(mcgi.data[1][0].endHour, 5);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_TeacherAffectOnlyOne()
{
	MockRules3Activities mock;
	mock.addTeachers(2);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t2", 1, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[1][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[1][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[1][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[1][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[1][0].startHour, 1);
	QCOMPARE(mcgi.data[1][0].endHour, 5);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_TeacherHasMoreThanOneOfThisConstraint()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 3, "t1", 3, 7);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 5);

	QCOMPARE(mcgi.data[0][1].minGapDuration, 3);
	QCOMPARE(mcgi.data[0][1].startHour, 3);
	QCOMPARE(mcgi.data[0][1].endHour, 7);

	ConstraintTeachersMinContinuousGapInInterval *ctr2 = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 4, 9);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, 100.0);

	QCOMPARE(mcgi.data[0][2].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][2].startHour, 4);
	QCOMPARE(mcgi.data[0][2].endHour, 9);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_NumErrorMsgs_WhenPreparationOfTeachersMinContinuousGapInIntervalFails()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 3, "t1", 3, 7);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 4, "t1", 5, 10);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 4, 8);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(!result, "Could not compute MinContinuousGapsForTeachers constraint list");
	QCOMPARE(mcgi.getErrors().count(), 1);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_MoreThanOneToSameTeacherWithSameData_StoresOnlyMostRestrictiveOne()
{
	MockRules3Activities mock;
	mock.addTeachers(2);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(90.0, 2, "t1", 1, 5);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(50.0, 2, "t1", 1, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 90.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 5);

	ConstraintTeachersMinContinuousGapInInterval *ctr2 = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 1, 5);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 5);

	QCOMPARE(mcgi.data[1][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[1][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[1][0].startHour, 1);
	QCOMPARE(mcgi.data[1][0].endHour, 5);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_StoresMostRestrictiveCase()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 2, 6);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 2);
	QCOMPARE(mcgi.data[0][0].endHour, 6);

	ConstraintTeachersMinContinuousGapInInterval *ctr2 = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 2, 5);
	mock.rules.addTimeConstraint(ctr2);
	mock.rules.computeInternalStructure(NULL);

	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 2);
	QCOMPARE(mcgi.data[0][0].endHour, 5);

	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 3, 5);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, -1.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 3);
	QCOMPARE(mcgi.data[0][0].endHour, 5);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_WithSameIntervalOfSameTeacher_WithWeight100_StoresLeastGap()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 3, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(90.0, 2, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 90.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 3);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 7);

	QCOMPARE(mcgi.data[0][1].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][1].startHour, 1);
	QCOMPARE(mcgi.data[0][1].endHour, 7);

	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 4, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);
	result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 90.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 4);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 7);

	QCOMPARE(mcgi.data[0][1].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][1].startHour, 1);
	QCOMPARE(mcgi.data[0][1].endHour, 7);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_WithWeightNot100_StoresBoth()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(90.0, 2, "t1", 2, 6);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 90.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 7);

	QCOMPARE(mcgi.data[0][1].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][1].startHour, 2);
	QCOMPARE(mcgi.data[0][1].endHour, 6);
}

void GeneratePreTest::TeachersMinContinuousGapInInterval_OneThatIncludesAnotherOfSameTeacher_WithDiffMinGap_StoresBoth()
{
	MockRules3Activities mock;
	mock.addTeachers(1);

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 7);
	mock.rules.addTimeConstraint(ctr);
	ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 3, "t1", 2, 6);
	mock.rules.addTimeConstraint(ctr);
	mock.rules.computeInternalStructure(NULL);

	MinContinuousGapInIntervalForTeachers mcgi;

	bool result = mcgi.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MinContinuousGapsForTeachers constraint list");

	QCOMPARE(mcgi.data[0][0].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][1].weightPercentage, 100.0);
	QCOMPARE(mcgi.data[0][2].weightPercentage, -1.0);

	QCOMPARE(mcgi.data[0][0].minGapDuration, 2);
	QCOMPARE(mcgi.data[0][0].startHour, 1);
	QCOMPARE(mcgi.data[0][0].endHour, 7);
}

GeneratePreTest::MockRules3Activities::MockRules3Activities()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	rules.init();
	Subject *subject = new Subject();
	subject->name = "Subject1";
	rules.addSubjectFast(subject);
	Subject *subject2 = new Subject();
	subject2->name = "Subject2";
	rules.addSubjectFast(subject2);
	//Activity *act = new Activity(1, 1, teachers, "Subject", activitytags, studentsnames, 1, 1, true, true, -1, -1);
	rules.addSimpleActivityFast(NULL, 12345, 12345, teachers, "Subject1", activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 23456, 23456, teachers, "Subject2", activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 34567, 34567, QStringList(), "Subject1", QStringList(), QStringList(), 1, 1, true, false, 10, 10);
	rules.computeInternalStructure(NULL);
}

void GeneratePreTest::MockRules3Activities::addTeachers(int n)
{
	for (int i = 1; i <= n; i++) {
		Teacher *t = new Teacher;
		t->name = QString("t%1").arg(i);
		bool ok = rules.addTeacher(t);
		QVERIFY2(ok, "Teacher not inserted");
	}
}

QTEST_APPLESS_MAIN(GeneratePreTest)

#include "tst_generatepretest.moc"
