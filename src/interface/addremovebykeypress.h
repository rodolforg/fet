#ifndef ADDREMOVEBYKEYPRESS_H
#define ADDREMOVEBYKEYPRESS_H

#include <QObject>

class AddRemoveByKeyPress : public QObject
{
	Q_OBJECT
public:
	explicit AddRemoveByKeyPress(QObject * obj, QObject * parent=nullptr);
	void setAddDstCallback(QString cb);
	void setRemoveDstCallback(QString cb);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
	QObject * obj;

	QString addToDst_cb;
	QString removeToDst_cb;
};

#endif // ADDREMOVEBYKEYPRESS_H
