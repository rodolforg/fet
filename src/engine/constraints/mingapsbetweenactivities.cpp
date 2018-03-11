#include "mingapsbetweenactivities.h"

#include "generate_pre.h"

MinGapsBetweenActivities::MinGapsBetweenActivities()
{

}

bool MinGapsBetweenActivities::prepare(const Rules &rules)
{
	activities.resize(rules.nInternalActivities);
	minGaps.resize(rules.nInternalActivities);
	weightPercentages.resize(rules.nInternalActivities);

	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	bool ok=true;

	for(int j=0; j<rules.nInternalActivities; j++){
		activities[j].clear();
		minGaps[j].clear();
		weightPercentages[j].clear();
	}

	QSet<const ConstraintMinGapsBetweenActivities*> mgset;

	for(int i=0; i<rules.nInternalTimeConstraints; i++)
		if(rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES){
			const ConstraintMinGapsBetweenActivities* mg=
			 (const ConstraintMinGapsBetweenActivities*)rules.internalTimeConstraintsList[i];

			assert(mg->_n_activities==mg->_activities.count());

			for(int j=0; j<mg->_n_activities; j++){
				int ai1=mg->_activities[j];
				for(int k=0; k<mg->_n_activities; k++)
					if(j!=k){
						int ai2=mg->_activities[k];
						if(ai1==ai2){
							ok=false;

							if(!mgset.contains(mg)){
								mgset.insert(mg);

								errors.append(ErrorCode(ErrorCode::Error,
											GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min gaps between activities with duplicate activities. The constraint "
											"is: %1. Please correct that.").arg(mg->getDetailedDescription(rules)), errorGroupId)
											);
								continue;
							}
						}
						int m=mg->minGaps;

						activities[ai1].append(ai2);
						minGaps[ai1].append(m);
						assert(mg->weightPercentage >=0 && mg->weightPercentage<=100);
						weightPercentages[ai1].append(mg->weightPercentage);
					}
			}
		}

	return ok;
}
