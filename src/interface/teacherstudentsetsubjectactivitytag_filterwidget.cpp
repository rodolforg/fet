#include "teacherstudentsetsubjectactivitytag_filterwidget.h"
#include "ui_teacherstudentsetsubjectactivitytag_filterwidget.h"
#include <QDebug>

TeacherStudentSetSubjectActivityTag_FilterWidget::TeacherStudentSetSubjectActivityTag_FilterWidget(const Rules &rules) :
	QWidget(),
	ui(new Ui::TeacherStudentSetSubjectActivityTag_FilterWidget)
{
	ui->setupUi(this);

	QSize tmp1=ui->teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=ui->studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=ui->subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=ui->activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	QSize tmp5=ui->roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	populateTeachers(rules);
	ui->teachersComboBox->setCurrentIndex(0);

	populateSubjects(rules);
	ui->subjectsComboBox->setCurrentIndex(0);

	populateActivityTags(rules);
	ui->activityTagsComboBox->setCurrentIndex(0);

	populateStudentsSets(rules);
	ui->studentsComboBox->setCurrentIndex(0);

	populateRooms(rules);
	ui->roomsComboBox->setCurrentIndex(0);

	connect(ui->teachersComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->studentsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->roomsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));

	setTeachersVisible(false);
	setSubjectsVisible(false);
	setStudentSetsVisible(false);
	setActivityTagsVisible(false);
	setRoomsVisible(false);
}

TeacherStudentSetSubjectActivityTag_FilterWidget::~TeacherStudentSetSubjectActivityTag_FilterWidget()
{
	delete ui;
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::onChange()
{
	emit FilterChanged();
}

QString TeacherStudentSetSubjectActivityTag_FilterWidget::teacher() const
{
	return ui->teachersComboBox->currentText();
}

QString TeacherStudentSetSubjectActivityTag_FilterWidget::studentsSet() const
{
	return ui->studentsComboBox->currentText();
}

QString TeacherStudentSetSubjectActivityTag_FilterWidget::subject() const
{
	return ui->subjectsComboBox->currentText();
}

QString TeacherStudentSetSubjectActivityTag_FilterWidget::activityTag() const
{
	return ui->activityTagsComboBox->currentText();
}

QString TeacherStudentSetSubjectActivityTag_FilterWidget::room() const
{
	return ui->roomsComboBox->currentText();
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setTeachersVisible(bool visible)
{
	ui->teachersLabel->setVisible(visible);
	ui->teachersComboBox->setVisible(visible);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setStudentSetsVisible(bool visible)
{
	ui->studentsLabel->setVisible(visible);
	ui->studentsComboBox->setVisible(visible);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setSubjectsVisible(bool visible)
{
	ui->subjectsLabel->setVisible(visible);
	ui->subjectsComboBox->setVisible(visible);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setActivityTagsVisible(bool visible)
{
	ui->activityTagsLabel->setVisible(visible);
	ui->activityTagsComboBox->setVisible(visible);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setRoomsVisible(bool visible)
{
	ui->roomsLabel->setVisible(visible);
	ui->roomsComboBox->setVisible(visible);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::hideLabels()
{
	ui->teachersLabel->hide();
	ui->studentsLabel->hide();
	ui->subjectsLabel->hide();
	ui->activityTagsLabel->hide();
	ui->roomsLabel->hide();
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::setDirection(QBoxLayout::Direction direction)
{
	if (!ui->filterGroupBox->layout()->metaObject()->inherits(&QBoxLayout::staticMetaObject)) {
		qWarning() << "Filter layout is not QBoxLayout. Direction not changed.";
		return;
	}
	((QBoxLayout*)ui->filterGroupBox->layout())->setDirection(direction);
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::populateTeachers(const Rules &rules)
{
	ui->teachersComboBox->addItem("");
	for(int i=0; i<rules.teachersList.size(); i++){
		Teacher* tch=rules.teachersList[i];
		ui->teachersComboBox->addItem(tch->name);
	}
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::populateStudentsSets(const Rules &rules)
{
	ui->studentsComboBox->addItem("");
	for(int i=0; i<rules.yearsList.size(); i++){
		StudentsYear* sty=rules.yearsList[i];
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

void TeacherStudentSetSubjectActivityTag_FilterWidget::populateSubjects(const Rules &rules)
{
	ui->subjectsComboBox->addItem("");
	for(int i=0; i<rules.subjectsList.size(); i++){
		Subject* sb=rules.subjectsList[i];
		ui->subjectsComboBox->addItem(sb->name);
	}
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::populateActivityTags(const Rules &rules)
{
	ui->activityTagsComboBox->addItem("");
	for(int i=0; i<rules.activityTagsList.size(); i++){
		ActivityTag* st=rules.activityTagsList[i];
		ui->activityTagsComboBox->addItem(st->name);
	}
}

void TeacherStudentSetSubjectActivityTag_FilterWidget::populateRooms(const Rules &rules)
{
	ui->roomsComboBox->addItem("");
	for(int i=0; i<rules.roomsList.size(); i++){
		Room* room=rules.roomsList[i];
		ui->roomsComboBox->addItem(room->name);
	}
}
