/***************************************************************************
                          addactivityform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include <QtGlobal>

#include "longtextmessagebox.h"

#include "addactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include "activityplanningform.h"

#include <QMessageBox>

#include <QDialog>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QScrollBar>

QSpinBox* AddActivityForm::dur(int i)
{
	assert(i>=0 && i<durList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return durList.at(i);
}

QCheckBox* AddActivityForm::activ(int i)
{
	assert(i>=0 && i<activList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return activList.at(i);
}

AddActivityForm::AddActivityForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName): QDialog(parent)
{
	setupUi(this);
	
	allTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	splitSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);

	durList.clear();
	durList.append(duration1SpinBox);
	durList.append(duration2SpinBox);
	durList.append(duration3SpinBox);
	durList.append(duration4SpinBox);
	durList.append(duration5SpinBox);
	durList.append(duration6SpinBox);
	durList.append(duration7SpinBox);
	durList.append(duration8SpinBox);
	durList.append(duration9SpinBox);
	durList.append(duration10SpinBox);
	durList.append(duration11SpinBox);
	durList.append(duration12SpinBox);
	durList.append(duration13SpinBox);
	durList.append(duration14SpinBox);
	durList.append(duration15SpinBox);
	durList.append(duration16SpinBox);
	durList.append(duration17SpinBox);
	durList.append(duration18SpinBox);
	durList.append(duration19SpinBox);
	durList.append(duration20SpinBox);
	durList.append(duration21SpinBox);
	durList.append(duration22SpinBox);
	durList.append(duration23SpinBox);
	durList.append(duration24SpinBox);
	durList.append(duration25SpinBox);
	durList.append(duration26SpinBox);
	durList.append(duration27SpinBox);
	durList.append(duration28SpinBox);
	durList.append(duration29SpinBox);
	durList.append(duration30SpinBox);
	durList.append(duration31SpinBox);
	durList.append(duration32SpinBox);
	durList.append(duration33SpinBox);
	durList.append(duration34SpinBox);
	durList.append(duration35SpinBox);
	
	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++)
		dur(i)->setMaximum(gt.rules.nHoursPerDay);

	activList.clear();
	activList.append(active1CheckBox);
	activList.append(active2CheckBox);
	activList.append(active3CheckBox);
	activList.append(active4CheckBox);
	activList.append(active5CheckBox);
	activList.append(active6CheckBox);
	activList.append(active7CheckBox);
	activList.append(active8CheckBox);
	activList.append(active9CheckBox);
	activList.append(active10CheckBox);
	activList.append(active11CheckBox);
	activList.append(active12CheckBox);
	activList.append(active13CheckBox);
	activList.append(active14CheckBox);
	activList.append(active15CheckBox);
	activList.append(active16CheckBox);
	activList.append(active17CheckBox);
	activList.append(active18CheckBox);
	activList.append(active19CheckBox);
	activList.append(active20CheckBox);
	activList.append(active21CheckBox);
	activList.append(active22CheckBox);
	activList.append(active23CheckBox);
	activList.append(active24CheckBox);
	activList.append(active25CheckBox);
	activList.append(active26CheckBox);
	activList.append(active27CheckBox);
	activList.append(active28CheckBox);
	activList.append(active29CheckBox);
	activList.append(active30CheckBox);
	activList.append(active31CheckBox);
	activList.append(active32CheckBox);
	activList.append(active33CheckBox);
	activList.append(active34CheckBox);
	activList.append(active35CheckBox);

	connect(subgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsChanged()));
	connect(groupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showGroupsChanged()));
	connect(yearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showYearsChanged()));

	connect(splitSpinBox, SIGNAL(valueChanged(int)), this, SLOT(splitChanged()));

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(addActivityPushButton, SIGNAL(clicked()), this, SLOT(addActivity()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	connect(allTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addTeacher()));
	connect(selectedTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeTeacher()));
	connect(allStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addStudents()));
	connect(selectedStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeStudents()));
	connect(allActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addActivityTag()));
	connect(selectedActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeActivityTag()));

	connect(clearActivityTagPushButton, SIGNAL(clicked()), this, SLOT(clearActivityTags()));
	connect(clearStudentsPushButton, SIGNAL(clicked()), this, SLOT(clearStudents()));
	connect(clearTeacherPushButton, SIGNAL(clicked()), this, SLOT(clearTeachers()));

	connect(minDayDistanceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(minDaysChanged()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	selectedStudentsListWidget->clear();
	updateStudentsListWidget();
	updateTeachersListWidget();
	updateSubjectsComboBox();
	updateActivityTagsListWidget();

	minDayDistanceSpinBox->setMaximum(gt.rules.nDaysPerWeek);
	minDayDistanceSpinBox->setMinimum(0);
	minDayDistanceSpinBox->setValue(1);
	
	int nSplit=splitSpinBox->value();
	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++){
		if(i<nSplit)
			subactivitiesTabWidget->setTabEnabled(i, true);
		else
			subactivitiesTabWidget->setTabEnabled(i, false);
	}
	
	minDayDistanceTextLabel->setEnabled(nSplit>=2);
	minDayDistanceSpinBox->setEnabled(nSplit>=2);
	percentageTextLabel->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	
	subactivitiesTabWidget->setCurrentIndex(0);
	
	nStudentsSpinBox->setMinimum(-1);
	nStudentsSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	nStudentsSpinBox->setValue(-1);

	addActivityPushButton->setDefault(true);
	addActivityPushButton->setFocus();
	
	if(teacherName!="")
		selectedTeachersListWidget->addItem(teacherName);
	if(studentsSetName!="")
		selectedStudentsListWidget->addItem(studentsSetName);
	if(subjectName!=""){
		int pos=-1;
		for(int i=0; i<subjectsComboBox->count(); i++){
			if(subjectsComboBox->itemText(i)==subjectName){
				pos=i;
				break;
			}
		}
		assert(pos>=0);
		subjectsComboBox->setCurrentIndex(pos);
	}
	else{
		//begin trick to pass a Qt 4.6.0 bug: the first entry is not highlighted with mouse until you move to second entry and then back up
		if(subjectsComboBox->view()){
			subjectsComboBox->view()->setCurrentIndex(QModelIndex());
		}
		//end trick
		subjectsComboBox->setCurrentIndex(-1);
	}
	if(activityTagName!="")
		selectedActivityTagsListWidget->addItem(activityTagName);
		
	foreach(Teacher* tch, gt.rules.teachersList)
		teacherNamesSet.insert(tch->name);
	foreach(Subject* sbj, gt.rules.subjectsList)
		subjectNamesSet.insert(sbj->name);
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagNamesSet.insert(at->name);
	/*foreach(StudentsYear* year, gt.rules.yearsList){
		numberOfStudentsHash.insert(year->name, year->numberOfStudents);
		foreach(StudentsGroup* group, year->groupsList){
			numberOfStudentsHash.insert(group->name, group->numberOfStudents);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				numberOfStudentsHash.insert(subgroup->name, subgroup->numberOfStudents);
			}
		}
	}*/
}

AddActivityForm::~AddActivityForm()
{
	saveFETDialogGeometry(this);
}

void AddActivityForm::updateTeachersListWidget()
{
	allTeachersListWidget->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		allTeachersListWidget->addItem(tch->name);
	}
	
	selectedTeachersListWidget->clear();
}

void AddActivityForm::addTeacher()
{
	if(allTeachersListWidget->currentRow()<0 || allTeachersListWidget->currentRow()>=allTeachersListWidget->count())
		return;
	
	for(int i=0; i<selectedTeachersListWidget->count(); i++)
		if(selectedTeachersListWidget->item(i)->text()==allTeachersListWidget->currentItem()->text())
			return;
			
	selectedTeachersListWidget->addItem(allTeachersListWidget->currentItem()->text());
	selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void AddActivityForm::removeTeacher()
{
	if(selectedTeachersListWidget->count()<=0 || selectedTeachersListWidget->currentRow()<0 ||
	 selectedTeachersListWidget->currentRow()>=selectedTeachersListWidget->count())
		return;
	
	int i=selectedTeachersListWidget->currentRow();
	selectedTeachersListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedTeachersListWidget->takeItem(i);
	delete item;
	if(i<selectedTeachersListWidget->count())
		selectedTeachersListWidget->setCurrentRow(i);
	else
		selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void AddActivityForm::addStudents()
{
	if(allStudentsListWidget->currentRow()<0 || allStudentsListWidget->currentRow()>=allStudentsListWidget->count())
		return;
	
	assert(canonicalStudentsSetsNames.count()==allStudentsListWidget->count());
	QString sn=canonicalStudentsSetsNames.at(allStudentsListWidget->currentRow());

	for(int i=0; i<selectedStudentsListWidget->count(); i++)
		if(selectedStudentsListWidget->item(i)->text()==sn)
			return;
	
	selectedStudentsListWidget->addItem(sn);
	selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void AddActivityForm::removeStudents()
{
	if(selectedStudentsListWidget->count()<=0 || selectedStudentsListWidget->currentRow()<0 ||
	 selectedStudentsListWidget->currentRow()>=selectedStudentsListWidget->count())
		return;
	
	int i=selectedStudentsListWidget->currentRow();
	selectedStudentsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedStudentsListWidget->takeItem(i);
	delete item;
	if(i<selectedStudentsListWidget->count())
		selectedStudentsListWidget->setCurrentRow(i);
	else
		selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void AddActivityForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;
	
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;
	
	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddActivityForm::removeActivityTag()
{
	if(selectedActivityTagsListWidget->count()<=0 || selectedActivityTagsListWidget->currentRow()<0 ||
	 selectedActivityTagsListWidget->currentRow()>=selectedActivityTagsListWidget->count())
		return;
	
	int i=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(i);
	delete item;
	if(i<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(i);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddActivityForm::updateSubjectsComboBox()
{
	subjectsComboBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sbj->name);
	}
}

void AddActivityForm::updateActivityTagsListWidget()
{
	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
	
	selectedActivityTagsListWidget->clear();
}

void AddActivityForm::showYearsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::showGroupsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::showSubgroupsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::updateStudentsListWidget()
{
	const int INDENT=2;

	bool showYears=yearsCheckBox->isChecked();
	bool showGroups=groupsCheckBox->isChecked();
	bool showSubgroups=subgroupsCheckBox->isChecked();

	allStudentsListWidget->clear();
	canonicalStudentsSetsNames.clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(showYears){
			allStudentsListWidget->addItem(sty->name);
			canonicalStudentsSetsNames.append(sty->name);
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(showGroups){
				QString begin=QString("");
				QString end=QString("");
				begin=QString(INDENT, ' ');
				allStudentsListWidget->addItem(begin+stg->name+end);
				canonicalStudentsSetsNames.append(stg->name);
			}
			if(showSubgroups) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];

				QString begin=QString("");
				QString end=QString("");
				begin=QString(2*INDENT, ' ');
				allStudentsListWidget->addItem(begin+sts->name+end);
				canonicalStudentsSetsNames.append(sts->name);
			}
		}
	}
	
	int q=allStudentsListWidget->verticalScrollBar()->minimum();
	allStudentsListWidget->verticalScrollBar()->setValue(q);
}

void AddActivityForm::splitChanged()
{
	int nSplit=splitSpinBox->value();
	
	minDayDistanceTextLabel->setEnabled(nSplit>=2);
	minDayDistanceSpinBox->setEnabled(nSplit>=2);
	percentageTextLabel->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);

	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++)
		if(i<nSplit)
			subactivitiesTabWidget->setTabEnabled(i, true);
		else
			subactivitiesTabWidget->setTabEnabled(i, false);
}

SecondMinDaysDialog::SecondMinDaysDialog(QWidget* p, int minD, double w) :QDialog(p)
{
	weight=-1;

	QString l=tr
	 ("You selected min days between activities %1 (above 1) and weight %2 (under 100.0). "
	  "Would you like to add also a second constraint to ensure that almost certainly the "
	  "distance between activities is at least %3 (%1-1) days? If yes, please select weight (recommended "
	  "95.0%-100.0%) and click Yes. If no, please click No (only one constraint will be added)").arg(CustomFETString::number(minD)).arg(w).arg(minD-1);
	l+="\n\n";
	l+=tr("(Yes means to add an additional constraint min %1 days between activities, weight 0.0%-100.0%. "
	  "If you say Yes, you will have 2 constraints min days added for current activities. "
	  "Adding the second constraint might lead to impossible timetables if the condition is "
	  "too tight, but you can remove the second constraint at any time).").arg(minD-1);
	l+="\n\n";
	l+=tr("Note: 95% is usually enough for min days constraints referring to same activities. "
	  "The weights are cumulated if referring to the same activities. If you have 2 constraints with say 95%"
	  " (say min n days and min n-1 days), "
	  "the min n days constraint is skipped with probability 5%, then min n-1 days constraint is skipped with "
	  "probability 0.25%=5%*5%, so you'll get in 99.75% cases the min n-1 days constraint respected.");
	l+="\n\n";
	l+=tr("Recommended answer is Yes, 95% (or higher).");

	setWindowTitle(tr("Add a second constraint or not?"));
				
	QVBoxLayout* vl=new QVBoxLayout(this);
	
	QPlainTextEdit* la=new QPlainTextEdit();
	la->setPlainText(l);
	la->setReadOnly(true);

	vl->addWidget(la);
				
	QPushButton* yes=new QPushButton(tr("Yes"));
	yes->setDefault(true);
	
	QPushButton* no=new QPushButton(tr("No"));

	QLabel* percLabel=new QLabel(this);
	percLabel->setText("Percentage");
	percText=new QLineEdit(this);
	percText->setText("95.0");
	
	//QHBoxLayout* hl2=new QHBoxLayout(vl);
	QHBoxLayout* hl2=new QHBoxLayout();
	vl->addLayout(hl2);
	
	//////
	QLabel* minDaysLabel=new QLabel(this);
	minDaysLabel->setText("Min days");
	QSpinBox* minDaysSpinBox=new QSpinBox(this);
	minDaysSpinBox->setMinimum(minD-1);
	minDaysSpinBox->setMaximum(minD-1);
	minDaysSpinBox->setValue(minD-1);
	minDaysSpinBox->setEnabled(false);
	//////

	//////
	hl2->addStretch(1);
	hl2->addWidget(minDaysLabel);
	hl2->addWidget(minDaysSpinBox);
	//////
	
	hl2->addStretch(1);
	hl2->addWidget(percLabel);
	hl2->addWidget(percText);
	
	//QHBoxLayout* hl=new QHBoxLayout(vl);
	QHBoxLayout* hl=new QHBoxLayout();
	vl->addLayout(hl);
	
	hl->addStretch(1);
	hl->addWidget(yes);
	hl->addWidget(no);
	
	connect(yes, SIGNAL(clicked()), this, SLOT(yesPressed()));
	connect(no, SIGNAL(clicked()), this, SLOT(reject()));
	
	int ww=this->sizeHint().width();
	if(ww>1000)
		ww=1000;
	if(ww<590)
		ww=590;

	int hh=this->sizeHint().height();
	if(hh>650)
		hh=650;
	if(hh<380)
		hh=380;
	
	this->resize(ww, hh);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

SecondMinDaysDialog::~SecondMinDaysDialog()
{
	saveFETDialogGeometry(this);
}

void SecondMinDaysDialog::yesPressed()
{
	double wt;
	QString tmp=percText->text();
	weight_sscanf(tmp, "%lf", &wt);
	if(wt<0.0 || wt>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be >=0 and <=100.0"));
		return;
	}
	weight=wt;
	accept();
}

void AddActivityForm::addActivity()
{
	double weight;
	QString tmp=percentageLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(percentageLineEdit->isEnabled() && (weight<0.0 || weight>100.0)){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) for added constraint min days between activities"));
		return;
	}

	//teachers
	QStringList teachers_names;
	if(selectedTeachersListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to add an activity without teacher(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedTeachersListWidget->count(); i++){
			//assert(gt.rules.searchTeacher(selectedTeachersListWidget->item(i)->text())>=0);
			assert(teacherNamesSet.contains(selectedTeachersListWidget->item(i)->text()));
			teachers_names.append(selectedTeachersListWidget->item(i)->text());
		}
	}

	//subject
	QString subject_name=subjectsComboBox->currentText();
	bool found=subjectNamesSet.contains(subject_name);
	/*int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){*/
	if(!found){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid subject"));
		return;
	}

	QStringList activity_tags_names;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++){
		//assert(gt.rules.searchActivityTag(selectedActivityTagsListWidget->item(i)->text())>=0);
		assert(activityTagNamesSet.contains(selectedActivityTagsListWidget->item(i)->text()));
		activity_tags_names.append(selectedActivityTagsListWidget->item(i)->text());
	}

	//students
	int numberOfStudents=0;
	QStringList students_names;
	if(selectedStudentsListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to add an activity without student set(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedStudentsListWidget->count(); i++){
			//assert(gt.rules.searchStudentsSet(selectedStudentsListWidget->item(i)->text())!=NULL);
			/*assert(numberOfStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=numberOfStudentsHash.value(selectedStudentsListWidget->item(i)->text());*/
			assert(gt.rules.permanentStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=gt.rules.permanentStudentsHash.value(selectedStudentsListWidget->item(i)->text())->numberOfStudents;
			students_names.append(selectedStudentsListWidget->item(i)->text());
		}

		if(nStudentsSpinBox->value()>=0)
			numberOfStudents=nStudentsSpinBox->value();
	}

	if(splitSpinBox->value()==1){ //indivisible activity
		int duration=duration1SpinBox->value();
		if(duration<0){
			QMessageBox::warning(this, tr("FET information"),
				tr("Invalid duration"));
			return;
		}

		bool active=false;
		if(active1CheckBox->isChecked())
			active=true;

		int activityid=0; //We set the id of this newly added activity = (the largest existing id + 1)
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > activityid)
				activityid = act->id;
		}
		activityid++;
		Activity a(gt.rules, activityid, 0, teachers_names, subject_name, activity_tags_names, students_names,
			duration, duration, /*parity,*/ active, (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);

		bool already_existing=false;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if((*act)==a)
				already_existing=true;
		}

		if(already_existing){
			int t=QMessageBox::question(this, tr("FET question"), 
				//tr("This activity already exists. Insert it again?"),
				tr("A similar activity already exists. Do you want to insert current activity?"),
				tr("Yes"),tr("No"));
			assert(t==0 || t==1 ||t==-1);
			if(t==1) //no pressed
				return;
			if(t==-1) //Esc pressed
				return;
		}

		bool tmp=gt.rules.addSimpleActivityFast(this, activityid, 0, teachers_names, subject_name, activity_tags_names,
			students_names, duration, duration, active,
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);
		if(tmp)
			QMessageBox::information(this, tr("FET information"), tr("Activity added"));
		else
			QMessageBox::critical(this, tr("FET information"), tr("Activity NOT added - please report error"));
	}
	else{ //split activity
		if(minDayDistanceSpinBox->value()>0 && splitSpinBox->value()>gt.rules.nDaysPerWeek){
			int t=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
			 tr("Possible incorrect setting. Are you sure you want to add current activity? See details below:")+"\n\n"+
			 tr("You want to add a container activity split into more than the number of days per week and also add a constraint min days between activities."
			  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
			 "\n\n"+
			 tr("The best way to add the activities would be:")+
			 "\n\n"+
			 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
			  ". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
			  " (possibly raising the weight of added constraint min days between activities up to 100%)")+
			  "\n\n"+
			 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
			  " activities equal with the number of days per week and the remaining components into other larger split activity."
			  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
			  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
			  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+
		  	 "\n\n"+
			 tr("Do you want to add current activities as they are now (not recommended) or cancel and edit them as instructed?")
			  ,
			 tr("Yes"), tr("No"), QString(), 0, 1);

			if(t==1)
				return;
		}

		int totalduration;
		int durations[MAX_SPLIT_OF_AN_ACTIVITY];
		bool active[MAX_SPLIT_OF_AN_ACTIVITY];
		int nsplit=splitSpinBox->value();

		totalduration=0;
		for(int i=0; i<nsplit; i++){
			durations[i]=dur(i)->value();
			active[i]=false;
			if(activ(i)->isChecked())
				active[i]=true;

			totalduration+=durations[i];
		}

		//the group id of this split activity and the id of the first partial activity
		//it is the maximum already existing id + 1
		int firstactivityid=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > firstactivityid)
				firstactivityid = act->id;
		}
		firstactivityid++;

		int minD=minDayDistanceSpinBox->value();
		bool tmp=gt.rules.addSplitActivityFast(this, firstactivityid, firstactivityid,
			teachers_names, subject_name, activity_tags_names, students_names,
			nsplit, totalduration, durations,
			active, minD, weight, forceConsecutiveCheckBox->isChecked(),
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);
		if(tmp){
			if(minD>1 && weight<100.0){
				SecondMinDaysDialog second(this, minD, weight);
				setParentAndOtherThings(&second, this);
				int code=second.exec();

				if(code==QDialog::Accepted){
					assert(second.weight>=0 && second.weight<=100.0);
					QList<int> acts;
					for(int i=0; i<nsplit; i++){
						acts.append(firstactivityid+i);
					}
					TimeConstraint* c=new ConstraintMinDaysBetweenActivities(second.weight, forceConsecutiveCheckBox->isChecked(), nsplit, acts, minD-1);
					bool tmp=gt.rules.addTimeConstraint(c);
					assert(tmp);
				}
			}
		
			QMessageBox::information(this, tr("FET information"), tr("Split activity added."
			 " Please note that FET currently cannot check for duplicates when adding split activities"
			 ". It is advisable to check the statistics after adding all the activities"));
		}
		else
			QMessageBox::critical(this, tr("FET information"), tr("Split activity NOT added - error???"));
	}

	PlanningChanged::increasePlanningCommunicationSpinBox();
}

void AddActivityForm::clearTeachers()
{
	selectedTeachersListWidget->clear();
}

void AddActivityForm::clearStudents()
{
	selectedStudentsListWidget->clear();
}

void AddActivityForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void AddActivityForm::help()
{
	QString s;
	
	s+=tr("Abbreviations in this dialog:");
	s+="\n\n";
	s+=tr("'Students' (the text near the spin box), means 'Number of students (-1 for automatic)'");
	s+="\n";
	s+=tr("'Split' means 'Split into ... activities per week'");
	s+="\n";
	s+=tr("'Min days' means 'The minimum required distance in days between each pair of activities'");
	s+="\n";
	s+=tr("'Weight %' means 'Percentage of added constraint (min days between activities constraint). Recommended: 95.0%-100.0%'");
	s+="\n";
	s+=tr("'Consecutive' means 'If activities on same day, force consecutive?'");
	s+="\n";
	s+=tr("The 'Duration' spin box and the 'Active' check box refer to each component of current activity, you can change "
	 "them for each component, separately, by selecting the corresponding tab in the tab widget.");
	s+="\n\n";
	
	s+=tr("A first notice: "
	 "If you use a 5 days week: "
	 "when adding an activity split into only 2 components "
	 "per week, the best practice is to add min days between activities to be 2. "
	 "If you split an activity into 3 components per week - please read FAQ question Q1-5-September-2008");
	s+="\n\n";
	
	s+=tr("You can select a teacher from all the teachers with the mouse or with the keyboard tab/up/down, then "
	 "double click it to add it to the selected teachers for current activity. "
	 "You can then choose to remove a teacher from the selected teachers. You can highlight it "
	 "with the mouse or with the keyboard, then double click it to remove this teacher from the selected teachers.");
	 
	s+="\n\n";
	
	s+=tr("The same procedure (double click) applies to students sets and activity tags.");
	
	s+="\n\n";
	
	s+=tr("You can check/uncheck show years, show groups or show subgroups.");
	s+="\n\n";
	
	 s+=tr("If you split a larger activity into more activities per week, you have a multitude of choices:\n"
	 "You can choose the minimum distance in days between each pair of activities."
	 " Please note that a minimum distance of 1 means that the activities must not be in the same day, "
	 "a minimum distance of 2 means that the activities must be separated by one day (distance from Monday"
	 " to Wednesday for instance is 2 days), etc.");

	s+="\n\n";
	 
	 s+=tr("If you have for instance an activity with 2 lessons per week and you want to spread them to at "
	 "least 2 days distance, you can add a constraint min days with min days = 2 and weight 95% "
	 "(or higher). If you want also to ensure that activities will "
	 "be separated by at least one day, you can use this feature: "
	 "add a constraint min days with minimum days 2 and weight 95% or lower, and after that you'll get "
	 "the possibility to add another constraint with min 1 days and weight 95% or higher. "
	 "It works if you first select in the dialog the min days >= 2 and click Add activities. Or you can add manually the constraints "
	 "(difficult this way). "
	 "Important: it is best practice to consider both constraints to have 95% weight. The combination assures that "
	 "the resultant is 99.75% weight");

	s+="\n\n";
	 
	s+=tr("Please note that the min days distance is a time constraint and you can only see/modify it in the "
	 "time constraints dialogs, not in the modify activity dialog. Additionally, you can see the constraints "
	 "for each activity in the details text box of each activity");

	s+="\n\n";
	 
	 s+=tr("If you choose a value greater or equal with 1 for min days, a time constraint min days between activities will be added automatically "
	 "(you can see this constraint in the time constraints list or you can see this constraint in the "
	 "detailed description of the activity). You can select a weight percentage for this constraint. "
	 "If you select 100%, the constraint must be respected all the time. If you select 95%, there is a small chance "
	 "that the timetable will not respect this constraint. Recommended values are 95.0%-100.0% (maybe you could try "
	 "with 95%, then 99.75%, or even 100.0%, but the generation time might be larger). Generally, 99.75% might be a good value. "
	 "Note: if you put a value less than 100% and the constraint is too tough, FET is able to find that this constraint "
	 "is impossible and will break it. 99.75% might be better than 95% but possibly slower. The percentage is subjective "
	 "(if you put 95% you may get 6 soft conflicts and if you put 99.75% you may get 3 soft conflicts). "
	 "Starting with FET-5.3.6, it is possible to change this value for all constraints in one click, in constraint min days"
	 " between activities dialog.");

	s+="\n\n";

	s+=tr("There is another option, if the activities are in the same day, force consecutive activities. You can select "
	 "this option for instance if you have 5 lessons of math in 5 days, and there is no timetable which respects "
	 "fully the days separation. Then, you can set the weight percent of the min days constraint to 95% and "
	 "add consecutive if same day. You will have as results say 3 lessons with duration 1 and a 2 hours lesson in another day. "
	 "Please be careful: if the activities are on the same day, even if the constraint has 0% weight, then the activities are forced to be "
	 "consecutive.");

	s+="\n\n";

	s+=tr("Current algorithm cannot schedule 3 activities in the same day if consecutive is checked, so "
	 "you will get no solution in such extreme cases (for instance, if you have 3 lessons and a teacher which works only 1 day per week, "
	 "and select 'force consecutive if same day', you will get an imposssible timetable. But these are extremely unlikely cases).");

	s+="\n\n";
	
	s+=tr("Note: You cannot add 'consecutive if same day' with min days=0. If you want this, you have to add "
	 "min days at least 1 (and any weight percentage).");

	s+="\n\n";
	
	s+=tr("Starting with version 5.0.0, it is possible to add activities with no students or no teachers");

	s+="\n\n";
	
	s+=tr("If you select a number of min days above 1 (say this number is n), you will get the possibility "
	 "to add a second constraint min days between activities, with min days = n-1 and a percentage of your choice. Just click "
	 "Add activities");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on adding activity(ies)"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
}

void AddActivityForm::minDaysChanged()
{
	percentageTextLabel->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
}
