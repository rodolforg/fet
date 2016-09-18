#include "randomknuth.h"
#include <cassert>
#include <QDateTime>
#include <QtGlobal>

int RandomKnuth::XX;
int RandomKnuth::YY;
int RandomKnuth::ZZ;

int RandomKnuth::getAA()
{
	return AA;
}

int RandomKnuth::getQQ()
{
	return QQ;
}

int RandomKnuth::getRR()
{
	return RR;
}

int RandomKnuth::getAAA()
{
	return AAA;
}

int RandomKnuth::getQQQ()
{
	return QQQ;
}

int RandomKnuth::getRRR()
{
	return RRR;
}

int RandomKnuth::getMMM()
{
	return MMM;
}

int RandomKnuth::getMM()
{
	return MM;
}

void RandomKnuth::init()
{
	qint64 tt = QDateTime::currentMSecsSinceEpoch();

	//XX is the current time
	//XX = 1 + ( (unsigned(tt)) % (unsigned(MM-1)) );
	XX = 1 + int( tt % (qint64(MM-1)) );
	assert(XX>0);
	assert(XX<MM);

	//YY is the next random, after initializing YY with the current time
	//YY = 1 + ( (unsigned(tt)) % (unsigned(MMM-1)) );
	YY = 1 + int( tt % (qint64(MMM-1)) );
	assert(YY>0);
	assert(YY<MMM);
	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;
	assert(YY>0);
	assert(YY<MMM);

	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).
}

int RandomKnuth::pick1MM1()
{
	assert(XX>0);
	assert(XX<MM);

	XX=AA*(XX%QQ)-RR*(XX/QQ);
	if(XX<0)
		XX+=MM;

	assert(XX>0);
	assert(XX<MM);

	assert(YY>0);
	assert(YY<MMM);

	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;

	assert(YY>0);
	assert(YY<MMM);

	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).

	return ZZ;
}

int RandomKnuth::pick(int k)
{
	//like in Knuth TAOCP vol.2, reject some numbers (very few), so that the distribution is perfectly uniform
	for(;;){
		int U=RandomKnuth::pick1MM1();
		if( U <= k * ((MM-1)/k) )
			return U % k;
	}
}

