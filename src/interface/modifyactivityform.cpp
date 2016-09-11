/***************************************************************************
                          modifyactivityform.cpp  -  description
                             -------------------
    begin                : Feb 9, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "modifyactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include "activityplanningform.h"

#include <QMessageBox>

#include <QPlainTextEdit>

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

QSpinBox* ModifyActivityForm::dur(int i)
{
	assert(i>=0 && i<durList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return durList.at(i);
}

QCheckBox* ModifyActivityForm::activ(int i)
{
	assert(i>=0 && i<activList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return activList.at(i);
}

ModifyActivityForm::ModifyActivityForm(QWidget* parent, int id, int activityGroupId): QDialog(parent)
{
	setupUi(this);

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
	
	allTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(clearTeacherPushButton, SIGNAL(clicked()), this, SLOT(clearTeachers()));
	connect(clearStudentsPushButton, SIGNAL(clicked()), this, SLOT(clearStudents()));
	connect(allTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addTeacher()));
	connect(selectedTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeTeacher()));
	connect(allStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addStudents()));
	connect(selectedStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeStudents()));
	connect(clearActivityTagPushButton, SIGNAL(clicked()), this, SLOT(clearActivityTags()));
	connect(allActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addActivityTag()));
	connect(selectedActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeActivityTag()));
	connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showYearsChanged()));
	connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showGroupsChanged()));
	connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsChanged()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	this->_id=id;
	this->_activityGroupId=activityGroupId;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->activityGroupId==this->_activityGroupId && act->id==this->_id)
			this->_activity=act;
	}
			
	this->_teachers=this->_activity->teachersNames;
	this->_subject = this->_activity->subjectName;
	this->_activityTags = this->_activity->activityTagsNames;
	this->_students=this->_activity->studentsNames;
	
	int nSplit;
	
	if(this->_activityGroupId!=0){
		nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->activityGroupId==this->_activityGroupId){
				if(nSplit>=MAX_SPLIT_OF_AN_ACTIVITY){
					assert(0);
				}
				else{
					if(this->_id==act->id)
						subactivitiesTabWidget->setCurrentIndex(nSplit);
					dur(nSplit)->setValue(act->duration);
					activ(nSplit)->setChecked(act->active);
					nSplit++;
				}
			}
		}
	}
	else{
		nSplit=1;
		dur(0)->setValue(this->_activity->duration);
		activ(0)->setChecked(this->_activity->active);
		subactivitiesTabWidget->setCurrentIndex(0);
	}

	splitSpinBox->setMinimum(nSplit);
	splitSpinBox->setMaximum(nSplit);
	splitSpinBox->setValue(nSplit);
	
	nStudentsSpinBox->setMinimum(-1);
	nStudentsSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	nStudentsSpinBox->setValue(-1);
	
	if(this->_activity->computeNTotalStudents==false)
		nStudentsSpinBox->setValue(this->_activity->nTotalStudents);
	
	updateStudentsListWidget();
	updateTeachersListWidget();
	updateSubjectsComboBox();
	updateActivityTagsListWidget();
	
	selectedStudentsListWidget->clear();
	for(QStringList::Iterator it=this->_students.begin(); it!=this->_students.end(); it++)
		selectedStudentsListWidget->addItem(*it);

	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++)
		if(i<nSplit)
			subactivitiesTabWidget->setTabEnabled(i, true);
		else
			subactivitiesTabWidget->setTabEnabled(i, false);
			
	okPushButton->setDefault(true);
	okPushButton->setFocus();

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

ModifyActivityForm::~ModifyActivityForm()
{
	saveFETDialogGeometry(this);
}

void ModifyActivityForm::updateTeachersListWidget()
{
	allTeachersListWidget->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		allTeachersListWidget->addItem(tch->name);
	}
	
	selectedTeachersListWidget->clear();
	for(QStringList::Iterator it=this->_teachers.begin(); it!=this->_teachers.end(); it++)
		selectedTeachersListWidget->addItem(*it);
}

void ModifyActivityForm::addTeacher()
{
	if(allTeachersListWidget->currentRow()<0 || allTeachersListWidget->currentRow()>=allTeachersListWidget->count())
		return;
	
	for(int i=0; i<selectedTeachersListWidget->count(); i++)
		if(selectedTeachersListWidget->item(i)->text()==allTeachersListWidget->currentItem()->text())
			return;
	
	selectedTeachersListWidget->addItem(allTeachersListWidget->currentItem()->text());
	selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void ModifyActivityForm::removeTeacher()
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

void ModifyActivityForm::addStudents()
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

void ModifyActivityForm::removeStudents()
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

void ModifyActivityForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;

	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;

	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void ModifyActivityForm::removeActivityTag()
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

void ModifyActivityForm::updateSubjectsComboBox()
{
	int i=0, j=-1;
	subjectsComboBox->clear();
	for(int k=0; k<gt.rules.subjectsList.size(); k++, i++){
		Subject* sbj=gt.rules.subjectsList[k];
		subjectsComboBox->addItem(sbj->name);
		if(sbj->name==this->_subject)
			j=i;
	}
	assert(j!=-1);
	subjectsComboBox->setCurrentIndex(j);
}

void ModifyActivityForm::updateActivityTagsListWidget()
{
	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
		
	selectedActivityTagsListWidget->clear();
	for(QStringList::Iterator it=this->_activityTags.begin(); it!=this->_activityTags.end(); it++)
		selectedActivityTagsListWidget->addItem(*it);
}

void ModifyActivityForm::showYearsChanged()
{
	updateStudentsListWidget();
}

void ModifyActivityForm::showGroupsChanged()
{
	updateStudentsListWidget();
}

void ModifyActivityForm::showSubgroupsChanged()
{
	updateStudentsListWidget();
}

void ModifyActivityForm::updateStudentsListWidget()
{
	const int INDENT=2;

	bool showYears=showYearsCheckBox->isChecked();
	bool showGroups=showGroupsCheckBox->isChecked();
	bool showSubgroups=showSubgroupsCheckBox->isChecked();

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

void ModifyActivityForm::cancel()
{
	this->reject();
}

void ModifyActivityForm::ok()
{
	//teachers
	QStringList teachers_names;
	if(selectedTeachersListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the activity without teacher(s)?"),
		 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

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
	/*int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){*/
	bool found=subjectNamesSet.contains(subject_name);
	if(!found){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject"));
		return;
	}

	//activity tag
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
		 tr("Do you really want to have the activity without student set(s)?"),
		 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

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
	}

	/*int total_number_of_students=0;
	for(QStringList::Iterator it=students_names.begin(); it!=students_names.end(); it++){
		StudentsSet* ss=gt.rules.searchStudentsSet(*it);
		assert(ss!=NULL);
		total_number_of_students+=ss->numberOfStudents;
	}*/
	int total_number_of_students=numberOfStudents;

	int totalduration;
	int durations[MAX_SPLIT_OF_AN_ACTIVITY];
	bool active[MAX_SPLIT_OF_AN_ACTIVITY];
	int nsplit=splitSpinBox->value();

	totalduration=0;
	for(int i=0; i<nsplit; i++){
		durations[i]=dur(i)->value();
		active[i]=activ(i)->isChecked();

		totalduration+=durations[i];
	}

	if(nStudentsSpinBox->value()==-1){
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, nsplit, totalduration, durations, active,
		 (nStudentsSpinBox->value()==-1), total_number_of_students);
	}
	else{
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, nsplit, totalduration, durations, active,
		 (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
	}
	
	PlanningChanged::increasePlanningCommunicationSpinBox();
	
	this->accept();
}

void ModifyActivityForm::clearTeachers()
{
	selectedTeachersListWidget->clear();
}

void ModifyActivityForm::clearStudents()
{
	selectedStudentsListWidget->clear();
}

void ModifyActivityForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void ModifyActivityForm::help()
{
	QString s;
	
	s+=tr("Abbreviations in this dialog:");
	s+="\n\n";
	s+=tr("'Students' (the text near the spin box), means 'Number of students (-1 for automatic)'");
	s+="\n";
	s+=tr("'Split' means 'Is split into ... activities per week'. This value cannot be changed."
	 " If you need to modify it, please remove the corresponding activities and add a new split activity.");
	s+="\n";
	s+=tr("The 'Duration' spin box and the 'Active' check box refer to each component of current activity, you can change "
	 "them for each component, separately, by selecting the corresponding tab in the tab widget.");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on modifying activity(ies)"));

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

	dialog.resize(600,470);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
}
