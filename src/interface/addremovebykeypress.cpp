#include "addremovebykeypress.h"

#include <QKeyEvent>

AddRemoveByKeyPress::AddRemoveByKeyPress(QObject * obj, QObject * parent)
	: QObject(parent), obj(obj)
{
}

void AddRemoveByKeyPress::setAddDstCallback(QString cb)
{
	addToDst_cb = cb;
}
void AddRemoveByKeyPress::setRemoveDstCallback(QString cb)
{
	removeToDst_cb = cb;
}

bool AddRemoveByKeyPress::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_Plus) {
			if (!addToDst_cb.isEmpty()) {
				QMetaObject::invokeMethod(obj, addToDst_cb.toStdString().c_str(), Qt::QueuedConnection);
				return true;
			}
		} else if (keyEvent->key() == Qt::Key_Minus) {
			if (!removeToDst_cb.isEmpty()) {
				QMetaObject::invokeMethod(obj, removeToDst_cb.toStdString().c_str(), Qt::QueuedConnection);
				return true;
			}
		}
		return QObject::eventFilter(obj, event);
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
