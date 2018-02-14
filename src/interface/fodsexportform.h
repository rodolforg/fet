#ifndef FODSEXPORTFORM_H
#define FODSEXPORTFORM_H

#include <QDialog>
#include <QTextStream>

#include "rules.h"
#include "timetableexporthelper.h"

namespace Ui {
class FOdsExportForm;
}

class FOdsExportForm : public QDialog
{
	Q_OBJECT

public:
	FOdsExportForm(const Rules &rules, const Solution &solution, QWidget *parent = 0);
	~FOdsExportForm();

	void ok();

private:
	struct HourFilter {
		HourFilter(const QStringList& validHours, const QStringList& breakTimes, const QStringList& relabel);
		const QStringList& validHours;
		const QStringList& breakTimes;
		const QStringList& relabel;
	};

	void writeTable(QTextStream& text, const HourFilter &filter, const TimetableExportHelper::Table& table, const int tblIdx, const QString& tableName, const char* data_row_style, const char* table_style, int whatShow) const;

	Ui::FOdsExportForm *ui;
	const Rules &rules;
	const Solution &solution;

	static const QString template_1;

	static const char *template_2;

	static const char *table_open_tag;
	static const char *table_close_tag;

	static const QString table_column_tag;

	static const QString row_open_tag;
	static const char *row_close_tag;

	static const QString row_doc_header;

	static const char * cell_default_open_tag;
	static const QString cell_styled_open_tag;
	static const QString cell_default_span_open_tag;
	static const QString cell_styled_span_open_tag;
	static const QString cell_empty_tag;
	static const QString cell_empty_covered_tag;
	static const QString cell_empty_covered_spanned_tag;
	static const char *cell_close_tag;

	static const QString text_par_tag;
	static const QString text_span_tag;

	static const QString timespan_row_label_format;

	enum ActivityFlags {
		SUBJECT      = 1,
		TEACHERS     = 2,
		STUDENTS     = 4,
		ACTIVITY_TAG = 8,
		ROOM         = 16,
		TIME         = 32,
		STUDENTS_ONLY_IF_DIFFERENT = 64,
		TEACHERS_ONLY_IF_DIFFERENT = 128,
		TIME_ROW_LABEL = 256
	};

	QString getActivityText(const Activity* act, int flags, int tblIdx) const;

	QString getEndHourLabel(int current_hour, const HourFilter& filter) const;
	int getEndHour(int current_hour, int timeSpan, const HourFilter& filter) const;
	int getRowSpan(int start_hour, int end_hour, const QList<int>& validHoursId) const;
};

#endif // FODSEXPORTFORM_H
