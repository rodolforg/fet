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

QStringList MinContinuousGapInIntervalForStudents::getErrors() const
{
	return errors;
}

MinContinuousGapInIntervalForStudents::MinContinuousGapInIntervalForStudents()
{

}

bool MinContinuousGapInIntervalForStudents::prepare(const Rules &rules)
{
	errors.clear();

	bool ok=true;

	for (int i = 0; i < MAX; i++) {
		for(int j=0; j<rules.nInternalSubgroups; j++) {
			data[j][i].minGapDuration = -1;
			data[j][i].startHour = -1;
			data[j][i].endHour = -1;
			data[j][i].weightPercentage = -1;
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

		foreach(int sbg, sbgs){
			bool fits = false;
			for (int j = 0; j < MAX; j++) {
				if (data[sbg][j].weightPercentage < 0) {
					data[sbg][j] = ctrData;
					fits = true;
					break;
				} else if (data[sbg][j].startHour == ctrData.startHour &&
						   data[sbg][j].endHour == ctrData.endHour &&
						   data[sbg][j].minGapDuration == ctrData.minGapDuration)
				{
					// Repeated costraint contents - just stores the greater weight
					if (data[sbg][j].weightPercentage < ctrData.weightPercentage)
						data[sbg][j].weightPercentage = ctrData.weightPercentage;
					fits = true;
					break;
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[sbg][j].weightPercentage == ctrData.weightPercentage &&
						   data[sbg][j].minGapDuration == ctrData.minGapDuration)
				{
					// Does a 100% constraint include a similar one?
					if (data[sbg][j].startHour <= ctrData.startHour &&
							data[sbg][j].endHour >= ctrData.endHour)
					{
						data[sbg][j].startHour = ctrData.startHour;
						data[sbg][j].endHour = ctrData.endHour;
						fits = true;
						break;
					}
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[sbg][j].startHour == ctrData.startHour &&
						   data[sbg][j].endHour == ctrData.endHour)
				{
					// Same interval but different mandatory (w:100%) min gaps?
					if (data[sbg][j].minGapDuration < ctrData.minGapDuration) {
						data[sbg][j].minGapDuration = ctrData.minGapDuration;
					}
					fits = true;
					break;
				}
			}
			if (!fits) {
				ok = false;
				errors.append(
							GeneratePreTranslate::tr("Cannot optimize for student set %1, because it has more than %2 constraints min continuous gap in interval"
							". Please modify your data correspondingly (leave maximum %2 constraints of type"
							" constraint student set(s) min continuous gap in interval for each student set) and try again")
							.arg(rules.internalSubgroupsList[sbg]->name)
							.arg(MAX)
							);
			}
		}
	}

	return ok;
}
