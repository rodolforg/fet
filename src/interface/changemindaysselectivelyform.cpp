/***************************************************************************
                          changemindaysselectivelyform.cpp  -  description
                             -------------------
    begin                : July 30, 2008
    copyright            : (C) 2008 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "changemindaysselectivelyform.h"

#include "fet.h"

#include <QMessageBox>

ChangeMinDaysSelectivelyForm::ChangeMinDaysSelectivelyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=oldConsecutiveComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=newConsecutiveComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	oldConsecutiveComboBox->clear();
	oldConsecutiveComboBox->addItem(tr("Any"));
	oldConsecutiveComboBox->addItem(tr("Yes"));
	oldConsecutiveComboBox->addItem(tr("No"));
	oldConsecutiveComboBox->setCurrentIndex(0);
	
	oldDaysSpinBox->setMinimum(-1);
	oldDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek);
	oldDaysSpinBox->setValue(-1);

	newConsecutiveComboBox->clear();
	newConsecutiveComboBox->addItem(tr("No change"));
	newConsecutiveComboBox->addItem(tr("Yes"));
	newConsecutiveComboBox->addItem(tr("No"));
	newConsecutiveComboBox->setCurrentIndex(0);
	
	newDaysSpinBox->setMinimum(-1);
	newDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek);
	newDaysSpinBox->setValue(-1);
	
	oldNActsSpinBox->setMinimum(-1);
	oldNActsSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
	oldNActsSpinBox->setValue(-1);
}

ChangeMinDaysSelectivelyForm::~ChangeMinDaysSelectivelyForm()
{
	saveFETDialogGeometry(this);

}

void ChangeMinDaysSelectivelyForm::ok()
{
	enum {ANY=0, YES=1, NO=2};
	enum {NOCHANGE=0};

	oldWeight=oldDays=oldConsecutive=oldNActs=-2;
	newWeight=newDays=newConsecutive=-2;

	QString oldWeightS=oldWeightLineEdit->text();
	weight_sscanf(oldWeightS, "%lf", &oldWeight);
	if(!(oldWeight==-1 || (oldWeight>=0.0 && oldWeight<=100.0))){
		QMessageBox::warning(this, tr("FET warning"), tr("Old weight must be -1 or both >=0 and <=100"));
		return;
	}
	
	QString newWeightS=newWeightLineEdit->text();
	weight_sscanf(newWeightS, "%lf", &newWeight);
	if(!(newWeight==-1 || (newWeight>=0.0 && newWeight<=100.0))){
		QMessageBox::warning(this, tr("FET warning"), tr("New weight must be -1 or both >=0 and <=100"));
		return;
	}
	
	oldDays=oldDaysSpinBox->value();
	if(!(oldDays==-1 || (oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek))){
		QMessageBox::warning(this, tr("FET warning"), tr("Old min days must be -1 or both >=1 and <=n_days_per_week"));
		return;
	}
	
	newDays=newDaysSpinBox->value();
	if(!(newDays==-1 || (newDays>=1 && newDays<=gt.rules.nDaysPerWeek))){
		QMessageBox::warning(this, tr("FET warning"), tr("New min days must be -1 or both >=1 and <=n_days_per_week"));
		return;
	}
	
	oldConsecutive=oldConsecutiveComboBox->currentIndex();
	assert(oldConsecutive>=0 && oldConsecutive<=2);

	newConsecutive=newConsecutiveComboBox->currentIndex();
	assert(newConsecutive>=0 && newConsecutive<=2);

	oldNActs=oldNActsSpinBox->value();
	if(!(oldNActs==-1 || oldNActs>=1)){
		QMessageBox::warning(this, tr("FET warning"), tr("Old n_acts must be -1 or >=1"));
		return;
	}
	
	this->accept();
}

void ChangeMinDaysSelectivelyForm::cancel()
{
	this->reject();
}
