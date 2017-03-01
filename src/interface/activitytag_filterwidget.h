#ifndef ACTIVITYTAG_FILTERWIDGET_H
#define ACTIVITYTAG_FILTERWIDGET_H

#include <QWidget>
#include "rules.h"

namespace Ui {
class ActivityTag_FilterWidget;
}

class ActivityTag_FilterWidget : public QWidget
{
	Q_OBJECT

signals:
	 void FilterChanged();

public:
	explicit ActivityTag_FilterWidget(const Rules &rules);
	~ActivityTag_FilterWidget();
	QString activityTag() const;

private:
	Ui::ActivityTag_FilterWidget *ui;

public slots:
	void onChange();
};

#endif // ACTIVITYTAG_FILTERWIDGET_H
