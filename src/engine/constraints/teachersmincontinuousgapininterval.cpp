/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "teachersmincontinuousgapininterval.h"

#include "generate_pre.h"

QStringList MinContinuousGapInIntervalForTeachers::getErrors() const
{
	return errors;
}

MinContinuousGapInIntervalForTeachers::MinContinuousGapInIntervalForTeachers()
{

}

bool MinContinuousGapInIntervalForTeachers::prepare(const Rules &rules)
{
	errors.clear();

	bool ok=true;

	for (int i = 0; i < MAX; i++) {
		for(int j=0; j<rules.nInternalTeachers; j++) {
			data[j][i].minGapDuration = -1;
			data[j][i].startHour = -1;
			data[j][i].endHour = -1;
			data[j][i].weightPercentage = -1;
		}
	}

	for (int ic = 0; ic < rules.nInternalTimeConstraints; ic++) {
		const TimeConstraint *ctr = rules.internalTimeConstraintsList[ic];
		Data ctrData;
		int to=-1, tf=-1;
		if(ctr->type==CONSTRAINT_TEACHERS_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			const ConstraintTeachersMinContinuousGapInInterval* tmcg = (const ConstraintTeachersMinContinuousGapInInterval*)ctr;
			ctrData.endHour = tmcg->endHour;
			ctrData.minGapDuration = tmcg->minGapDuration;
			ctrData.startHour = tmcg->startHour;
			ctrData.weightPercentage = tmcg->weightPercentage;
			to = 0;
			tf = rules.nInternalTeachers;
		} else if (ctr->type==CONSTRAINT_TEACHER_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			const ConstraintTeacherMinContinuousGapInInterval* tmcg = (const ConstraintTeacherMinContinuousGapInInterval*)ctr;
			ctrData.endHour = tmcg->endHour;
			ctrData.minGapDuration = tmcg->minGapDuration;
			ctrData.startHour = tmcg->startHour;
			ctrData.weightPercentage = tmcg->weightPercentage;
			to = tmcg->teacher_ID;
			tf = to+1;
		} else {
			continue;
		}

		for(int t=to; t<tf; t++){
			bool fits = false;
			for (int j = 0; j < MAX; j++) {
				if (data[t][j].weightPercentage < 0) {
					data[t][j] = ctrData;
					fits = true;
					break;
				} else if (data[t][j].startHour == ctrData.startHour &&
						   data[t][j].endHour == ctrData.endHour &&
						   data[t][j].minGapDuration == ctrData.minGapDuration)
				{
					// Repeated costraint contents - just stores the greater weight
					if (data[t][j].weightPercentage < ctrData.weightPercentage)
						data[t][j].weightPercentage = ctrData.weightPercentage;
					fits = true;
					break;
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[t][j].weightPercentage == ctrData.weightPercentage &&
						   data[t][j].minGapDuration == ctrData.minGapDuration)
				{
					// Does a 100% constraint include a similar one?
					if (data[t][j].startHour <= ctrData.startHour &&
							data[t][j].endHour >= ctrData.endHour)
					{
						data[t][j].startHour = ctrData.startHour;
						data[t][j].endHour = ctrData.endHour;
						fits = true;
						break;
					}
				} else if (ctrData.weightPercentage == 100.0 &&
						   data[t][j].startHour == ctrData.startHour &&
						   data[t][j].endHour == ctrData.endHour)
				{
					// Same interval but different mandatory (w:100%) min gaps?
					if (data[t][j].minGapDuration < ctrData.minGapDuration) {
						data[t][j].minGapDuration = ctrData.minGapDuration;
					}
					fits = true;
					break;
				}
			}
			if (!fits) {
				ok = false;
				errors.append(
							GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has more than %2 constraints min continuous gap in interval"
							". Please modify your data correspondingly (leave maximum %2 constraints of type"
							" constraint teacher(s) min continuous gap in interval for each teacher) and try again")
							.arg(rules.internalTeachersList[t]->name)
							.arg(MAX)
							);
			}
		}
	}

	return ok;
}
