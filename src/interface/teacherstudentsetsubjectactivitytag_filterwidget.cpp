#include "teacherstudentsetsubjectactivitytag_filterwidget.h"
#include "ui_teacherstudentsetsubjectactivitytag_filterwidget.h"

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

	ui->teachersComboBox->addItem("");
	for(int i=0; i<rules.teachersList.size(); i++){
		Teacher* tch=rules.teachersList[i];
		ui->teachersComboBox->addItem(tch->name);
	}
	ui->teachersComboBox->setCurrentIndex(0);

	ui->subjectsComboBox->addItem("");
	for(int i=0; i<rules.subjectsList.size(); i++){
		Subject* sb=rules.subjectsList[i];
		ui->subjectsComboBox->addItem(sb->name);
	}
	ui->subjectsComboBox->setCurrentIndex(0);

	ui->activityTagsComboBox->addItem("");
	for(int i=0; i<rules.activityTagsList.size(); i++){
		ActivityTag* st=rules.activityTagsList[i];
		ui->activityTagsComboBox->addItem(st->name);
	}
	ui->activityTagsComboBox->setCurrentIndex(0);

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
	ui->studentsComboBox->setCurrentIndex(0);

	connect(ui->teachersComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->studentsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
	connect(ui->activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));

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
