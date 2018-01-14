#ifndef GROUPACTIVITIESININITIALORDERITEM_H
#define GROUPACTIVITIESININITIALORDERITEM_H

#include <QCoreApplication>
#include <QString>
#include <QList>

class Rules;

class GroupActivitiesInInitialOrderItem{
	Q_DECLARE_TR_FUNCTIONS(GroupActivitiesInInitialOrderItem)

public:
	GroupActivitiesInInitialOrderItem();
	~GroupActivitiesInInitialOrderItem();

	QString comments;
	bool active;

	QList<int> ids;
	QList<int> indices; //in the rules internal activities list

	QString getXmlDescription() const;
	QString getDescription() const;
	QString getDetailedDescription(const Rules& r) const;

	void removeUseless(const Rules &r);
};

typedef QList<GroupActivitiesInInitialOrderItem*> GroupActivitiesInInitialOrderList;


#endif // GROUPACTIVITIESININITIALORDERITEM_H
