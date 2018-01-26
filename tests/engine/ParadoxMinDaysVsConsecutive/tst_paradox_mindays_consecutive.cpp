#include <QtTest>

#include "constraints/paradoxmindaysvsconsecutive.h"

#include "generate_pre.h"
#include "timetable.h"
Timetable gt; // Just to please generate_pre.cpp and lockunlock.cpp that yet require it. It must be not used in tests

class ParadoxMindaysVsConsecutiveTest : public QObject
{
	Q_OBJECT

public:
	ParadoxMindaysVsConsecutiveTest();
	~ParadoxMindaysVsConsecutiveTest();

private slots:
	void init();
	void cleanup();
	void test_generate_error_message();
	void test_generate_multiple_error_messages();
	void test_reset_properly();

};

ParadoxMindaysVsConsecutiveTest::ParadoxMindaysVsConsecutiveTest()
{

}

ParadoxMindaysVsConsecutiveTest::~ParadoxMindaysVsConsecutiveTest()
{

}

void ParadoxMindaysVsConsecutiveTest::init()
{
	gt.rules.init();
}

void ParadoxMindaysVsConsecutiveTest::cleanup()
{
	gt.rules.kill();
}

void ParadoxMindaysVsConsecutiveTest::test_generate_error_message()
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
	rules.computeInternalStructure(NULL);

	ConstraintBasicCompulsoryTime * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities *ctr_min = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr_min);
	ConstraintTwoActivitiesConsecutive *ctr_consecutive = new ConstraintTwoActivitiesConsecutive(100, 12345, 23456);
	rules.addTimeConstraint(ctr_consecutive);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

void ParadoxMindaysVsConsecutiveTest::test_generate_multiple_error_messages()
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
	rules.computeInternalStructure(NULL);

	ConstraintBasicCompulsoryTime * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities *ctr_min = new ConstraintMinDaysBetweenActivities(100, false, 3, acts, 1);
	rules.addTimeConstraint(ctr_min);
	ConstraintTwoActivitiesConsecutive *ctr_consecutive = new ConstraintTwoActivitiesConsecutive(100, 12345, 23456);
	rules.addTimeConstraint(ctr_consecutive);
	ConstraintTwoActivitiesConsecutive *ctr_consecutive2 = new ConstraintTwoActivitiesConsecutive(100, 34567, 23456);
	rules.addTimeConstraint(ctr_consecutive2);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 2);
}

void ParadoxMindaysVsConsecutiveTest::test_reset_properly()
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
	rules.computeInternalStructure(NULL);

	ConstraintBasicCompulsoryTime * ctr = new ConstraintBasicCompulsoryTime();
	rules.addTimeConstraint(ctr);
	ConstraintBasicCompulsorySpace * sctr = new ConstraintBasicCompulsorySpace();
	rules.addSpaceConstraint(sctr);
	ConstraintMinDaysBetweenActivities *ctr_min = new ConstraintMinDaysBetweenActivities(100, false, 2, acts, 1);
	rules.addTimeConstraint(ctr_min);
	ConstraintTwoActivitiesConsecutive *ctr_consecutive = new ConstraintTwoActivitiesConsecutive(100, 12345, 23456);
	rules.addTimeConstraint(ctr_consecutive);
	rules.computeInternalStructure(NULL);

	ParadoxMinDaysVsConsecutive paradox;
	processTimeSpaceConstraints(NULL);
	bool result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);

	result = paradox.prepare(rules);
	QVERIFY2(!result, "Paradox found");

	QCOMPARE(paradox.getErrors().size(), 1);
}

QTEST_APPLESS_MAIN(ParadoxMindaysVsConsecutiveTest)

#include "tst_paradox_mindays_consecutive.moc"
