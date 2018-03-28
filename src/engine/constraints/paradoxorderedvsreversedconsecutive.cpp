/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "paradoxorderedvsreversedconsecutive.h"
#include "generate_pre.h"

ParadoxOrderedVsReversedConsecutive::ParadoxOrderedVsReversedConsecutive()
{

}

bool ParadoxOrderedVsReversedConsecutive::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < constrTwoActivitiesOrderedPercentages[ai1].size(); i++){
			if (constrTwoActivitiesOrderedPercentages[ai1][i] >= 100) {
				int ai2 = constrTwoActivitiesOrderedActivities[ai1][i];
				// search for contradictory constraints Two Consecutive Activities in reverse order
				for (int mai2 = 0; mai2 < constrTwoActivitiesConsecutiveActivities[ai2].size(); mai2++) {
					if (constrTwoActivitiesConsecutiveActivities[ai2][mai2] == ai1) {
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be consecutive activities, but there is a constraint Two Ordered Activities that demands a reversed activity order.");
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
