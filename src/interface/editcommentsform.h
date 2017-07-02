#ifndef EDITCOMMENTSFORM_H
#define EDITCOMMENTSFORM_H

#include <QDialog>

namespace Ui {
class EditCommentsForm_template;
}

class EditCommentsForm : public QDialog
{
	Q_OBJECT

public:
	explicit EditCommentsForm(const QString& settingsName, QWidget *parent = 0, const QString &title = "");
	~EditCommentsForm();

	void setComments(const QString &comments);
	QString getComments() const;

private:
	Ui::EditCommentsForm_template *ui;
	QString settingsName;
};

#endif // EDITCOMMENTSFORM_H
