/***************************************************************************
						  constraint_basedialog.cpp  -  description
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

#include "constraint_basedialog.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QShortcut>

#include <cassert>

#include "longtextmessagebox.h"

#include "centerwidgetonscreen.h"
#include "timetable_defs.h"

ConstraintBaseDialog::ConstraintBaseDialog(QWidget* parent): QDialog(parent),
	  filterWidget(nullptr)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);

	modifyConstraintPushButton->setDefault(true);

	helpPushButton->setVisible(false);

	instructionsLabel->setVisible(false);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged(int)));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(editComments()));
	connect(activeCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggleActiveConstraint(bool)));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), constraintsListWidget);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));

	centerWidgetOnScreen(this);

	modifyConstraintPushButton->setEnabled(false);
	removeConstraintPushButton->setEnabled(false);
	commentsPushButton->setEnabled(false);
	activeCheckBox->setEnabled(false);
//	populateFilters();
//	filterChanged();
}

ConstraintBaseDialog::~ConstraintBaseDialog()
{
	delete filterWidget;
}

void ConstraintBaseDialog::setFilterWidget(QWidget *widget)
{
	if (filterWidget != nullptr) {
		rightVerticalLayout->removeWidget(filterWidget);
	}
	filterWidget = widget;
	if (widget != nullptr) {
		rightVerticalLayout->addWidget(widget);
	}
}

void ConstraintBaseDialog::filterChanged()
{
	// The clear order matters.
	// When clearing QListWidget, it emits currentRowChanged signal that
	// with index 0 and QList size 0 also if in the inverse clearing order.
	// Better solution: use model-view concept instead of this approach.
	// Or may just use QObject::blockSignals(bool)
	constraintsListWidget->clear();
	visibleConstraintsList.clear();

	fillConstraintList(visibleConstraintsList);

	if(USE_GUI_COLORS) {
		for(int i = 0; i < constraintsListWidget->count(); ++i)
		{
			QListWidgetItem* item = constraintsListWidget->item(i);
			if (isConstraintActive(visibleConstraintsList[i]))
				item->setBackground(constraintsListWidget->palette().base());
			else
				item->setBackground(constraintsListWidget->palette().alternateBase());
		}
	}

	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		this->constraintChanged(-1);
}

void ConstraintBaseDialog::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		modifyConstraintPushButton->setEnabled(false);
		removeConstraintPushButton->setEnabled(false);
		commentsPushButton->setEnabled(false);
		activeCheckBox->setEnabled(false);
		activeCheckBox->setChecked(false);
		return;
	}

	assert(index<this->visibleConstraintsList.size());
	QString s=getConstraintDetailedDescription(visibleConstraintsList.at(index));
	currentConstraintTextEdit->setPlainText(s);
	modifyConstraintPushButton->setEnabled(true);
	removeConstraintPushButton->setEnabled(true);
	commentsPushButton->setEnabled(true);
	activeCheckBox->setEnabled(true);
	activeCheckBox->setChecked(isConstraintActive(visibleConstraintsList.at(index)));
}

void ConstraintBaseDialog::addConstraint()
{
	QDialog *form = createAddDialog();
//	setParentAndOtherThings(form, this);
	form->exec();
	delete form;

	filterChanged();

	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintBaseDialog::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	void* ctr=this->visibleConstraintsList.at(i);

	QDialog *form = createModifyDialog(ctr);
//	setParentAndOtherThings(form, this);
	form->exec();
	delete form;

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	constraintsListWidget->setCurrentRow(i);
}

void ConstraintBaseDialog::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	void* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=getConstraintDetailedDescription(ctr);

	QListWidgetItem* item;

	bool confirmedRemoval = 0 == LongTextMessageBox::confirmation( this, tr("FET confirmation"),
														 s, tr("&Yes"), tr("&No"), 0, 0, 1 );
	if (!confirmedRemoval) // The user clicked the Cancel button or pressed Escape
		return;

	if (!beforeRemoveConstraint())
		return;

	doRemoveConstraint(ctr);

	item=constraintsListWidget->takeItem(i);
	visibleConstraintsList.removeAt(i);
	delete item;

	afterRemoveConstraint();

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	constraintsListWidget->setCurrentRow(i);
}

void ConstraintBaseDialog::editComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}

	void* ctr=this->visibleConstraintsList.at(i);
	editComments(ctr);

	filterChanged();
	constraintsListWidget->setCurrentRow(i);
}

void ConstraintBaseDialog::toggleActiveConstraint(bool checked)
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}

	void* ctr=this->visibleConstraintsList.at(i);
	toggleActiveConstraint(ctr, checked);

	filterChanged();
	constraintsListWidget->setCurrentRow(i);
}

void ConstraintBaseDialog::deleteItem()
{
	QListWidgetItem * item = constraintsListWidget->currentItem();

	if (item)
		removeConstraint();
}

bool ConstraintBaseDialog::beforeRemoveConstraint()
{
	return true;
}

void ConstraintBaseDialog::afterRemoveConstraint()
{
}

void ConstraintBaseDialog::setHelpText(QString msg)
{
	helpPushButton->setHidden(msg.isEmpty());
	helpMsg = msg;
}

void ConstraintBaseDialog::help()
{
	LongTextMessageBox::largeInformation(this, tr("FET help"), helpMsg);
}

QWidget *ConstraintBaseDialog::getFilterWidget() const
{
	return filterWidget;
}

void ConstraintBaseDialog::setInstructionText(QString msg)
{
	instructionsLabel->setHidden(msg.isEmpty());
	instructionsLabel->setText(msg);
}
