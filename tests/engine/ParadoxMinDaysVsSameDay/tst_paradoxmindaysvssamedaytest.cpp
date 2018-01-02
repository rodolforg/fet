#include <QtTest>

#include "constraints/paradoxmindaysvssameday.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp and lockunlock.cpp that yet require it. It must be not used in tests

class ParadoxMinDaysVsSameDayTest : public QObject
{
	Q_OBJECT

public:
	ParadoxMinDaysVsSameDayTest();
	~ParadoxMinDaysVsSameDayTest();

private slots:
	void init();
	void cleanup();
	void test_generate_error_message();
	void test_generate_multiple_error_messages();
	void test_reset_properly();
};

ParadoxMinDaysVsSameDayTest::ParadoxMinDaysVsSameDayTest()
{

}

ParadoxMinDaysVsSameDayTest::~ParadoxMinDaysVsSameDayTest()
{

}

void ParadoxMinDaysVsSameDayTest::init()
{
	gt.rules.init();
}

void ParadoxMinDaysVsSameDayTest::cleanup()
{
	gt.rules.kill();
}

void ParadoxMinDaysVsSameDayTest::test_generate_error_message()
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
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintActivitiesSameStartingDay(100, 2, acts);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsSameDay paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMinDaysVsSameDayTest::test_generate_multiple_error_messages()
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
	ctr = new ConstraintMinDaysBetweenActivities(100, false, 3, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintActivitiesSameStartingDay(100, 2, QList<int>() << 12345 << 23456);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintActivitiesSameStartingDay(100, 2, QList<int>() << 34567 << 23456);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsSameDay paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 2);
}

void ParadoxMinDaysVsSameDayTest::test_reset_properly()
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
	rules.computeInternalStructure(NULL);

	TimeConstraint * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ctr = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr);
	ctr = new ConstraintActivitiesSameStartingDay(100, 2, acts);
	rules.addTimeConstraint(ctr);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsSameDay paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

QTEST_APPLESS_MAIN(ParadoxMinDaysVsSameDayTest)

#include "tst_paradoxmindaysvssamedaytest.moc"
