#include "groupactivitiesininitialorderitem.h"
#include "rules.h"

QString getActivityDetailedDescription(const Rules& r, int id); //Implemented in timeconstraint.cpp

GroupActivitiesInInitialOrderItem::GroupActivitiesInInitialOrderItem() :
	active(true)
{
}

GroupActivitiesInInitialOrderItem::~GroupActivitiesInInitialOrderItem()
{
}

void GroupActivitiesInInitialOrderItem::removeUseless(const Rules& r)
{
	QList<int> tmpList;

	for(int id : qAsConst(ids)){
		Activity* act=r.activitiesPointerHash.value(id, NULL);
		if(act!=NULL)
			tmpList.append(id);
	}

	ids=tmpList;
}

QString GroupActivitiesInInitialOrderItem::getXmlDescription() const
{
	QString s;

	s+="<GroupActivitiesInInitialOrder>\n";
	s+="	<Number_of_Activities>"+CustomFETString::number(ids.count())+"</Number_of_Activities>\n";
	for(int id : qAsConst(ids))
		s+=QString("	<Activity_Id>")+CustomFETString::number(id)+QString("</Activity_Id>\n");

	s+="	<Active>";
	if(this->active==true)
		s+="true";
	else
		s+="false";
	s+="</Active>\n";

	s+="	<Comments>"+protect(comments)+"</Comments>\n";

	s+="</GroupActivitiesInInitialOrder>\n";

	return s;
}

QString GroupActivitiesInInitialOrderItem::getDescription() const
{
	QString s;
	if(!active)
		s+="X - ";

	s+=tr("Group activities in initial order item");
	s+=QString(", ");
	s+=tr("NA:%1", "Number of activities").arg(ids.count());
	for(int id : qAsConst(ids))
		s+=QString(", ")+tr("Id:%1", "Id of activity").arg(id);

	if(!comments.isEmpty())
		s+=", "+tr("C: %1", "Comments").arg(comments);

	return s;
}

QString GroupActivitiesInInitialOrderItem::getDetailedDescription(const Rules &r) const
{
	QString s=tr("Timetable generation option"); s+=QString("\n");
	s+=tr("Group activities in initial order item"); s+=QString("\n");
	s+=tr("Number of activities=%1").arg(ids.count()); s+=QString("\n");
	for(int id : qAsConst(ids)){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+=QString("\n");
	}

	//Not active?
	QString activeYesNo;
	if(this->active==true)
		activeYesNo=tr("yes");
	else
		activeYesNo=tr("no");
	if(!active){
		s+=tr("Active=%1", "Represents a boolean value, if a 'group activities in initial order' item is active or not, %1 is yes or no").arg(activeYesNo);
		s+="\n";
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}
