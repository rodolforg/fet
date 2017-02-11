#include <QString>
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

	void MaxDays_ReturnOkIfNoMinDaysConstraint();
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

void GeneratePreTest::MaxDays_ReturnOkIfNoMinDaysConstraint()
{
	MockRules3Activities mock;

	MaxDaysBetweenActivities mdba;

	bool result = mdba.prepare(mock.rules);

	QVERIFY2(result, "Could not compute MaxDays constraint list");
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

QTEST_APPLESS_MAIN(GeneratePreTest)

#include "tst_generatepretest.moc"
