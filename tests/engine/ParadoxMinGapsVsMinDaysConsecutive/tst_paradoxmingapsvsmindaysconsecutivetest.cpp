/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QtTest>

#include "constraints/paradoxmingapsvsmindaysconsecutive.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp that yet require it. It must be not used in tests

class ParadoxMinGapsVsMinDaysConsecutiveTest : public QObject
{
	Q_OBJECT

public:
	ParadoxMinGapsVsMinDaysConsecutiveTest();

private Q_SLOTS:
	void init();
	void cleanup();
	void test_no_error_if_not_consecutive_flag();
	void test_generate_error_message();
	void test_generate_error_message_even_when_activity_order_is_reversed();
	void test_generate_multiple_error_messages();
	void test_reset_properly();
};

ParadoxMinGapsVsMinDaysConsecutiveTest::ParadoxMinGapsVsMinDaysConsecutiveTest()
{
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::init()
{
	gt.rules.init();
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::cleanup()
{
	gt.rules.kill();
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::test_no_error_if_not_consecutive_flag()
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
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVsMinDaysConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 0);
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::test_generate_error_message()
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
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, true, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVsMinDaysConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::test_generate_error_message_even_when_activity_order_is_reversed()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");
	QList<int> acts;
	acts << 23456 << 12345; // reversed order

	Rules &rules = gt.rules;
	if (!rules.initialized)
		rules.init();
	Subject *psubject = new Subject();
	psubject->name = subject;
	rules.addSubjectFast(psubject);
	rules.addSimpleActivityFast(12345, 12345, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.addSimpleActivityFast(23456, 23456, teachers, subject, activitytags, studentsnames, 1, 1, true, false, 10, 10);
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, true, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVsMinDaysConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::test_generate_multiple_error_messages()
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
	ctr = new ConstraintMinDaysBetweenActivities(100, true, 3, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 3, acts, 1);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVsMinDaysConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 3);
}

void ParadoxMinGapsVsMinDaysConsecutiveTest::test_reset_properly()
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
	rules.computeInternalStructure();

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, true, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintMinGapsBetweenActivities(100, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxMinGapsVsMinDaysConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

QTEST_APPLESS_MAIN(ParadoxMinGapsVsMinDaysConsecutiveTest)

#include "tst_paradoxmingapsvsmindaysconsecutivetest.moc"
