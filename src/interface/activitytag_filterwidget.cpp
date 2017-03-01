#include "activitytag_filterwidget.h"
#include "ui_activitytag_filterwidget.h"

ActivityTag_FilterWidget::ActivityTag_FilterWidget(const Rules &rules) :
	QWidget(),
	ui(new Ui::ActivityTag_FilterWidget)
{
	ui->setupUi(this);

	QSize tmp1=ui->activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);

	ui->activityTagsComboBox->addItem("");
	for(int i=0; i<rules.activityTagsList.size(); i++){
		ActivityTag* st=rules.activityTagsList[i];
		ui->activityTagsComboBox->addItem(st->name);
	}
	ui->activityTagsComboBox->setCurrentIndex(0);

	connect(ui->activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(onChange()));
}

ActivityTag_FilterWidget::~ActivityTag_FilterWidget()
{
	delete ui;
}

void ActivityTag_FilterWidget::onChange()
{
	emit FilterChanged();
}

QString ActivityTag_FilterWidget::activityTag() const
{
	return ui->activityTagsComboBox->currentText();
}
