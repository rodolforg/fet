/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QtTest>

#include "constraints/paradoxorderedvsreversedconsecutive.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp that yet require it. It must be not used in tests

class ParadoxOrderedVsReversedConsecutiveTest : public QObject
{
	Q_OBJECT

public:
	ParadoxOrderedVsReversedConsecutiveTest();

private:
	Rules &rules;
	void populate_basic_data();

private Q_SLOTS:
	void init();
	void cleanup();
	void test_redundant_prints_no_error_message();
	void test_generate_error_message();
	void test_generate_error_message_reverse_order();
	void test_generate_multiple_error_messages();
	void test_reset_properly();
	void test_added_to_generate();
};

ParadoxOrderedVsReversedConsecutiveTest::ParadoxOrderedVsReversedConsecutiveTest()
	: rules(gt.rules)
{
}

void ParadoxOrderedVsReversedConsecutiveTest::populate_basic_data()
{
	QStringList teachers;
	QStringList activitytags;
	QStringList studentsnames;
	QString subject("subject1");

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
}

void ParadoxOrderedVsReversedConsecutiveTest::init()
{
	rules.init();
}

void ParadoxOrderedVsReversedConsecutiveTest::cleanup()
{
	rules.kill();
}

void ParadoxOrderedVsReversedConsecutiveTest::test_redundant_prints_no_error_message()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxOrderedVsReversedConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(result, "Paradox wrongly found");

	QCOMPARE(paradox.getErrors().size(), 0);
}

void ParadoxOrderedVsReversedConsecutiveTest::test_generate_error_message()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 23456, 12345);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxOrderedVsReversedConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxOrderedVsReversedConsecutiveTest::test_generate_error_message_reverse_order()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 23456, 12345);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxOrderedVsReversedConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxOrderedVsReversedConsecutiveTest::test_generate_multiple_error_messages()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 23456, 12345);
	rules.addTimeConstraint(ctr);

	ctr = new ConstraintTwoActivitiesOrdered(100, 23456, 34567);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 34567, 23456);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxOrderedVsReversedConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 2);
}

void ParadoxOrderedVsReversedConsecutiveTest::test_reset_properly()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 23456, 12345);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	ParadoxOrderedVsReversedConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxOrderedVsReversedConsecutiveTest::test_added_to_generate()
{
	populate_basic_data();

	TimeConstraint* ctr = new ConstraintTwoActivitiesOrdered(100, 12345, 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintTwoActivitiesConsecutive(100, 23456, 12345);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure();

	bool success = processTimeSpaceConstraints(NULL);
	QVERIFY2(!success, "Paradox not checked in generate_pre");
}

QTEST_APPLESS_MAIN(ParadoxOrderedVsReversedConsecutiveTest)

#include "tst_paradoxorderedvsreversedconsecutivetest.moc"
