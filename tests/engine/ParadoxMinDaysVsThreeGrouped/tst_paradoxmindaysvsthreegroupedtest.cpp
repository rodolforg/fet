#include <QString>
#include <QtTest>

#include "constraints/paradoxmindaysvsthreegrouped.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp and lockunlock.cpp that yet require it. It must be not used in tests

class ParadoxMinDaysVsThreeGroupedTest : public QObject
{
	Q_OBJECT

public:
	ParadoxMinDaysVsThreeGroupedTest();

private Q_SLOTS:
	void init();
	void cleanup();
	void test_generate_error_message_first_actvity_pair();
	void test_generate_multiple_error_messages();
	void test_reset_properly();
	void test_all_pairs();
};

ParadoxMinDaysVsThreeGroupedTest::ParadoxMinDaysVsThreeGroupedTest()
{
}

void ParadoxMinDaysVsThreeGroupedTest::init()
{
	gt.rules.init();
}

void ParadoxMinDaysVsThreeGroupedTest::cleanup()
{
	gt.rules.kill();
}

void ParadoxMinDaysVsThreeGroupedTest::test_generate_error_message_first_actvity_pair()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");
	QList<int> acts;
	acts << 12345 << 23456;

	Rules &rules = gt.rules;
	if (!rules.initialized)
		rules.init();
	Subject *psubject = new Subject();
	psubject->name = subject;
	rules.addSubjectFast(psubject);
	rules.addSimpleActivityFast(NULL, 12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities * minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsThreeGrouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for the first activity pair");
	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinDaysVsThreeGroupedTest::test_generate_multiple_error_messages()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");
	QList<int> acts;
	acts << 12345 << 23456 << 34567;

	Rules &rules = gt.rules;
	if (!rules.initialized)
		rules.init();
	Subject *psubject = new Subject();
	psubject->name = subject;
	rules.addSubjectFast(psubject);
	rules.addSimpleActivityFast(NULL, 12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 45678, 45678, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities * minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 3, acts, 1);
	rules.addTimeConstraint(minDaysCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsThreeGrouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for all three activities");
	QCOMPARE(paradox.getErrors().size(), 3);
}

void ParadoxMinDaysVsThreeGroupedTest::test_reset_properly()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");
	QList<int> acts;
	acts << 12345 << 23456;

	Rules &rules = gt.rules;
	if (!rules.initialized)
		rules.init();
	Subject *psubject = new Subject();
	psubject->name = subject;
	rules.addSubjectFast(psubject);
	rules.addSimpleActivityFast(NULL, 12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsThreeGrouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found");

	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinDaysVsThreeGroupedTest::test_all_pairs()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");
	QList<int> acts;
	acts << 12345 << 23456 << 34567;

	Rules &rules = gt.rules;
	if (!rules.initialized)
		rules.init();
	Subject *psubject = new Subject();
	psubject->name = subject;
	rules.addSubjectFast(psubject);
	rules.addSimpleActivityFast(NULL, 12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(NULL, 34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities * minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 3, acts, 1);
	rules.addTimeConstraint(minDaysCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsThreeGrouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for all three activities");
	QCOMPARE(paradox.getErrors().size(), 3);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	// First two activities
	acts.clear();
	acts << 12345 << 23456;
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for two first activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for two first activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	// First and third activities
	acts.clear();
	acts << 12345 << 34567;
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for first and third activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for first and third activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	// Last two activities
	acts.clear();
	acts << 23456 << 34567;
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for two last activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minDaysCtr), "Min Days Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minDaysCtr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(minDaysCtr);

	rules.computeInternalStructure(NULL);
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-days for two last activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);
}


QTEST_APPLESS_MAIN(ParadoxMinDaysVsThreeGroupedTest)

#include "tst_paradoxmindaysvsthreegroupedtest.moc"
