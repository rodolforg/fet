/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QtTest>

#include "timeconstraint.h"
#include "rules.h"
#include "solution.h"

#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp that yet require it. It must be not used in tests

class ConstraintTeachersMinContinuousGapInIntervalTest : public QObject
{
	Q_OBJECT

public:
	ConstraintTeachersMinContinuousGapInIntervalTest();

private Q_SLOTS:
	void OneTeacher_CheckInvalidValues();
	void AllTeachers_CheckInvalidValues();
	void OneTeacher_FitnessAcceptsNullPointer();
	void AllTeachers_FitnessAcceptsNullPointer();
	void OneTeacher_CheckFitness();
	void AllTeachers_CheckFitness();
	void OneTeacher_CheckFitness_EnoughGapButNotContinuous();
	void AllTeachers_CheckFitness_EnoughGapButNotContinuous();
	// TODO check read/write xml
};

ConstraintTeachersMinContinuousGapInIntervalTest::ConstraintTeachersMinContinuousGapInIntervalTest()
{
}

void ConstraintTeachersMinContinuousGapInIntervalTest::OneTeacher_CheckInvalidValues()
{
	Rules rules;
	rules.init();
	Teacher *t = new Teacher();
	t->name = "t1";
	rules.addTeacher(t);
	rules.computeInternalStructure();

	ConstraintTeacherMinContinuousGapInInterval ctr(100.0, 2, "t1", 1, 5);

	ctr.weightPercentage = 120;
	ErrorCode result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted weight > 100%");

	ctr.weightPercentage = -2;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted weight < 0%");

	ctr.weightPercentage = 100;

	ctr.minGapDuration = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted min continuous gap < 0");

	ctr.minGapDuration = ctr.endHour-ctr.startHour+1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted min continuous gap > interval");

	ctr.minGapDuration = 2;

	ctr.startHour = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour < 0");

	ctr.startHour = ctr.endHour+1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour > end hour");

	ctr.startHour = 1;

	ctr.endHour = 0;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour > end hour");

	ctr.endHour = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted end hour < 0");

	ctr.endHour = rules.nHoursPerDay + 1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted end hour > hours per day");

	ctr.endHour = rules.nHoursPerDay;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(!result, "Does not accept end-of-day as end hour");
}

void ConstraintTeachersMinContinuousGapInIntervalTest::AllTeachers_CheckInvalidValues()
{
	Rules rules;
	rules.init();
	rules.computeInternalStructure();

	ConstraintTeachersMinContinuousGapInInterval ctr(100.0, 2, 1, 5);

	ctr.weightPercentage = 120;
	ErrorCode result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted weight > 100%");

	ctr.weightPercentage = -2;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted weight < 0%");

	ctr.weightPercentage = 100;

	ctr.minGapDuration = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted min continuous gap < 0");

	ctr.minGapDuration = ctr.endHour-ctr.startHour+1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted min continuous gap > interval");

	ctr.minGapDuration = 2;

	ctr.startHour = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour < 0");

	ctr.startHour = ctr.endHour+1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour > end hour");

	ctr.startHour = 1;

	ctr.endHour = 0;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted start hour > end hour");

	ctr.endHour = -1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted end hour < 0");

	ctr.endHour = rules.nHoursPerDay + 1;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(result, "Accepted end hour > hours per day");

	ctr.endHour = rules.nHoursPerDay;
	result = ctr.computeInternalStructure(rules);
	QVERIFY2(!result, "Does not accept end-of-day as end hour");
}

void ConstraintTeachersMinContinuousGapInIntervalTest::OneTeacher_FitnessAcceptsNullPointer()
{
	Rules rules;
	rules.init();
	Teacher *t1 = new Teacher();
	t1->name = "t1";
	bool ok = rules.addTeacherFast(t1);
	QVERIFY2(ok, "Could not add teacher");

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 1, 3);
	ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;
	c.setTime(0, 1);

	ctr->fitness(c, rules);
}

void ConstraintTeachersMinContinuousGapInIntervalTest::AllTeachers_FitnessAcceptsNullPointer()
{
	Rules rules;
	rules.init();

	ConstraintTeachersMinContinuousGapInInterval *ctr = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 1, 3);
	bool ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;
	c.setTime(0, 1);

	ctr->fitness(c, rules);
}

void ConstraintTeachersMinContinuousGapInIntervalTest::OneTeacher_CheckFitness()
{
	Rules rules;
	rules.init();
	Teacher *t1 = new Teacher();
	t1->name = "t1";
	rules.addTeacherFast(t1);
	Subject *s1 = new Subject();
	s1->name = "subject1";
	rules.addSubjectFast(s1);

	QStringList tags, students;
	QStringList teachers;
	teachers << t1->name;
	const int duration = 2;
	ErrorList errors = rules.addSimpleActivityFast(1, 1, teachers, "subject1", tags, students, duration, duration, true, false, 0, 0);
	QVERIFY2(!errors.hasError(), "Could not add activity");

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 2, 4);
	bool ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;

	double fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*0+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*1+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 1.0);

	c.setTime(0, rules.nDaysPerWeek*2+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	c.setTime(0, rules.nDaysPerWeek*3+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 1.0);

	c.setTime(0, rules.nDaysPerWeek*4+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);


	c.setTime(0, rules.nDaysPerWeek*0+2);
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*1+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 1.0);

	c.setTime(0, rules.nDaysPerWeek*2+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	c.setTime(0, rules.nDaysPerWeek*3+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 1.0);

	c.setTime(0, rules.nDaysPerWeek*4+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);
}

void ConstraintTeachersMinContinuousGapInIntervalTest::AllTeachers_CheckFitness()
{
	Rules rules;
	rules.init();
	Teacher *t1 = new Teacher();
	t1->name = "t1";
	rules.addTeacherFast(t1);
	t1 = new Teacher();
	t1->name = "t2";
	rules.addTeacherFast(t1);
	Subject *s1 = new Subject();
	s1->name = "subject1";
	rules.addSubjectFast(s1);

	QStringList tags, students;
	QStringList teachers;
	teachers << "t1" << "t2";
	const int duration = 2;
	ErrorList errors = rules.addSimpleActivityFast(1, 1, teachers, "subject1", tags, students, duration, duration, true, false, 0, 0);
	QVERIFY2(!errors.hasError(), "Could not add activity");

	ConstraintTeachersMinContinuousGapInInterval *ctr = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 2, 4);
	bool ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;

	double fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*0+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*1+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	c.setTime(0, rules.nDaysPerWeek*2+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 4.0);

	c.setTime(0, rules.nDaysPerWeek*3+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	c.setTime(0, rules.nDaysPerWeek*4+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);


	c.setTime(0, rules.nDaysPerWeek*0+2);
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*1+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	ConflictInfo conflicts;
	c.setTime(0, rules.nDaysPerWeek*2+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules, &conflicts);
	QCOMPARE(fitness, 4.0);
	QCOMPARE(conflicts.weights.count(), 2);
	QCOMPARE(conflicts.weights[0], 2.0);

	c.setTime(0, rules.nDaysPerWeek*3+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 2.0);

	c.setTime(0, rules.nDaysPerWeek*4+2);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);
}

void ConstraintTeachersMinContinuousGapInIntervalTest::OneTeacher_CheckFitness_EnoughGapButNotContinuous()
{
	Rules rules;
	rules.init();
	Teacher *t1 = new Teacher();
	t1->name = "t1";
	rules.addTeacherFast(t1);
	Subject *s1 = new Subject();
	s1->name = "subject1";
	rules.addSubjectFast(s1);

	QStringList tags, students;
	QStringList teachers;
	teachers << t1->name;
	const int duration = 2;
	ErrorList errors = rules.addSimpleActivityFast(1, 1, teachers, "subject1", tags, students, duration, duration, true, false, 0, 0);
	QVERIFY2(!errors.hasError(), "Could not add activity");

	ConstraintTeacherMinContinuousGapInInterval *ctr = new ConstraintTeacherMinContinuousGapInInterval(100.0, 2, "t1", 2, 6);
	bool ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;

	double fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 0.0);

	c.setTime(0, rules.nDaysPerWeek*3+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules);
	QCOMPARE(fitness, 1.0);
}

void ConstraintTeachersMinContinuousGapInIntervalTest::AllTeachers_CheckFitness_EnoughGapButNotContinuous()
{
	Rules rules;
	rules.init();
	Teacher *t1 = new Teacher();
	t1->name = "t1";
	rules.addTeacherFast(t1);
	t1 = new Teacher();
	t1->name = "t2";
	rules.addTeacherFast(t1);
	Subject *s1 = new Subject();
	s1->name = "subject1";
	rules.addSubjectFast(s1);

	QStringList tags, students;
	QStringList teachers;
	teachers << "t1" << "t2";
	const int duration = 2;
	ErrorList errors = rules.addSimpleActivityFast(1, 1, teachers, "subject1", tags, students, duration, duration, true, false, 0, 0);
	QVERIFY2(!errors.hasError(), "Could not add activity");

	ConstraintTeachersMinContinuousGapInInterval *ctr = new ConstraintTeachersMinContinuousGapInInterval(100.0, 2, 2, 6);
	bool ok = rules.addTimeConstraint(ctr);
	QVERIFY2(ok, "Could not add constraint");

	rules.computeInternalStructure();

	Solution c;

	ConflictInfo info;
	double fitness = ctr->fitness(c, rules, &info);
	QCOMPARE(fitness, 0.0);
	QCOMPARE(info.weights.count(), 0);

	c.setTime(0, rules.nDaysPerWeek*3+0);
	c.resetFitness();
	fitness = ctr->fitness(c, rules, &info);
	QCOMPARE(fitness, 2.0);
	QCOMPARE(info.weights.count(), 2);
	QCOMPARE(info.weights[0], 1.0);
	QCOMPARE(info.descriptions.count(), 2);
}

QTEST_APPLESS_MAIN(ConstraintTeachersMinContinuousGapInIntervalTest)

#include "tst_constraintteachersmincontinuousgapinintervaltest.moc"
