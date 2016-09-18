#include <QString>
#include <QtTest>
#include "engine/randomknuth.h"

class RandomKnuthTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void testCase_data();
	void testCase();
};

void RandomKnuthTest::init()
{
	QVERIFY2(RandomKnuth::getMM()==2147483647, "Value for MM has changed");
	QVERIFY2(RandomKnuth::getAA()==48271, "Value for AA has changed");
	QVERIFY2(RandomKnuth::getQQ()==44488, "Value for QQ has changed");
	QVERIFY2(RandomKnuth::getRR()==3399, "Value for RR has changed");

	QVERIFY2(RandomKnuth::getMMM()==2147483399, "Value for MMM has changed");
	QVERIFY2(RandomKnuth::getMMM()==RandomKnuth::getMM()-248, "Value for MMM is not MM-248");
	QVERIFY2(RandomKnuth::getAAA()==40692, "Value for AAA has changed");
	QVERIFY2(RandomKnuth::getQQQ()==52774, "Value for QQQ has changed");
	QVERIFY2(RandomKnuth::getRRR()==3791, "Value for RRR has changed");
}

void RandomKnuthTest::testCase()
{
	QFETCH(int, iXX);
	QFETCH(int, iYY);
	QFETCH(int, XX);
	QFETCH(int, YY);
	QFETCH(int, tttt);

	RandomKnuth::XX = iXX;
	RandomKnuth::YY = iYY;
	QCOMPARE(RandomKnuth::pick1MM1(), tttt);
	QCOMPARE(RandomKnuth::XX, XX);
	QCOMPARE(RandomKnuth::YY, YY);
}

void RandomKnuthTest::testCase_data()
{
	QTest::addColumn<int>("iXX");
	QTest::addColumn<int>("iYY");
	QTest::addColumn<int>("XX");
	QTest::addColumn<int>("YY");
	QTest::addColumn<int>("tttt");

	QTest::newRow("case 1") << 123 << 123 << 5937333 << 5005116 << 932217;
	QTest::newRow("case 2") << 4321 << 54321 << 208578991 << 62946733 << 145632258;
	QTest::newRow("case 3") << 87654321 << 987654321 << 618944401 << 1625301246 << 1141126801;
	QTest::newRow("case 4") << 1 << 1 << 48271 << 40692 << 7579;
	QTest::newRow("case 5") << RandomKnuth::getMM()-1 << RandomKnuth::getMMM()-1 << 2147435376 << 2147442707 << 2147476315;
	QTest::newRow("case 6") << 100 << 1000 << 4827100 << 40692000 << 2111618746;
}

QTEST_APPLESS_MAIN(RandomKnuthTest)

#include "tst_randomknuthtest.moc"
