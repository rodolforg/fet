/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "studentsmincontinuousgapininterval.h"
#include "generate_pre.h"

MinContinuousGapInIntervalForStudents::MinContinuousGapInIntervalForStudents()
{
	data = new Data[MAX_TOTAL_SUBGROUPS][MAX];
}

MinContinuousGapInIntervalForStudents::~MinContinuousGapInIntervalForStudents()
{
	delete [] data;
}

bool MinContinuousGapInIntervalForStudents::prepare(const Rules &rules)
{
	int errorGroupId = ErrorCode::nextGroupId();
	errors.clear();

	bool ok=true;

	for (int i = 0; i < MAX; i++) {
		for(int j=0; j<rules.nInternalSubgroups; j++) {
			data[i][j].minGapDuration = -1;
			data[i][j].startHour = -1;
			data[i][j].endHour = -1;
			data[i][j].weightPercentage = -1;
		}
	}

	for (int ic = 0; ic < rules.nInternalTimeConstraints; ic++) {
		const TimeConstraint *ctr = rules.internalTimeConstraintsList[ic];
		Data ctrData;
		QList<int> sbgs;
		if(ctr->type==CONSTRAINT_STUDENTS_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			const ConstraintStudentsMinContinuousGapInInterval* stmcg = (const ConstraintStudentsMinContinuousGapInInterval*)ctr;
			ctrData.endHour = stmcg->endHour;
			ctrData.minGapDuration = stmcg->minGapDuration;
			ctrData.startHour = stmcg->startHour;
			ctrData.weightPercentage = ctr->weightPercentage;
			for (int i=0; i < rules.nInternalSubgroups; i++)
				sbgs << i;
		} else if (ctr->type==CONSTRAINT_STUDENTSSET_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			const ConstraintStudentsSetMinContinuousGapInInterval* stmcg = (const ConstraintStudentsSetMinContinuousGapInInterval*)ctr;
			ctrData.endHour = stmcg->endHour;
			ctrData.minGapDuration = stmcg->minGapDuration;
			ctrData.startHour = stmcg->startHour;
			ctrData.weightPercentage = ctr->weightPercentage;
			sbgs = stmcg->iSubgroupsList;
		} else {
			continue;
		}

		for(int sbg : qAsConst(sbgs)){
			bool fits = false;
			for (int j = 0; j < MAX; j++) {
				if (data[j][sbg].weightPercentage < 0) {
					data[j][sbg] = ctrData;
					fits = true;
					break;
				} else if (data[j][sbg].startHour == ctrData.startHour &&
						   data[j][sbg].endHour == ctrData.endHour &&
						   data[j][sbg].minGapDuration == ctrData.minGapDuration)
				{
					// Repeated costraint contents - just stores the greater weight
					if (data[j][sbg].weightPercentage < ctrData.weightPercentage)
						data[j][sbg].weightPercentage = ctrData.weightPercentage;
					fits = true;
					break;
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[j][sbg].weightPercentage == ctrData.weightPercentage &&
						   data[j][sbg].minGapDuration == ctrData.minGapDuration)
				{
					// Does a 100% constraint include a similar one?
					if (data[j][sbg].startHour <= ctrData.startHour &&
							data[j][sbg].endHour >= ctrData.endHour)
					{
						data[j][sbg].startHour = ctrData.startHour;
						data[j][sbg].endHour = ctrData.endHour;
						fits = true;
						break;
					}
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[j][sbg].startHour == ctrData.startHour &&
						   data[j][sbg].endHour == ctrData.endHour)
				{
					// Same interval but different mandatory (w:100%) min gaps?
					if (data[j][sbg].minGapDuration < ctrData.minGapDuration) {
						data[j][sbg].minGapDuration = ctrData.minGapDuration;
					}
					fits = true;
					break;
				}
			}
			if (!fits) {
				ok = false;
				errors.append(ErrorCode(ErrorCode::Error,
							GeneratePreTranslate::tr("Cannot optimize for student set %1, because it has more than %2 constraints min continuous gap in interval"
							". Please modify your data correspondingly (leave maximum %2 constraints of type"
							" constraint student set(s) min continuous gap in interval for each student set) and try again")
							.arg(rules.internalSubgroupsList[sbg]->name)
							.arg(MAX), errorGroupId)
							);
			}
		}
	}

	return ok;
}
