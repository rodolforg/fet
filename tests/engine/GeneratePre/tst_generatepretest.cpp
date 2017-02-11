#include <QString>
#include <QtTest>

#include "generate_pre.h"

Timetable gt;

class GeneratePreTest : public QObject
{
	Q_OBJECT

public:
	GeneratePreTest();

private:
	struct MockRules3Activities
	{
		MockRules3Activities(Rules &rules);
		~MockRules3Activities();
	private:
		Rules &rules;
	};
	Rules * rules;

private slots:
	void initTestCase();
	void cleanupTestCase();
	void MinDays_ReturnOkIfNoMinDaysConstraint();
	void MinDays_InactiveConstraintIsIgnored();
	void MinDays_ComputedSize();
	void MinDays_CheckValues();
	void MinDays_ActivityHasMoreThanOneOfThisConstraint();
	void MinDays_FailIfActivityMinDaysToItself();
	void MinDays_FailIfActivityMinDaysToItself_v2();
	void MinDays_ThreeActivitiesAtOnce();
};

GeneratePreTest::GeneratePreTest()
	: rules(NULL)
{
}

void GeneratePreTest::initTestCase()
{
	VERBOSE = false;
	rules = &gt.rules;
}

void GeneratePreTest::cleanupTestCase()
{
	rules->kill();
}

void GeneratePreTest::MinDays_ReturnOkIfNoMinDaysConstraint()
{
	MockRules3Activities mock(*rules);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");
}

void GeneratePreTest::MinDays_InactiveConstraintIsIgnored()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	rules->addTimeConstraint(ctr);
	ctr->active = false;
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities.getD1(), 3);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[0].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[0].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[0].count(), 0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1].count(), 0);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1].count(), 0);

}

void GeneratePreTest::MinDays_ComputedSize()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	rules->addTimeConstraint(ctr);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities.getD1(), 3);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[0].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[0].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[0].count(), 1);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1].count(), 1);
}

void GeneratePreTest::MinDays_CheckValues()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	rules->addTimeConstraint(ctr);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities.getD1(), 3);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0][0], rules->activitiesHash.value(23456, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[0][0], 5);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[0][0], false);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[0][0], 50.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1][0], rules->activitiesHash.value(12345, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1][0], 5);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1][0], false);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1][0], 50.0);
}

void GeneratePreTest::MinDays_ActivityHasMoreThanOneOfThisConstraint()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 23456;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(50.0, false, 2, acts, 5);
	rules->addTimeConstraint(ctr1);
	acts.clear();
	acts << 23456 << 34567;
	ConstraintMinDaysBetweenActivities *ctr2 = new ConstraintMinDaysBetweenActivities(75.0, true, 2, acts, 4);
	rules->addTimeConstraint(ctr2);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities.getD1(), 3);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1].count(), 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1].count(), 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1].count(), 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1].count(), 2);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[2].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[2].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[2].count(), 1);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[2].count(), 1);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[2][0], rules->activitiesHash.value(23456, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[2][0], 4);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[2][0], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[2][0], 75.0);
}

void GeneratePreTest::MinDays_FailIfActivityMinDaysToItself()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 12345;

	QSKIP("Disable test due to usage of assert in Rules::addTimeConstraint()");

	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, false, 2, acts, 5);
	rules->addTimeConstraint(ctr1);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);
	QVERIFY2(result == false, "Should not accept constraint MinDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinDays_FailIfActivityMinDaysToItself_v2()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 12345;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, false, 2, acts, 5);
	rules->timeConstraintsList.append(ctr1);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);
	QVERIFY2(result == false, "Should not accept constraint MinDaysBetweenActivities if the activities are the same one");
}

void GeneratePreTest::MinDays_ThreeActivitiesAtOnce()
{
	MockRules3Activities mock(*rules);

	QList<int> acts;
	acts << 12345 << 23456 << 34567;
	ConstraintMinDaysBetweenActivities *ctr1 = new ConstraintMinDaysBetweenActivities(80.0, true, 3, acts, 2);
	rules->timeConstraintsList.append(ctr1);
	rules->computeInternalStructure(NULL);

	bool result = minDaysBetweenActivitiesList.computeMinDays(NULL);

	QVERIFY2(result, "Could not compute MinDays constraint list");

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities.getD1(), 3);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0].count(), 2);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0][0], rules->activitiesHash.value(23456, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[0][0], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[0][0], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[0][0], 80.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[0][1], rules->activitiesHash.value(34567, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[0][1], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[0][1], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[0][1], 80.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1].count(), 2);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1][0], rules->activitiesHash.value(12345, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1][0], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1][0], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1][0], 80.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[1][1], rules->activitiesHash.value(34567, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[1][1], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[1][1], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[1][1], 80.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[2].count(), 2);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[2][0], rules->activitiesHash.value(12345, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[2][0], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[2][0], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[2][0], 80.0);

	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfActivities[2][1], rules->activitiesHash.value(23456, -1));
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfMinDays[2][1], 2);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfConsecutiveIfSameDay[2][1], true);
	QCOMPARE(minDaysBetweenActivitiesList.minDaysListOfWeightPercentages[2][1], 80.0);
}

GeneratePreTest::MockRules3Activities::MockRules3Activities(Rules &_rules)
	: rules(_rules)
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

GeneratePreTest::MockRules3Activities::~MockRules3Activities()
{
	rules.kill();
}

QTEST_APPLESS_MAIN(GeneratePreTest)

#include "tst_generatepretest.moc"
