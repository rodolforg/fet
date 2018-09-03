/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "paradoxorderedifsamedayvsreversed.h"
#include "generate_pre.h"

ParadoxOrderedIfSameDayVsReversed::ParadoxOrderedIfSameDayVsReversed()
{

}

// check for reversed Ordered and Ordered If On Same Day
bool ParadoxOrderedIfSameDayVsReversed::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	QSet<QPair<int,int> > groups;

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < constrTwoActivitiesOrderedIfSameDayPercentages[ai1].size(); i++){
			if (constrTwoActivitiesOrderedIfSameDayPercentages[ai1][i] >= 100) {
				int ai2 = constrTwoActivitiesOrderedIfSameDayActivities[ai1][i];
				// search for contradictory constraints Two Ordered Activities in reverse order
				for (int mai2 = 0; mai2 < constrTwoActivitiesOrderedActivities[ai2].size(); mai2++) {
					if (constrTwoActivitiesOrderedActivities[ai2][mai2] == ai1) {
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be ordered activities, but there is a constraint Two Ordered Activities If On Same Day that demands a reversed activity order.");
						s+=QString("\n----\n%1\n----\n%2")
								.arg(rules.internalActivitiesList[ai1].getDescription())
								.arg(rules.internalActivitiesList[ai2].getDescription());
						s+="\n\n";
						s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

						errors << ErrorCode(ErrorCode::Error, s, errorGroupId);

						ok = false;
					}
				}
				// check whether this group was already visited, but in reversed order
				const QPair<int,int> g(ai1, ai2);
				const QPair<int,int> invg(ai2, ai1);
				if (groups.contains(invg))
					continue;
				groups << g;
				// search for contradictory constraints Two Ordered If On Same Day Activities in reverse order
				for (int mai2 = 0; mai2 < constrTwoActivitiesOrderedIfSameDayActivities[ai2].size(); mai2++) {
					if (constrTwoActivitiesOrderedIfSameDayActivities[ai2][mai2] == ai1) {
						QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
								.arg(rules.internalActivitiesList[ai1].id)
								.arg(rules.internalActivitiesList[ai2].id);
						s+="\n";
						s+=GeneratePreTranslate::tr("They should be ordered activities, but there is another constraint Two Ordered Activities If On Same Day that demands a reversed activity order.");
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
