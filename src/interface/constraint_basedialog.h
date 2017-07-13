/***************************************************************************
						  constraint_basedialog.h  -  description
							 -------------------
	begin                : 2017
	copyright            : (C) 2017 by Rodolfo RG
	This file is part of a modification of FET timetable (the original is developed by Liviu Lalescu)
 ***************************************************************************/

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

#include "ui_constraint_basedialog_template.h"

class ConstraintBaseDialog : public QDialog, protected Ui::ConstraintBaseDialog_template
{
	Q_OBJECT
protected:
	/**
	 * @brief Instantiate a dialog (without executing it) for adding a new constraint
	 * @return the instantiated dialog
	 */
	virtual QDialog *createAddDialog() = 0;
	/**
	 * @brief Instantiate a dialog (without executing it) for modifying an existing constraint
	 * @param ctr the constraint to be modified
	 * @return the instantiated dialog
	 */
	virtual QDialog *createModifyDialog(void *ctr) = 0;

	void setHelpText(QString msg);

	void setInstructionText(QString msg);

	void filterChanged();
	void setFilterWidget(QWidget *widget);
	QWidget *getFilterWidget() const;

	/**
	 * @brief A callback function called before a constraint is removed from this constraint list
	 *
	 * It is called after user has initially confirmed constraint removal. Useful for extra confirmation.
	 * @return true if it should resume constraint removal
	 */
	virtual bool beforeRemoveConstraint();
	/**
	 * @brief doRemoveConstraint Remove constraint from timetable rules
	 * @param ctr generic pointer to a constraint (TimeConstraint or SpaceConstraint) in constraint list
	 */
	virtual void doRemoveConstraint(void * ctr) = 0;

	/**
	 * @brief A callback function called after a constraint is successfully removed from this dialog list
	 */
	virtual void afterRemoveConstraint();

	QList<void*> visibleConstraintsList;

	/**
	 * @brief fillConstraintList
	 * @param list List of filtered constraints (those that aproved by filterOk() )
	 */
	virtual void fillConstraintList(QList<void*>& list) = 0;

	virtual QString getConstraintDetailedDescription(const void *ctr) const = 0;

	/// Allows user to edit constraint comments
	virtual void editComments(void *ctr) = 0;

	/// Returns true if constraint is active
	virtual bool isConstraintActive(const void *ctr) const = 0;
	/// Returns true if constraint is active
	virtual void toggleActiveConstraint(void *ctr, bool checked) const = 0;

public:
	ConstraintBaseDialog(QWidget* parent);
	virtual ~ConstraintBaseDialog();

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	void editComments();
	void toggleActiveConstraint(bool checked);
	void deleteItem();
	void help();

private:
	QString helpMsg;
	QWidget *filterWidget;
};

#endif // CONSTRAINTBASEDIALOG_H
