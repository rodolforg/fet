#include "mindaysbetweenactivities.h"

#include "messageboxes.h"

#include "timetable.h"
extern Timetable gt;

#include "generate_pre.h"

MinDaysBetweenActivities::MinDaysBetweenActivities()
{

}

bool MinDaysBetweenActivities::computeMinDays(QWidget *parent)
{
	minDaysListOfActivities.resize(gt.rules.nInternalActivities);
	minDaysListOfMinDays.resize(gt.rules.nInternalActivities);
	minDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);
	minDaysListOfConsecutiveIfSameDay.resize(gt.rules.nInternalActivities);

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minDaysListOfActivities[j].clear();
		minDaysListOfMinDays[j].clear();
		minDaysListOfConsecutiveIfSameDay[j].clear();
		minDaysListOfWeightPercentages[j].clear();
	}

	QSet<ConstraintMinDaysBetweenActivities*> mdset;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* md=
					(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){
							ok=false;

							if(!mdset.contains(md)){
								mdset.insert(md);
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
									   GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min days with duplicate activities. The constraint "
									   "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
									   GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
									   1, 0 );

								if(t==0)
									return ok;
							}
						}
						int m=md->minDays;

						minDaysListOfActivities[ai1].append(ai2);
						minDaysListOfMinDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						minDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						minDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	return ok;
}
