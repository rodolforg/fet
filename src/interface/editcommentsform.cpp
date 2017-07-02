#include "editcommentsform.h"
#include "ui_editcommentsform_template.h"

#include "interface/centerwidgetonscreen.h"

EditCommentsForm::EditCommentsForm(const QString &settingsName, QWidget *parent, const QString &title) :
	QDialog(parent),
	ui(new Ui::EditCommentsForm_template),
	settingsName(settingsName)
{
	ui->setupUi(this);
	setWindowTitle(title);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, settingsName);
}

EditCommentsForm::~EditCommentsForm()
{
	saveFETDialogGeometry(this, settingsName);
	delete ui;
}

void EditCommentsForm::setComments(const QString &comments)
{
	ui->commentsPT->setPlainText(comments);
	if (!isVisible())
		ui->commentsPT->selectAll();
}

QString EditCommentsForm::getComments() const
{
	return ui->commentsPT->toPlainText();
}
