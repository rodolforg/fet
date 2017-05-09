/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H
#define ADDCONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H

#include <QDialog>

class Rules;
class TimeConstraint;
class ConstraintTeacherMinContinuousGapInInterval;
class ConstraintTeachersMinContinuousGapInInterval;

namespace Ui {
class AddConstraintTeachersMinContinuousGapInIntervalForm;
}

class AddConstraintTeachersMinContinuousGapInIntervalForm : public QDialog
{
	Q_OBJECT

public:
	AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, bool forAllTeachers);
	AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, ConstraintTeachersMinContinuousGapInInterval *ctr);
	AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, ConstraintTeacherMinContinuousGapInInterval *ctr);
	~AddConstraintTeachersMinContinuousGapInIntervalForm();

private:
	Ui::AddConstraintTeachersMinContinuousGapInIntervalForm *ui;

	void fillCommomUiData(Rules &rules);

	TimeConstraint *ctr;

public slots:
	void handleConstraint();
//	void modifyConstraint();
	void hourIntervalChanged();
};

#endif // ADDCONSTRAINTTEACHERSMINCONTINUOUSGAPININTERVALFORM_H
