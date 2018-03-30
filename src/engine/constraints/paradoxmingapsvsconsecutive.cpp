/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "paradoxmingapsvsconsecutive.h"
#include "generate_pre.h"

ParadoxMinGapsVsConsecutive::ParadoxMinGapsVsConsecutive()
{

}

bool ParadoxMinGapsVsConsecutive::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < minGapsBetweenActivitiesList.weightPercentages[ai1].size(); i++){
			if (minGapsBetweenActivitiesList.weightPercentages[ai1][i] > 0) {
				int ai2 = minGapsBetweenActivitiesList.activities[ai1][i];
				// search for contradictory constraints: ai1 and ai2 must be consecutive
				for (int mai1 = 0; mai1 < constrTwoActivitiesConsecutiveActivities[ai1].size(); mai1++) {
					if (constrTwoActivitiesConsecutiveActivities[ai1][mai1] == ai2 && constrTwoActivitiesConsecutivePercentages[ai1][mai1] > 0) {
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be consecutive activities, but there shall be %1 minimum gaps between them.")
								.arg(minGapsBetweenActivitiesList.minGaps[ai1][mai1]);
						s+=QString("\n----\n%1\n----\n%2")
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
	return ok;
}
