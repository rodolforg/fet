#include "paradoxmindaysvsconsecutive.h"
#include "generate_pre.h"

ParadoxMinDaysVsConsecutive::ParadoxMinDaysVsConsecutive()
{

}

bool ParadoxMinDaysVsConsecutive::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < constrTwoActivitiesConsecutivePercentages[ai1].size(); i++){
			if (constrTwoActivitiesConsecutivePercentages[ai1][i] >= 100) {
				int ai2 = constrTwoActivitiesConsecutiveActivities[ai1][i];
				// search for contradictory constraints Min days between ai1 and ai2
				for (int mai1 = 0; mai1 < minDaysBetweenActivitiesList.activities[ai1].size(); mai1++) {
					if (minDaysBetweenActivitiesList.activities[ai1][mai1] == ai2 && minDaysBetweenActivitiesList.weightPercentages[ai1][mai1] >= 100) {
						if (minDaysBetweenActivitiesList.minDays[ai1][mai1] > 0) {
							QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
							 .arg(rules.internalActivitiesList[ai1].id)
							 .arg(rules.internalActivitiesList[ai2].id);
							s+="\n";
							s+=GeneratePreTranslate::tr("There should be %3 min days between them but they must also be consecutive activities.")
							 .arg(minDaysBetweenActivitiesList.minDays[ai1][mai1]);
							s+=QString("\n----\n%4\n----\n%5")
							 .arg(rules.internalActivitiesList[ai1].getDescription())
							 .arg(rules.internalActivitiesList[ai2].getDescription());
							s+="\n\n";
							s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

							errors << ErrorCode(ErrorCode::Error, s, errorGroupId);

							ok = false;
						}
					}
				}
			}
		}
	}
	return ok;
}
