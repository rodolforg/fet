/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINTBASEDIALOG_H
#define CONSTRAINTBASEDIALOG_H

#include <QDialog>
#include "timetable.h"

#include "ui_constraint_basedialog_template.h"

class ConstraintBaseDialog : public QDialog, Ui::ConstraintBaseDialog_template
{
	Q_OBJECT
protected:
	/**
	 * @brief Instantiate a dialog (without executing it) for adding a new constraint
	 * @return the instantiated dialog
	 */
	virtual QDialog *createAddDialog() = 0;
	/**
	 * @brief nstantiate a dialog (without executing it) for modifying an existing constraint
	 * @param ctr the constraint to be modified
	 * @return the instantiated dialog
	 */
	virtual QDialog *createModifyDialog(TimeConstraint *ctr) = 0;

	void setHelpText(QString msg);

	void setInstructionText(QString msg);

	/**
	 * @brief Check if a constraint passes in the filter, i.e it should be shown in constraint list
	 * @param ctr the constraint
	 * @return true if it matches the filter
	 */
	virtual bool filterOk(const TimeConstraint *ctr) const = 0;
	void filterChanged();
	void setFilterWidget(QWidget *widget);

	TimeConstraintsList visibleConstraintsList;

public:
	ConstraintBaseDialog(QWidget* parent);
	virtual ~ConstraintBaseDialog();

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	void help();

private:
	QString helpMsg;
	QWidget *filterWidget;
};

#endif // CONSTRAINTBASEDIALOG_H
