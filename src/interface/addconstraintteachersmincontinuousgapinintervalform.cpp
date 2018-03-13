/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "addconstraintteachersmincontinuousgapinintervalform.h"
#include "ui_addconstraintteachersmincontinuousgapinintervalform_template.h"

#include "fet.h"
#include "timeconstraint.h"

#include <QMessageBox>
#include "longtextmessagebox.h"
#include "centerwidgetonscreen.h"

AddConstraintTeachersMinContinuousGapInIntervalForm::AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, bool forAllTeachers) :
	QDialog(parent),
	ui(new Ui::AddConstraintTeachersMinContinuousGapInIntervalForm),
	ctr(NULL)
{
	Rules &rules = gt.rules;
	fillCommomUiData(rules);

	ui->startHourComboBox->setCurrentIndex(0);
	ui->endHourComboBox->setCurrentIndex(rules.nHoursPerDay);

	if (forAllTeachers) {
		ui->teacherComboBox->clear();
		ui->teacherVerticalWidget->hide();
	}
}

AddConstraintTeachersMinContinuousGapInIntervalForm::AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, ConstraintTeachersMinContinuousGapInInterval *c) :
	QDialog(parent),
	ui(new Ui::AddConstraintTeachersMinContinuousGapInIntervalForm),
	ctr(c)
{
	assert(c != NULL);

	Rules &rules = gt.rules;
	fillCommomUiData(rules);

	//: This is the title of the dialog to add a new constraint of this type
	setWindowTitle("Modify teachers min continuous gap in interval");
	ui->okPushButton->setText(tr("Ok"));
	ui->closePushButton->setText(tr("Cancel"));

	ui->startHourComboBox->setCurrentIndex(c->startHour);
	ui->endHourComboBox->setCurrentIndex(c->endHour);
	ui->minContinuousGapSpinBox->setValue(c->minGapDuration);
	ui->teacherComboBox->clear();
	ui->teacherVerticalWidget->hide();
}

AddConstraintTeachersMinContinuousGapInIntervalForm::AddConstraintTeachersMinContinuousGapInIntervalForm(QWidget *parent, ConstraintTeacherMinContinuousGapInInterval *c) :
	QDialog(parent),
	ui(new Ui::AddConstraintTeachersMinContinuousGapInIntervalForm),
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
	ui->teacherComboBox->setCurrentText(c->teacherName);
}

AddConstraintTeachersMinContinuousGapInIntervalForm::~AddConstraintTeachersMinContinuousGapInIntervalForm()
{
	saveFETDialogGeometry(this);
	delete ui;
}

void AddConstraintTeachersMinContinuousGapInIntervalForm::fillCommomUiData(Rules &rules)
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

	foreach (Teacher *tch, rules.teachersList)
		ui->teacherComboBox->addItem(tch->name);
}

void AddConstraintTeachersMinContinuousGapInIntervalForm::handleConstraint()
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

	QString teacher_name = ui->teacherComboBox->currentText();
	if (!teacher_name.isEmpty()) {
		int teacher_ID = gt.rules.searchTeacher(teacher_name);
		if(teacher_ID<0){
			QMessageBox::warning(this, tr("FET information"),
				tr("Invalid teacher"));
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
		if (teacher_name.isEmpty())
			c = new ConstraintTeachersMinContinuousGapInInterval(weight, min_gap, startHour, endHour);
		else
			c = new ConstraintTeacherMinContinuousGapInInterval(weight, min_gap, teacher_name, startHour, endHour);

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
		if (ctr->type == CONSTRAINT_TEACHER_MIN_CONTINUOUS_GAP_IN_INTERVAL) {
			ConstraintTeacherMinContinuousGapInInterval *c = (ConstraintTeacherMinContinuousGapInInterval*)ctr;
			c->weightPercentage = weight;
			c->minGapDuration = min_gap;
			c->startHour = startHour;
			c->endHour = endHour;
			c->teacherName = teacher_name;
		} else {
			assert(ctr->type == CONSTRAINT_TEACHERS_MIN_CONTINUOUS_GAP_IN_INTERVAL);
			ConstraintTeachersMinContinuousGapInInterval *c = (ConstraintTeachersMinContinuousGapInInterval*)ctr;
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

void AddConstraintTeachersMinContinuousGapInIntervalForm::hourIntervalChanged()
{
	int start = ui->startHourComboBox->currentIndex();
	int end = ui->endHourComboBox->currentIndex();
	int diff = end - start;
	if (diff < 0)
		diff = 0;
	ui->minContinuousGapSpinBox->setMaximum(diff);
}
