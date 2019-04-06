#include "editcommentsform.h"
#include "ui_editcommentsform_template.h"

#include "centerwidgetonscreen.h"

EditCommentsForm::EditCommentsForm(const QString &settingsName, QWidget *parent, const QString &title) :
	QDialog(parent),
	ui(new Ui::EditCommentsForm_template),
	settingsName(settingsName)
{
	ui->setupUi(this);
	setWindowTitle(title);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, settingsName);

	ui->commentsPT->installEventFilter(this);
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

bool EditCommentsForm::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->commentsPT && event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->modifiers() == Qt::CTRL) {
			if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
				accept();
				return true;
			}
        }
    }
	return false;
}
