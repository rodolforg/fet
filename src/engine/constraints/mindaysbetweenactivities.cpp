#include "mindaysbetweenactivities.h"

#include "timetable.h"
extern Timetable gt;

#include "generate_pre.h"

QStringList MinDaysBetweenActivities::getErrors() const
{
	return errors;
}

MinDaysBetweenActivities::MinDaysBetweenActivities()
{

}

bool MinDaysBetweenActivities::prepare()
{
	activities.resize(gt.rules.nInternalActivities);
	minDays.resize(gt.rules.nInternalActivities);
	weightPercentages.resize(gt.rules.nInternalActivities);
	consecutiveIfSameDay.resize(gt.rules.nInternalActivities);

	errors.clear();

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		activities[j].clear();
		minDays[j].clear();
		consecutiveIfSameDay[j].clear();
		weightPercentages[j].clear();
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
								errors.append(
											GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min days with duplicate activities. The constraint "
											"is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules))
											);
								continue;
							}
						}
						int m=md->minDays;

						activities[ai1].append(ai2);
						minDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						weightPercentages[ai1].append(md->weightPercentage);
						consecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	return ok;
}
