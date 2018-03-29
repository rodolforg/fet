/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "paradoxconsecutivevsconsecutive.h"
#include "generate_pre.h"

ParadoxConsecutiveVsConsecutive::ParadoxConsecutiveVsConsecutive()
{

}

bool ParadoxConsecutiveVsConsecutive::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();
	QSet<QPair<int,int> > visitedPairs;

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < constrTwoActivitiesConsecutivePercentages[ai1].size(); i++){
			if (constrTwoActivitiesConsecutivePercentages[ai1][i] >= 100) {
				int ai2 = constrTwoActivitiesConsecutiveActivities[ai1][i];
				visitedPairs << QPair<int,int>(ai2, ai1);

				// search for contradictory constraints Two Consecutive Activities in reverse order
				// ai1 -> ai2   &   ai1 <- ai2
				if (!visitedPairs.contains(QPair<int, int>(ai1,ai2))) {
					for (int mai1 = 0; mai1 < inverseConstrTwoActivitiesConsecutiveActivities[ai1].size(); mai1++) {
						if (inverseConstrTwoActivitiesConsecutiveActivities[ai1][mai1] == ai2) {
							QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
									.arg(rules.internalActivitiesList[ai1].id)
									.arg(rules.internalActivitiesList[ai2].id);
							s+="\n";
							s+=GeneratePreTranslate::tr("They should be consecutive activities, but there is a constraint Two Consecutive Activities that demands a reversed activity order.");
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
				// search for another Two Consecutive Activity after same activity ai1
				// ai1 -> ai2   &   ai1 -> ai3
				for (int mai1 = i+1; mai1 < constrTwoActivitiesConsecutiveActivities[ai1].size(); mai1++) {
					if (constrTwoActivitiesConsecutiveActivities[ai1][mai1] != ai2) {
						int ai3 = constrTwoActivitiesConsecutiveActivities[ai1][mai1];
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2, and %1 and %3.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id)
								.arg(rules.internalActivitiesList[ai3].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be consecutive activities, but there is another constraint Two Consecutive Activities that demands a different activity right after the first one.");
						s+=QString("\n----\n%1\n----\n%2\n----\n%3")
								.arg(rules.internalActivitiesList[ai1].getDescription())
								.arg(rules.internalActivitiesList[ai2].getDescription())
								.arg(rules.internalActivitiesList[ai3].getDescription());
						s+="\n\n";
						s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

						errors << ErrorCode(ErrorCode::Error, s, errorGroupId);

						ok = false;
					}
				}
			}
		}
		// search for another Two Consecutive Activity before same activity ai1
		// ai2 -> ai1   &   ai3 -> ai1
		for (int i = 0; i < inverseConstrTwoActivitiesConsecutivePercentages[ai1].size(); i++){
			if (inverseConstrTwoActivitiesConsecutivePercentages[ai1][i] >= 100) {
				int ai2 = inverseConstrTwoActivitiesConsecutiveActivities[ai1][i];
				for (int mai1 = i+1; mai1 < inverseConstrTwoActivitiesConsecutiveActivities[ai1].size(); mai1++) {
					if (inverseConstrTwoActivitiesConsecutiveActivities[ai1][mai1] != ai2) {
						int ai3 = inverseConstrTwoActivitiesConsecutiveActivities[ai1][mai1];
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2, and %1 and %3.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id)
								.arg(rules.internalActivitiesList[ai3].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be consecutive activities, but there is another constraint Two Consecutive Activities that demands a different activity right before the first one.");
						s+=QString("\n----\n%1\n----\n%2\n----\n%3")
								.arg(rules.internalActivitiesList[ai1].getDescription())
								.arg(rules.internalActivitiesList[ai2].getDescription())
								.arg(rules.internalActivitiesList[ai3].getDescription());
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
