#ifndef TEACHERSTUDENTSETSUBJECTACTIVITYTAG_FILTERWIDGET_H
#define TEACHERSTUDENTSETSUBJECTACTIVITYTAG_FILTERWIDGET_H

#include <QWidget>
#include "rules.h"

namespace Ui {
class TeacherStudentSetSubjectActivityTag_FilterWidget;
}

class TeacherStudentSetSubjectActivityTag_FilterWidget : public QWidget
{
	Q_OBJECT

signals:
	 void FilterChanged();

public:
	explicit TeacherStudentSetSubjectActivityTag_FilterWidget(const Rules &rules);
	~TeacherStudentSetSubjectActivityTag_FilterWidget();
	QString teacher();
	QString studentsSet();
	QString subject();
	QString activityTag();

private:
	Ui::TeacherStudentSetSubjectActivityTag_FilterWidget *ui;

public slots:
	void onChange();
};

#endif // TEACHERSTUDENTSETSUBJECTACTIVITYTAG_FILTERWIDGET_H
