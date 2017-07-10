/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H
#define ADDCONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H

#include <QDialog>

class Rules;
class TimeConstraint;
class ConstraintStudentsSetMinContinuousGapInInterval;
class ConstraintStudentsMinContinuousGapInInterval;

namespace Ui {
class AddConstraintStudentsMinContinuousGapInIntervalForm;
}

class AddConstraintStudentsMinContinuousGapInIntervalForm : public QDialog
{
	Q_OBJECT

public:
	AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, bool forAllStudents);
	AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, ConstraintStudentsMinContinuousGapInInterval *ctr);
	AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, ConstraintStudentsSetMinContinuousGapInInterval *ctr);
	~AddConstraintStudentsMinContinuousGapInIntervalForm();

private:
	Ui::AddConstraintStudentsMinContinuousGapInIntervalForm *ui;

	void fillCommomUiData(Rules &rules);

	TimeConstraint *ctr;

public slots:
	void handleConstraint();
//	void modifyConstraint();
	void hourIntervalChanged();
};

#endif // ADDCONSTRAINTSTUDENTSMINCONTINUOUSGAPININTERVALFORM_H
