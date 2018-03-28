/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QtTest>

#include "constraints/paradoxmingapsvs3grouped.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp that yet require it. It must be not used in tests

class ParadoxMinGapsVs3GroupedTest : public QObject
{
	Q_OBJECT

public:
	ParadoxMinGapsVs3GroupedTest();

private Q_SLOTS:
	void init();
	void cleanup();
	void test_generate_error_message_first_activity_pair();
	void test_generate_multiple_error_messages();
	void test_reset_properly();
	void test_all_pairs();
	void test_added_to_generate();
};

ParadoxMinGapsVs3GroupedTest::ParadoxMinGapsVs3GroupedTest()
{
}

void ParadoxMinGapsVs3GroupedTest::init()
{
	gt.rules.init();
}

void ParadoxMinGapsVs3GroupedTest::cleanup()
{
	gt.rules.kill();
}

void ParadoxMinGapsVs3GroupedTest::test_generate_error_message_first_activity_pair()
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
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinGapsBetweenActivities * minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVs3Grouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for the first activity pair");
	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinGapsVs3GroupedTest::test_generate_multiple_error_messages()
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
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(45678, 45678, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinGapsBetweenActivities * minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 3, acts, 1);
	rules.addTimeConstraint(minGapsCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVs3Grouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for all three activities");
	QCOMPARE(paradox.getErrors().size(), 3);
}

void ParadoxMinGapsVs3GroupedTest::test_reset_properly()
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
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinGapsBetweenActivities * minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVs3Grouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for the first activity pair");
	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for the first activity pair second time");
	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinGapsVs3GroupedTest::test_all_pairs()
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
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinGapsBetweenActivities * minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 3, acts, 1);
	rules.addTimeConstraint(minGapsCtr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVs3Grouped paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found for all three activities");
	QCOMPARE(paradox.getErrors().size(), 3);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	// First two activities
	acts.clear();
	acts << 12345 << 23456;
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for two first activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for two first activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	// First and third activities
	acts.clear();
	acts << 12345 << 34567;
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for first and third activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for first and third activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	// Last two activities
	acts.clear();
	acts << 23456 << 34567;
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for two last activities");
	QCOMPARE(paradox.getErrors().size(), 1);

	QVERIFY2(rules.removeTimeConstraint(minGapsCtr), "Min Gaps Constraint should exist!");

	//    in reversed order
	acts.swap(0, 1);
	minGapsCtr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(minGapsCtr);

	rules.computeInternalStructure();
	processTimeSpaceConstraints(NULL);
	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox not found with min-gaps for two last activities in reverse order");
	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinGapsVs3GroupedTest::test_added_to_generate()
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
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(34567, 34567, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintThreeActivitiesGrouped(100, 12345, 23456, 34567);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	bool success = processTimeSpaceConstraints(NULL);
	QVERIFY2(!success, "Paradox not checked in generate_pre");
}

QTEST_APPLESS_MAIN(ParadoxMinGapsVs3GroupedTest)

#include "tst_paradoxmingapsvs3groupedtest.moc"
