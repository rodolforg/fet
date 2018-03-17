/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "paradoxmindaysvsthreegrouped.h"
#include "generate_pre.h"

ParadoxMinDaysVsThreeGrouped::ParadoxMinDaysVsThreeGrouped()
{

}

bool ParadoxMinDaysVsThreeGrouped::prepare(const Rules& rules)
{
	bool ok = true;
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	QSet<QSet<int> > groups;

	for(int ai1=0; ai1<rules.nInternalActivities; ai1++){
		for (int i = 0; i < constrThreeActivitiesGroupedPercentages[ai1].size(); i++){
			if (constrThreeActivitiesGroupedPercentages[ai1][i] >= 100) {
				int ai2 = constrThreeActivitiesGroupedActivities[ai1][i].first;
				int ai3 = constrThreeActivitiesGroupedActivities[ai1][i].second;
				// check whether this group was already visited, but in reversed order
				QSet<int> g;
				g << ai1 << ai2 << ai3;
				if (groups.contains(g))
					continue;
				groups << g;
				QList<QPair<int, int> > pairs;
				pairs << QPair<int, int>(ai1, ai2) << QPair<int, int>(ai1, ai3) << QPair<int, int>(ai2, ai3);
				QPair<int, int> pair;
				foreach (pair, pairs) {
					// search for contradictory constraints Min days between paired activities
					int pai1 = pair.first;
					int pai2 = pair.second;
					for (int mai1 = 0; mai1 < minDaysBetweenActivitiesList.activities[pai1].size(); mai1++) {
						if (minDaysBetweenActivitiesList.activities[pai1][mai1] == pai2 && minDaysBetweenActivitiesList.weightPercentages[pai1][mai1] >= 100) {
							if (minDaysBetweenActivitiesList.minDays[pai1][mai1] > 0) {
								QString s=GeneratePreTranslate::tr("There are contradictory constraints with activities %1 and %2.")
								 .arg(rules.internalActivitiesList[pai1].id)
								 .arg(rules.internalActivitiesList[pai2].id);
								s+="\n";
								s+=GeneratePreTranslate::tr("There should be %1 min days between them but they must also be grouped activities.")
								 .arg(minDaysBetweenActivitiesList.minDays[pai1][mai1]);
								s+=QString("\n----\n%1\n----\n%2")
								 .arg(rules.internalActivitiesList[pai1].getDescription())
								 .arg(rules.internalActivitiesList[pai2].getDescription());
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
	}
	return ok;
}
