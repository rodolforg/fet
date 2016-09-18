#ifndef RANDOMKNUTH_H
#define RANDOMKNUTH_H


class RandomKnuth
{
public:
	static int XX;
	static int YY;
private:
	static int ZZ;

	//for random Knuth - from Knuth TAOCP Vol. 2 Seminumerical Algorithms section 3.6
	//these numbers are really important - please do not change them, NEVER!!!
	//if you want, write a new random number generator routine, with other name
	//I think I found a minor possible improvement, the author said: if(Z<=0) then Z+=MM,
	//but I think this would be better: if(Z<=0) then Z+=MM-1. - Yes, the author confirmed
private:
	static const int MM=2147483647;
	static const int AA=48271;
	static const int QQ=44488;
	static const int RR=3399;

	static const int MMM=2147483399;
	static const int AAA=40692;
	static const int QQQ=52774;
	static const int RRR=3791;

public:
	static void init();
	static int pick1MM1(); //a random between 1 and MM-1
	static int pick(int k); //a random between 0 and k-1
	static int getMM();
	static int getAA();
	static int getQQ();
	static int getRR();
	static int getMMM();
	static int getAAA();
	static int getQQQ();
	static int getRRR();
};

#endif // RANDOMKNUTH_H
