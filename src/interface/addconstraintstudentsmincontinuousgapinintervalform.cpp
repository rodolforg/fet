/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "addconstraintstudentsmincontinuousgapinintervalform.h"
#include "ui_addconstraintstudentsmincontinuousgapinintervalform_template.h"

#include "fet.h"
#include "timeconstraint.h"

#include <QMessageBox>
#include "longtextmessagebox.h"

#include "fetguisettings.h"

AddConstraintStudentsMinContinuousGapInIntervalForm::AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, bool forAllStudents) :
	QDialog(parent),
	ui(new Ui::AddConstraintStudentsMinContinuousGapInIntervalForm),
	ctr(NULL)
{
	Rules &rules = gt.rules;
	fillCommomUiData(rules);

	ui->startHourComboBox->setCurrentIndex(0);
	ui->endHourComboBox->setCurrentIndex(rules.nHoursPerDay);

	if (forAllStudents) {
		ui->studentsComboBox->clear();
		ui->studentsVerticalWidget->hide();
	}
}

AddConstraintStudentsMinContinuousGapInIntervalForm::AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, ConstraintStudentsMinContinuousGapInInterval *c) :
	QDialog(parent),
	ui(new Ui::AddConstraintStudentsMinContinuousGapInIntervalForm),
	ctr(c)
{
	assert(c != NULL);

	Rules &rules = gt.rules;
	fillCommomUiData(rules);

	//: This is the title of the dialog to add a new constraint of this type
	setWindowTitle("Modify students min continuous gap in interval");
	ui->okPushButton->setText(tr("Ok"));
	ui->closePushButton->setText(tr("Cancel"));

	ui->startHourComboBox->setCurrentIndex(c->startHour);
	ui->endHourComboBox->setCurrentIndex(c->endHour);
	ui->minContinuousGapSpinBox->setValue(c->minGapDuration);
	ui->studentsComboBox->clear();
	ui->studentsVerticalWidget->hide();
}

AddConstraintStudentsMinContinuousGapInIntervalForm::AddConstraintStudentsMinContinuousGapInIntervalForm(QWidget *parent, ConstraintStudentsSetMinContinuousGapInInterval *c) :
	QDialog(parent),
	ui(new Ui::AddConstraintStudentsMinContinuousGapInIntervalForm),
	ctr(c)
{
	assert(c != NULL);

	Rules &rules = gt.rules;
	fillCommomUiData(rules);

	//: This is the title of the dialog to add a new constraint of this type
	setWindowTitle("Modify teacher min continuous gap in interval");
	ui->okPushButton->setText(tr("Ok"));
	ui->closePushButton->setText(tr("Cancel"));

	ui->startHourComboBox->setCurrentIndex(c->startHour);
	ui->endHourComboBox->setCurrentIndex(c->endHour);
	ui->minContinuousGapSpinBox->setValue(c->minGapDuration);
	ui->studentsComboBox->setCurrentText(c->students);
}

AddConstraintStudentsMinContinuousGapInIntervalForm::~AddConstraintStudentsMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
	delete ui;
}

void AddConstraintStudentsMinContinuousGapInIntervalForm::fillCommomUiData(Rules &rules)
{
	ui->setupUi(this);

	ui->okPushButton->setDefault(true);

	connect(ui->okPushButton, SIGNAL(clicked()), this, SLOT(handleConstraint()));
	connect(ui->closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	for (int i = 0; i < rules.nHoursPerDay; i++)
		ui->startHourComboBox->addItem(rules.hoursOfTheDay[i]);

	for (int i = 0; i < rules.nHoursPerDay; i++)
		ui->endHourComboBox->addItem(rules.hoursOfTheDay[i]);
	ui->endHourComboBox->addItem(tr("End of day"));

	ui->minContinuousGapSpinBox->setMinimum(0);
	ui->minContinuousGapSpinBox->setMaximum(rules.nHoursPerDay);

	connect(ui->startHourComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(hourIntervalChanged()));
	connect(ui->endHourComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(hourIntervalChanged()));

	ui->studentsComboBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		ui->studentsComboBox->addItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			ui->studentsComboBox->addItem(stg->name);
			if(SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				ui->studentsComboBox->addItem(sts->name);
			}
		}
	}
}

void AddConstraintStudentsMinContinuousGapInIntervalForm::handleConstraint()
{
	Rules &rules = gt.rules;

	double weight;
	weight_sscanf(ui->weightLineEdit->text(), "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	int min_gap = ui->minContinuousGapSpinBox->value();

	QString students = ui->studentsComboBox->currentText();
	if (!students.isEmpty()) {
		StudentsSet * ss = gt.rules.searchStudentsSet(students);
		if(ss == NULL){
			QMessageBox::warning(this, tr("FET information"),
				tr("Invalid students set"));
			return;
		}
	}

	int startHour = ui->startHourComboBox->currentIndex();
	int endHour = ui->endHourComboBox->currentIndex();
	if(startHour<0 || startHour>=rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour invalid"));
		return;
	}
	if(endHour<0 || endHour>rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour invalid"));
		return;
	}
	if(endHour<=startHour){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour cannot be greater or equal than end hour"));
		return;
	}

	if (ctr == NULL) {
		TimeConstraint *c;
		if (students.isEmpty())
			c = new ConstraintStudentsMinContinuousGapInInterval(weight, min_gap, startHour, endHour);
		else
			c = new ConstraintStudentsSetMinContinuousGapInInterval(weight, min_gap, students, startHour, endHour);

		bool ok = rules.addTimeConstraint(c);
		if(ok)
			LongTextMessageBox::information(this, tr("FET information"),
				tr("Constraint added:")+"\n\n"+c->getDetailedDescription(gt.rules));
		else{
			QMessageBox::warning(this, tr("FET information"),
				tr("Constraint NOT added - please report error"));
			delete c;
		}
	} else {
		if (ctr->type == CONSTRAINT_STUDENTSSET_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			ConstraintStudentsSetMinContinuousGapInInterval *c = (ConstraintStudentsSetMinContinuousGapInInterval*)ctr;
			c->weightPercentage = weight;
			c->minGapDuration = min_gap;
			c->startHour = startHour;
			c->endHour = endHour;
			c->students = students;
		} else {
			assert(ctr->type == CONSTRAINT_STUDENTS_MIN_CONTINUOUS_GAP_IN_INTERVAL);
			ConstraintStudentsMinContinuousGapInInterval *c = (ConstraintStudentsMinContinuousGapInInterval*)ctr;
			c->weightPercentage = weight;
			c->minGapDuration = min_gap;
			c->startHour = startHour;
			c->endHour = endHour;
		}

		gt.rules.internalStructureComputed=false;
		gt.rules.setModified(true);

		close();
	}
}

void AddConstraintStudentsMinContinuousGapInIntervalForm::hourIntervalChanged()
{
	int start = ui->startHourComboBox->currentIndex();
	int end = ui->endHourComboBox->currentIndex();
	int diff = end - start;
	if (diff < 0)
		diff = 0;
	ui->minContinuousGapSpinBox->setMaximum(diff);
}
