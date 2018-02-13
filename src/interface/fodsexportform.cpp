#include "fodsexportform.h"
#include "ui_fodsexportform_template.h"

#include "timetableexporthelper.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <QFileDialog>

FOdsExportForm::FOdsExportForm(const Rules &rules, const Solution& solution, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FOdsExportForm),
	rules(rules),
	solution(solution)
{
	ui->setupUi(this);
	ok();
}

FOdsExportForm::~FOdsExportForm()
{
	delete ui;
}

void FOdsExportForm::ok()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Export to Flat-ODS"), "", tr("Flat ODS Files (*.fods)"));
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("FET warning"),
			tr("Could not open file to export to."));
        return;
	}

	QTextStream text(&file);
	text.setCodec("UTF-8");
	text << template_1;

	TimetableExportHelper helper(rules, solution);
	TimetableExportHelper::Table tt_years;
	helper.getYearsTimetable(tt_years);
	tt_years.computeExtraInfo();

	for (int y=0; y < rules.yearsList.size(); y++) {
		QString yearName = rules.yearsList[y]->name;

		const QStringList validHours_medio = {"7h00", "7h50", "8h40", "9h50", "10h40", "11h30", "13h00", "13h50", "14h40", "15h50", "16h40", "17h30", "18h30", "19h20", "20h30", "21h20"};
		const QStringList intervals_medio = {"9h30","9h40", "12h20", "15h30","15h40", "18h20", "20h10", "20h20"};
		const QStringList relabel_medio = {"7h00", "7h50", "8h40", "9h50", "10h40", "11h30", "13h00", "13h50", "14h40", "15h50", "16h40", "17h30", "18h30", "19h20", "20h25", "21h15", "22h05"};

		const QStringList validHours_superior = {"7h00", "7h30", "8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30", "21h00", "21h30", };
		const QStringList intervals_superior = {};
		const QStringList relabel_superior = {"7h00", "7h30", "8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30", "21h00", "21h30", "22h00"};

		const bool isSuperior = yearName.startsWith("ENPRO.") || yearName.startsWith("FISIC.") || yearName.startsWith("BAFIS.") || yearName.startsWith("MFIS.");

		const QStringList &validHours = isSuperior? validHours_superior : validHours_medio;
		const QStringList &intervals = isSuperior? intervals_superior : intervals_medio;
		const QStringList &relabel = isSuperior? relabel_superior : relabel_medio;

		HourFilter filter(validHours, intervals, relabel);
		int whatShow = SUBJECT|TEACHERS|STUDENTS_ONLY_IF_DIFFERENT|ROOM|TIME_ROW_LABEL;
		writeTable(text, filter, tt_years, y, yearName, isSuperior? "row_hora_normal_superior" : "row_hora_normal", "pm1", whatShow);
	}

//	TimetableExportHelper::Table tt_teachers;
//	helper.getTeachersTimetable(tt_teachers);
//	tt_teachers.computeExtraInfo();
//	for (int y=0; y < rules.teachersList.size(); y++) {
//		QString teacherName = rules.teachersList[y]->name;

//		QStringList validHours;// = {"7h00", "7h50", "8h40", "9h50", "10h40", "11h30", "13h00", "13h50", "14h40", "15h50", "16h40", "17h30", "18h30", "19h20", "20h30", "21h20"};
//		for (int h=0; h<rules.nHoursPerDay; h++)
//			validHours << rules.hoursOfTheDay[h];
//		const QStringList intervals;// = {"9h30","9h40", "12h20", "15h30","15h40", "18h20", "20h10", "20h20"};
//		const QStringList &relabel = validHours;//{"7h00", "7h50", "8h40", "9h50", "10h40", "11h30", "13h00", "13h50", "14h40", "15h50", "16h40", "17h30", "18h30", "19h20", "20h25", "21h15", "22h05"};
//		validHours.pop_back();

//		int whatShow = SUBJECT|STUDENTS_ONLY_IF_DIFFERENT|ROOM;
//		HourFilter filter(validHours, intervals, relabel);
//		writeTable(text, filter, tt_teachers, y, teacherName, "row_hora_normal_professor", "pm2", whatShow);
//	}

	text << template_2;
	file.close();
}

QString FOdsExportForm::getActivityText(const Activity *act, int flags, int tblIdx)
{
	QString text;
	if (flags & ActivityFlags::SUBJECT)
		text += text_par_tag.arg(act->subjectName);
	if (flags & ActivityFlags::TEACHERS_ONLY_IF_DIFFERENT) {
		if (!act->teachersNames.contains(rules.teachersList[tblIdx]->name))
			text += text_par_tag.arg(text_span_tag.arg("T1").arg(act->teachersNames.join(", ")));
	} else if (flags & ActivityFlags::TEACHERS) {
		text += text_par_tag.arg(text_span_tag.arg("T1").arg(act->teachersNames.join(", ")));
	}
	if (flags & ActivityFlags::STUDENTS_ONLY_IF_DIFFERENT) {
		if (!act->studentsNames.contains(rules.yearsList[tblIdx]->name))
			text += text_par_tag.arg(text_span_tag.arg("T1").arg(act->studentsNames.join(", ")));
	} else if (flags & ActivityFlags::STUDENTS) {
		text += text_par_tag.arg(text_span_tag.arg("T1").arg(act->studentsNames.join(", ")));
	}
	if (flags & ActivityFlags::ACTIVITY_TAG) {
		QStringList printableActivityTagNames;
		foreach (QString name, act->activityTagsNames) {
			int tagId = rules.searchActivityTag(name);
			if (tagId < 0)
				continue;
			const ActivityTag *tag = rules.activityTagsList[tagId];
			if (tag->printable)
				printableActivityTagNames << name;
		}
		text += text_par_tag.arg(text_span_tag.arg("T1").arg(printableActivityTagNames.join(", ")));
	}
	if (flags & ActivityFlags::ROOM) {
		TimetableExportHelper helper(rules, solution);
		const Room *room = helper.getRoom(act);
		if (room != NULL)
			text += text_par_tag.arg(text_span_tag.arg("T1").arg(room->name));
	}
	return text;
}

QString FOdsExportForm::getEndHourLabel(int current_hour, const HourFilter& filter) const
{
	for (int hi = current_hour; hi < rules.nHoursPerDay; hi++) {
		const QString &hourLabel = rules.hoursOfTheDay[hi];
		if (filter.breakTimes.contains(hourLabel)) {
			return hourLabel; // relabel?
		}
		if (filter.validHours.contains(hourLabel)) {
			int idx = filter.validHours.indexOf(hourLabel);
			return filter.relabel[idx];
		}
	}
	return filter.relabel.last();
}

int FOdsExportForm::getEndHour(int current_hour, int timeSpan, const HourFilter& filter) const
{
	int endHour = current_hour+1;
	for (int remaining=timeSpan-1; endHour<rules.nHoursPerDay && remaining>0; endHour++, remaining--) {
		if (filter.breakTimes.contains(rules.hoursOfTheDay[endHour]))
			break;
	}
	return endHour;
}

int FOdsExportForm::getRowSpan(int start_hour, int end_hour, const QList<int>& validHoursId) const
{
	int rowspan = 0;
	for (int h = start_hour; h < end_hour; h++)
		if (validHoursId.contains(h))
			rowspan++;
	return rowspan;
}

void FOdsExportForm::writeTable(QTextStream &text, const HourFilter &filter, const TimetableExportHelper::Table &table, const int tblIdx, const QString &tableName, const char *data_row_style, const char *table_style, int whatShow)
{
	QList<int> validHoursId;
	foreach (QString hour, filter.validHours) {
		for (int i = 0; i < rules.nHoursPerDay; i++) {
			if (rules.hoursOfTheDay[i] == hour) {
				validHoursId << i;
				break;
			}
		}
	}
	qSort(validHoursId);

	text << QString(table_open_tag).arg(tableName).arg(table_style);
	text << table_column_tag.arg("col_hora").arg("Hora");
	int required_columns = 0;
	for(int d=0; d<rules.nDaysPerWeek; d++) {
		const char *default_cell_style = (d < rules.nDaysPerWeek - 1) ? "Conteúdo" : "Conteúdo";//"Conteúdo_20_fim";
		const int columnsToday = table.maxSimultaneousActivitiesPerDay(tblIdx,d) > 1 ? table.maxSimultaneousActivitiesPerDay(tblIdx,d) : 1;
		const char *col_style = columnsToday <= 1 ? "col_dia_normal" : "col_dia_div_2";

		required_columns += columnsToday;

		for (int idd = 0; idd < columnsToday; idd++)
			text << table_column_tag.arg(col_style).arg(default_cell_style);
	}
	text << table_column_tag.arg("col_fim_dia").arg("Coluna_20_fim");

	text << row_doc_header.arg(required_columns+1).arg(tableName).arg(required_columns+1-2);

	text << row_open_tag.arg("row_dias_semana");
	text << cell_styled_open_tag.arg("Dia_20_início");
	text << cell_close_tag;
	for (int d=0; d < rules.nDaysPerWeek; d++) {
		const char *cell_style = d < rules.nDaysPerWeek-1 ? "Dia_20_meio" : "Dia_20_fim";
		if(table.maxSimultaneousActivitiesPerDay(tblIdx,d) > 1) {
			text << cell_styled_span_open_tag.arg(cell_style).arg(table.maxSimultaneousActivitiesPerDay(tblIdx,d)).arg(1);
			text << text_par_tag.arg(rules.daysOfTheWeek[d]);
			text << cell_close_tag;

			text << cell_empty_covered_spanned_tag.arg(cell_style).arg(table.maxSimultaneousActivitiesPerDay(tblIdx,d)-1);
		} else {
			text << cell_styled_open_tag.arg(cell_style);
			text << text_par_tag.arg(rules.daysOfTheWeek[d]);
			text << cell_close_tag;
		}
	}
	text << row_close_tag;

	int h_filtered = 0;
	for (int h=0; h < rules.nHoursPerDay; h++) {
		QString hourLabel = rules.hoursOfTheDay[h];
		if (!filter.validHours.contains(hourLabel) && !filter.breakTimes.contains(hourLabel))
			continue;

		bool isBreakTime = filter.breakTimes.contains(hourLabel);
		bool wasBreakTime = h > 0 && filter.breakTimes.contains(rules.hoursOfTheDay[h-1]);
		if (isBreakTime && wasBreakTime)
			continue;
		const char *row_style = !isBreakTime ? data_row_style : "row_intervalo";
		text << row_open_tag.arg(row_style);
		if (isBreakTime) {
			text << cell_default_span_open_tag.arg(required_columns+1).arg(1);
			text << cell_close_tag;
		} else {
			QString endHourLabel = getEndHourLabel(h+1, filter);
			if (whatShow & TIME_ROW_LABEL) {
				QString hourRowLabel = timespan_row_label_format.arg(filter.relabel[h_filtered]).arg(endHourLabel);
				text << cell_default_open_tag << text_par_tag.arg(hourRowLabel) << cell_close_tag;
			} else {
				text << cell_empty_tag;
			}

			for (int d = 0; d < rules.nDaysPerWeek; d++) {
				const int nActivitiesNow = table.data[tblIdx][d][h].activities.count();
				const int nColumnsToday = table.maxSimultaneousActivitiesPerDay(tblIdx,d);
				if (nActivitiesNow == 0) {
					if (nColumnsToday <= 1)
						text << cell_default_open_tag;
					else
						text << cell_default_span_open_tag.arg(nColumnsToday).arg(1);
					text << cell_close_tag;
					if (nColumnsToday > 1)
						text << cell_empty_covered_spanned_tag.arg("Conteúdo").arg(nColumnsToday-1);
				} else if (nActivitiesNow == 1) {
					const char *style = d < rules.nDaysPerWeek-1 ? "Conteúdo_20_preenchido" : "Conteúdo_20_preenchido";//"Conteúdo_20_preenchido_20_fim";
					const Activity *act = table.data[tblIdx][d][h].activities[0];
					if (h > 0 && table.data[tblIdx][d][h-1].activities.contains(act) && !wasBreakTime) {
						// covered
						text << cell_empty_covered_tag.arg(style);
					} else {
						int endHour = getEndHour(h, table.data[tblIdx][d][h].timeSpan, filter);
						int rowspan = getRowSpan(h, endHour, validHoursId);
						if (rowspan == 1 && nColumnsToday == nActivitiesNow)
							text << cell_styled_open_tag.arg(style);
						else
							text << cell_styled_span_open_tag.arg(style).arg(nColumnsToday).arg(rowspan);
						text << getActivityText(act, whatShow, tblIdx);
						text << cell_close_tag;
					}
					if (nColumnsToday != nActivitiesNow)
						text << cell_empty_covered_spanned_tag.arg(style).arg(nColumnsToday-1);
				} else {
					const char *style = d < rules.nDaysPerWeek-1 ? "Conteúdo_20_preenchido" : "Conteúdo_20_preenchido";//"Conteúdo_20_preenchido_20_fim";
					QList<const Activity*> activities = table.data[tblIdx][d][h].activities;
					struct A{
						A(const Rules &rules, const QString &yearName) :
							rules(rules), yearName(yearName)
						{}
						bool operator()(const Activity* a, const Activity* b) const
						{
							QList<int> a_groups;
							foreach (QString g, a->studentsNames) {
								int groupIdx = rules.searchGroup(yearName, g);
								if (groupIdx >= 0)
									a_groups << groupIdx;
							}

							QList<int> b_groups;
							foreach (QString g, b->studentsNames) {
								int groupIdx = rules.searchGroup(yearName, g);
								if (groupIdx >= 0)
									b_groups << groupIdx;
							}

							return a_groups[0] < b_groups[0];
						}
					private:
						const Rules &rules;
						const QString &yearName;
					} customLess(rules, tableName);
					std::sort(activities.begin(), activities.end(), customLess);
					foreach (const Activity * act, activities) {
						int endHour = getEndHour(h, table.data[tblIdx][d][h].timeSpan, filter);
						int rowspan = getRowSpan(h, endHour, validHoursId);
						text << cell_styled_span_open_tag.arg(style).arg(1).arg(rowspan);
						text << getActivityText(act, whatShow, tblIdx);
						text << cell_close_tag;
					}
					if (nActivitiesNow < nColumnsToday)
						text << cell_empty_covered_spanned_tag.arg(style).arg(nColumnsToday-nActivitiesNow);
				}
			}
			h_filtered ++;
		}
		text << cell_empty_tag;
		text << row_close_tag;
	}

	text << row_open_tag.arg("row_fim_dia");
	text << cell_empty_covered_spanned_tag.arg("Fim_20_Tabela").arg(required_columns+1);
	text << row_close_tag;
	text<< table_close_tag;
}

const QString FOdsExportForm::template_1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
										   "\n"
										   "<office:document xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" xmlns:ooow=\"http://openoffice.org/2004/writer\" xmlns:oooc=\"http://openoffice.org/2004/calc\" xmlns:dom=\"http://www.w3.org/2001/xml-events\" xmlns:xforms=\"http://www.w3.org/2002/xforms\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:rpt=\"http://openoffice.org/2005/report\" xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" xmlns:xhtml=\"http://www.w3.org/1999/xhtml\" xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\" xmlns:tableooo=\"http://openoffice.org/2009/table\" xmlns:drawooo=\"http://openoffice.org/2010/draw\" xmlns:calcext=\"urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0\" xmlns:loext=\"urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0\" xmlns:field=\"urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0\" xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" xmlns:css3t=\"http://www.w3.org/TR/css3-text/\" office:version=\"1.2\" office:mimetype=\"application/vnd.oasis.opendocument.spreadsheet\">\n"
										   " <office:settings>\n"
										   "  <config:config-item-set config:name=\"ooo:configuration-settings\">\n"
										   "   <config:config-item config:name=\"SyntaxStringRef\" config:type=\"short\">7</config:config-item>\n"
										   "   <config:config-item config:name=\"AllowPrintJobCancel\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"SaveVersionOnClose\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"IsKernAsianPunctuation\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"CharacterCompressionType\" config:type=\"short\">0</config:config-item>\n"
										   "   <config:config-item config:name=\"ApplyUserData\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"PrinterSetup\" config:type=\"base64Binary\">lwH+/0hQLURlc2tqZXQtMzUxMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQ1VQUzpIUC1EZXNramV0LTM1MTAAAAAAAAAAAAAAAAAWAAMAtAAAAAAAAAAEAAhSAAAEdAAASm9iRGF0YSAxCnByaW50ZXI9SFAtRGVza2pldC0zNTEwCm9yaWVudGF0aW9uPVBvcnRyYWl0CmNvcGllcz0xCmNvbGxhdGU9ZmFsc2UKbWFyZ2luZGFqdXN0bWVudD0wLDAsMCwwCmNvbG9yZGVwdGg9MjQKcHNsZXZlbD0wCnBkZmRldmljZT0xCmNvbG9yZGV2aWNlPTAKUFBEQ29udGV4RGF0YQpQYWdlU2l6ZTpBNAAAEgBDT01QQVRfRFVQTEVYX01PREUTAER1cGxleE1vZGU6OlVua25vd24=</config:config-item>\n"
										   "   <config:config-item config:name=\"PrinterName\" config:type=\"string\">HP-Deskjet-3510</config:config-item>\n"
										   "   <config:config-item-map-indexed config:name=\"ForbiddenCharacters\">\n"
										   "    <config:config-item-map-entry>\n"
										   "     <config:config-item config:name=\"Language\" config:type=\"string\">pt</config:config-item>\n"
										   "     <config:config-item config:name=\"Country\" config:type=\"string\">BR</config:config-item>\n"
										   "     <config:config-item config:name=\"Variant\" config:type=\"string\"/>\n"
										   "     <config:config-item config:name=\"BeginLine\" config:type=\"string\"/>\n"
										   "     <config:config-item config:name=\"EndLine\" config:type=\"string\"/>\n"
										   "    </config:config-item-map-entry>\n"
										   "   </config:config-item-map-indexed>\n"
										   "   <config:config-item config:name=\"AutoCalculate\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"LinkUpdateMode\" config:type=\"short\">3</config:config-item>\n"
										   "   <config:config-item config:name=\"HasColumnRowHeaders\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"LoadReadonly\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"UpdateFromTemplate\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"ShowZeroValues\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"GridColor\" config:type=\"long\">12632256</config:config-item>\n"
										   "   <config:config-item config:name=\"ShowPageBreaks\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"ShowGrid\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"IsOutlineSymbolsSet\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"IsDocumentShared\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"ShowNotes\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"EmbedFonts\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"HasSheetTabs\" config:type=\"boolean\">true</config:config-item>\n"
										   "   <config:config-item config:name=\"RasterSubdivisionY\" config:type=\"int\">1</config:config-item>\n"
										   "   <config:config-item config:name=\"RasterIsVisible\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"RasterResolutionX\" config:type=\"int\">1000</config:config-item>\n"
										   "   <config:config-item config:name=\"RasterResolutionY\" config:type=\"int\">1000</config:config-item>\n"
										   "   <config:config-item config:name=\"IsSnapToRaster\" config:type=\"boolean\">false</config:config-item>\n"
										   "   <config:config-item config:name=\"RasterSubdivisionX\" config:type=\"int\">1</config:config-item>\n"
										   "   <config:config-item config:name=\"IsRasterAxisSynchronized\" config:type=\"boolean\">true</config:config-item>\n"
										   "  </config:config-item-set>\n"
										   " </office:settings>\n"
										   " <office:font-face-decls>\n"
										   "  <style:font-face style:name=\"Times New Roman\" svg:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\"/>\n"
										   "  <style:font-face style:name=\"Calibri\" svg:font-family=\"Calibri\" style:font-family-generic=\"swiss\"/>\n"
										   "  <style:font-face style:name=\"Liberation Sans\" svg:font-family=\"&apos;Liberation Sans&apos;\" style:font-family-generic=\"swiss\" style:font-pitch=\"variable\"/>\n"
										   "  <style:font-face style:name=\"DejaVu Sans\" svg:font-family=\"&apos;DejaVu Sans&apos;\" style:font-family-generic=\"system\" style:font-pitch=\"variable\"/>\n"
										   "  <style:font-face style:name=\"FreeSans\" svg:font-family=\"FreeSans\" style:font-family-generic=\"system\" style:font-pitch=\"variable\"/>\n"
										   " </office:font-face-decls>\n"
										   " <office:styles>\n"
										   "  <style:default-style style:family=\"table-cell\">\n"
										   "   <style:paragraph-properties style:tab-stop-distance=\"12.5mm\"/>\n"
										   "   <style:text-properties style:font-name=\"Liberation Sans\" fo:language=\"pt\" fo:country=\"BR\" style:font-name-asian=\"DejaVu Sans\" style:language-asian=\"zh\" style:country-asian=\"CN\" style:font-name-complex=\"DejaVu Sans\" style:language-complex=\"hi\" style:country-complex=\"IN\"/>\n"
										   "  </style:default-style>\n"
										   "  <style:default-style style:family=\"graphic\">\n"
										   "   <style:graphic-properties svg:stroke-color=\"#3465a4\" draw:fill-color=\"#729fcf\" fo:wrap-option=\"no-wrap\" draw:shadow-offset-x=\"3mm\" draw:shadow-offset-y=\"3mm\"/>\n"
										   "   <style:paragraph-properties style:text-autospace=\"ideograph-alpha\" style:punctuation-wrap=\"simple\" style:line-break=\"strict\" style:writing-mode=\"page\" style:font-independent-line-spacing=\"false\">\n"
										   "    <style:tab-stops/>\n"
										   "   </style:paragraph-properties>\n"
										   "   <style:text-properties style:use-window-font-color=\"true\" fo:font-family=\"&apos;Liberation Serif&apos;\" style:font-family-generic=\"roman\" style:font-pitch=\"variable\" fo:font-size=\"12pt\" fo:language=\"pt\" fo:country=\"BR\" style:letter-kerning=\"true\" style:font-name-asian=\"DejaVu Sans\" style:font-size-asian=\"12pt\" style:language-asian=\"zh\" style:country-asian=\"CN\" style:font-name-complex=\"DejaVu Sans\" style:font-size-complex=\"12pt\" style:language-complex=\"hi\" style:country-complex=\"IN\"/>\n"
										   "  </style:default-style>\n"
										   "  <style:style style:name=\"Default\" style:family=\"table-cell\">\n"
										   "   <style:text-properties style:font-name-complex=\"FreeSans\" style:font-family-complex=\"FreeSans\" style:font-family-generic-complex=\"system\" style:font-pitch-complex=\"variable\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Result\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n"
										   "   <style:text-properties fo:font-style=\"italic\" style:text-underline-style=\"solid\" style:text-underline-width=\"auto\" style:text-underline-color=\"font-color\" fo:font-weight=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Result2\" style:family=\"table-cell\" style:parent-style-name=\"Result\" style:data-style-name=\"N104\"/>\n"
										   "  <style:style style:name=\"Heading\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n"
										   "   <style:table-cell-properties style:text-align-source=\"fix\" style:repeat-content=\"false\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\"/>\n"
										   "   <style:text-properties fo:font-size=\"16pt\" fo:font-style=\"italic\" fo:font-weight=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Heading1\" style:family=\"table-cell\" style:parent-style-name=\"Heading\">\n"
										   "   <style:table-cell-properties style:rotation-angle=\"90\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Título_20_tabela\" style:display-name=\"Título tabela\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n"
										   "   <style:table-cell-properties style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border=\"none\" style:direction=\"ltr\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"12pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"12pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"12pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Dia_20_meio\" style:display-name=\"Dia meio\" style:family=\"table-cell\" style:parent-style-name=\"Título_20_tabela\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border-left=\"0.74pt solid #000000\" style:direction=\"ltr\" fo:border-right=\"0.74pt solid #000000\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" fo:border-top=\"1.76pt solid #000000\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"12pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"12pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"12pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Dia_20_fim\" style:display-name=\"Dia fim\" style:family=\"table-cell\" style:parent-style-name=\"Dia_20_meio\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border-left=\"0.74pt solid #000000\" style:direction=\"ltr\" fo:border-right=\"1.76pt solid #000000\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" fo:border-top=\"1.76pt solid #000000\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"12pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"12pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"12pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Dia_20_início\" style:display-name=\"Dia início\" style:family=\"table-cell\" style:parent-style-name=\"Dia_20_meio\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" fo:border-left=\"1.76pt solid #000000\" fo:border-right=\"0.74pt solid #000000\" style:rotation-align=\"none\" fo:border-top=\"1.76pt solid #000000\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Calibri\" fo:font-family=\"Calibri\" style:font-family-generic=\"swiss\" fo:font-size=\"12pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"normal\" style:font-size-asian=\"12pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"normal\" style:font-size-complex=\"12pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"normal\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Hora\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border-left=\"1.76pt solid #000000\" style:direction=\"ltr\" fo:border-right=\"0.74pt solid #000000\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" fo:border-top=\"0.74pt solid #000000\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"8pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"8pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"8pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Hora_20_fim\" style:display-name=\"Hora fim\" style:family=\"table-cell\" style:parent-style-name=\"Hora\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"1.76pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border-left=\"1.76pt solid #000000\" style:direction=\"ltr\" fo:border-right=\"0.74pt solid #000000\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" fo:border-top=\"0.74pt solid #000000\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"8pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"8pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"8pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Conteúdo\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n"
										   "   <style:table-cell-properties style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border=\"0.74pt solid #000000\" style:direction=\"ltr\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#808080\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"10pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"10pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"10pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Conteúdo_20_preenchido\" style:display-name=\"Conteúdo preenchido\" style:family=\"table-cell\" style:parent-style-name=\"Conteúdo\">\n"
										   "   <style:table-cell-properties fo:background-color=\"#dddddd\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border=\"0.74pt solid #000000\" style:direction=\"ltr\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"10pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"10pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"10pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Conteúdo_20_fim\" style:display-name=\"Conteúdo fim\" style:family=\"table-cell\" style:parent-style-name=\"Conteúdo\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"wrap\" fo:border-left=\"0.74pt solid #000000\" style:direction=\"ltr\" fo:border-right=\"1.76pt solid #000000\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" fo:border-top=\"0.74pt solid #000000\" style:vertical-align=\"middle\" loext:vertical-justify=\"auto\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#808080\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"10pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"10pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"10pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Conteúdo_20_preenchido_20_fim\" style:display-name=\"Conteúdo preenchido fim\" style:family=\"table-cell\" style:parent-style-name=\"Conteúdo_20_preenchido\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"0.74pt solid #000000\" fo:background-color=\"#dddddd\" fo:border-left=\"0.74pt solid #000000\" fo:border-right=\"2.01pt solid #000000\" fo:border-top=\"0.74pt solid #000000\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"center\" css3t:text-justify=\"auto\" fo:margin-left=\"0mm\" style:writing-mode=\"page\"/>\n"
										   "   <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:text-line-through-type=\"none\" style:font-name=\"Times New Roman\" fo:font-family=\"&apos;Times New Roman&apos;\" style:font-family-generic=\"roman\" fo:font-size=\"10pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"10pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-size-complex=\"10pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Conteúdo_20_vazio_professor\" style:display-name=\"Conteúdo vazio professor\" style:family=\"table-cell\" style:parent-style-name=\"Conteúdo\">\n"
										   "   <style:table-cell-properties fo:border-top=\"0pt solid #000000\" fo:border-bottom=\"0pt solid #000000\" />\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Coluna_20_fim\" style:display-name=\"Coluna fim\" style:family=\"table-cell\" style:parent-style-name=\"Conteúdo\">\n"
										   "   <style:table-cell-properties fo:border-left=\"2.01pt solid #000000\" />\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"Fim_20_Tabela\" style:display-name=\"Fim Tabela\" style:family=\"table-cell\" style:parent-style-name=\"Dia_20_meio\">\n"
										   "   <style:table-cell-properties fo:border-bottom=\"none\" fo:border-left=\"none\" fo:border-right=\"none\" fo:border-top=\"2.01pt solid #000000\"/>\n"
										   "  </style:style>\n"
										   " </office:styles>\n"
										   " <office:automatic-styles>\n"
										   "  <style:style style:name=\"col_hora\" style:family=\"table-column\">\n"
										   "   <style:table-column-properties fo:break-before=\"auto\" style:column-width=\"11.20mm\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"col_dia_div_2\" style:family=\"table-column\">\n"
										   "   <style:table-column-properties fo:break-before=\"auto\" style:column-width=\"26.49mm\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"col_dia_normal\" style:family=\"table-column\">\n"
										   "   <style:table-column-properties fo:break-before=\"auto\" style:column-width=\"53mm\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"col_fim_dia\" style:family=\"table-column\">\n"
										   "   <style:table-column-properties fo:break-before=\"auto\" style:column-width=\"1.01mm\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_table_header\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"8.01mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_dias_semana\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"5.29mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"true\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_hora_normal\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"11.01mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_hora_normal_superior\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"6.1mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_hora_normal_professor\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"2.0mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_intervalo\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"1.8mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"row_fim_dia\" style:family=\"table-row\">\n"
										   "   <style:table-row-properties style:row-height=\"1.01mm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"ta1\" style:family=\"table\" style:master-page-name=\"Default\">\n"
										   "   <style:table-properties table:display=\"true\" style:writing-mode=\"lr-tb\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"ta2\" style:family=\"table\" style:master-page-name=\"Professor\">\n"
										   "   <style:table-properties table:display=\"true\" style:writing-mode=\"lr-tb\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"gr1\" style:family=\"graphic\">\n"
										   "   <style:graphic-properties draw:stroke=\"none\" svg:stroke-width=\"0mm\" draw:fill=\"none\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"top\" draw:auto-grow-height=\"false\" fo:padding-top=\"1.25mm\" fo:padding-bottom=\"1.25mm\" fo:padding-left=\"2.5mm\" fo:padding-right=\"2.5mm\" fo:wrap-option=\"wrap\" draw:color-mode=\"standard\" draw:luminance=\"0%\" draw:contrast=\"0%\" draw:gamma=\"100%\" draw:red=\"0%\" draw:green=\"0%\" draw:blue=\"0%\" fo:clip=\"rect(0mm, 0mm, 0mm, 0mm)\" draw:image-opacity=\"100%\" style:mirror=\"none\" style:protect=\"size\"/>\n"
										   "  </style:style>\n"
										   "  <style:page-layout style:name=\"pm1\">\n"
										   "   <style:page-layout-properties fo:page-width=\"297mm\" fo:page-height=\"210.01mm\" style:num-format=\"1\" style:print-orientation=\"landscape\" fo:margin-top=\"4.99mm\" fo:margin-bottom=\"4.99mm\" fo:margin-left=\"4.99mm\" fo:margin-right=\"4.99mm\" style:shadow=\"none\" style:table-centering=\"both\" style:writing-mode=\"lr-tb\"/>\n"
										   "   <style:header-style>\n"
										   "    <style:header-footer-properties fo:min-height=\"4.99mm\" fo:margin-left=\"0mm\" fo:margin-right=\"0mm\" fo:margin-bottom=\"0mm\"/>\n"
										   "   </style:header-style>\n"
										   "   <style:footer-style>\n"
										   "    <style:header-footer-properties fo:min-height=\"7.5mm\" fo:margin-left=\"0mm\" fo:margin-right=\"0mm\" fo:margin-top=\"2.5mm\"/>\n"
										   "   </style:footer-style>\n"
										   "  </style:page-layout>\n"
										   "  <style:page-layout style:name=\"pm2\">\n"
										   "   <style:page-layout-properties fo:page-height=\"297mm\" fo:page-width=\"210.01mm\" style:num-format=\"1\" fo:margin-top=\"4.99mm\" fo:margin-bottom=\"4.99mm\" fo:margin-left=\"4.99mm\" fo:margin-right=\"4.99mm\" style:shadow=\"none\" style:table-centering=\"both\" style:writing-mode=\"lr-tb\"/>\n"
										   "   <style:header-style>\n"
										   "    <style:header-footer-properties fo:min-height=\"4.99mm\" fo:margin-left=\"0mm\" fo:margin-right=\"0mm\" fo:margin-bottom=\"0mm\"/>\n"
										   "   </style:header-style>\n"
										   "   <style:footer-style>\n"
										   "    <style:header-footer-properties fo:min-height=\"7.5mm\" fo:margin-left=\"0mm\" fo:margin-right=\"0mm\" fo:margin-top=\"2.5mm\"/>\n"
										   "   </style:footer-style>\n"
										   "  </style:page-layout>\n"
										   "  <style:style style:name=\"P1\" style:family=\"paragraph\">\n"
										   "   <loext:graphic-properties draw:fill=\"none\"/>\n"
										   "   <style:paragraph-properties fo:text-align=\"start\"/>\n"
										   "   <style:text-properties fo:font-size=\"18pt\"/>\n"
										   "  </style:style>\n"
										   "  <style:style style:name=\"T1\" style:family=\"text\">\n"
										   "   <style:text-properties fo:font-weight=\"normal\" style:font-weight-asian=\"normal\" style:font-weight-complex=\"normal\"/>\n"
										   "  </style:style>\n"
										   " </office:automatic-styles>\n"
										   " <office:master-styles>\n"
										   "  <style:master-page style:name=\"Default\" style:page-layout-name=\"pm1\">\n"
										   "   <style:header style:display=\"false\">\n"
										   "    <text:p><text:sheet-name>?? ?</text:sheet-name></text:p>\n"
										   "   </style:header>\n"
										   "   <style:header-left style:display=\"false\"/>\n"
										   "   <style:footer style:display=\"false\">\n"
										   "    <text:p>Página <text:page-number>1</text:page-number></text:p>\n"
										   "   </style:footer>\n"
										   "   <style:footer-left style:display=\"false\"/>\n"
										   "  </style:master-page>\n"
										   "  <style:master-page style:name=\"Professor\" style:page-layout-name=\"pm2\">\n"
										   "   <style:header style:display=\"false\">\n"
										   "    <text:p><text:sheet-name>?? ?</text:sheet-name></text:p>\n"
										   "   </style:header>\n"
										   "   <style:header-left style:display=\"false\"/>\n"
										   "   <style:footer style:display=\"false\">\n"
										   "    <text:p>Página <text:page-number>1</text:page-number></text:p>\n"
										   "   </style:footer>\n"
										   "   <style:footer-left style:display=\"false\"/>\n"
										   "  </style:master-page>\n"
										   " </office:master-styles>\n"
										   " <office:body>\n"
										   "  <office:spreadsheet>\n"
										   "   <table:calculation-settings table:automatic-find-labels=\"false\"/>\n";

const char * FOdsExportForm::template_2 = "    <table:named-expressions/>\n"
										  "  </office:spreadsheet>\n"
										  " </office:body>\n"
										  "</office:document>\n";

const char * FOdsExportForm::table_open_tag     = "   <table:table table:name=\"%1\" table:style-name=\"%2\">\n";
const char * FOdsExportForm::table_close_tag    = "   </table:table>\n";

const QString FOdsExportForm::table_column_tag = "    <table:table-column table:style-name=\"%1\" table:default-cell-style-name=\"%2\"/>\n";

const QString FOdsExportForm::row_open_tag = "    <table:table-row table:style-name=\"%1\">\n";
const char * FOdsExportForm::row_close_tag  = "    </table:table-row>\n";

const QString FOdsExportForm::row_doc_header = "    <table:table-row table:style-name=\"row_table_header\">\n"
											   "     <table:table-cell table:style-name=\"Título_20_tabela\" table:number-columns-spanned=\"%1\" table:number-rows-spanned=\"1\">\n"
											   "      <text:p>%2</text:p>\n"
											   "     </table:table-cell>\n"
											   "     <table:covered-table-cell table:number-columns-repeated=\"%3\" table:style-name=\"Default\"/>\n"
											   "     <table:covered-table-cell table:style-name=\"Default\">\n"
											   "      <draw:frame table:end-x=\"8.05mm\" table:end-y=\"6.21mm\" draw:z-index=\"0\" draw:name=\"Picture in %2\" draw:style-name=\"gr1\" svg:width=\"11.07mm\" svg:height=\"4.95mm\" svg:x=\"0mm\" svg:y=\"1.26mm\">\n"
											   "       <draw:image xlink:href=\"\">\n"
											   "        <office:binary-data>/9j/4AAQSkZJRgABAQEA3ADcAAD/2wBDAAIBAQEBAQIBAQECAgICAgQDAgICAgUEBAMEBgUG\n"
											   "         BgYFBgYGBwkIBgcJBwYGCAsICQoKCgoKBggLDAsKDAkKCgr/2wBDAQICAgICAgUDAwUKBwYH\n"
											   "         CgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgr/wAAR\n"
											   "         CAArAGADASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAA\n"
											   "         AgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkK\n"
											   "         FhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWG\n"
											   "         h4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl\n"
											   "         5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREA\n"
											   "         AgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYk\n"
											   "         NOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOE\n"
											   "         hYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk\n"
											   "         5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD9/KjmvLS3dI57qNGkbbGrOBub0Hqakr8Z\n"
											   "         f+CxH7O/x18df8FA9Q1/QNf0vWLPV7Wxg0CKTxlaRHQxDZI72063EyCwBkWWdS22KQ3KlWaS\n"
											   "         RlHj55m8clwaruPNeSVr23v117H13BfD2U8TZw8JmOYU8FBQlL2lS3K2rWiuaUFdpt/FtF6M\n"
											   "         /Zjzof8AnqvXH3qBIjcK4/Ovyl8JfAD462vxy0u3+IHjDwnqviiPxZAuuX1p460sSXl+Loed\n"
											   "         IkJmSbc8u4hPKV8nGxW+Ue2fsN/sb/tK/B79o7SfHfxC+HjaZpNva3SXV1/bFpL9+FlVdsUr\n"
											   "         MfmK9sDGa+Sy7jjMswx0KEcumoubhKV21Fae87QtbV7tLTdn5jHOsRUxzoQw7lFTcedNtWTt\n"
											   "         zfCtHur20PvCiiiv0U+gCiiigAooooAKKKKACvy5/wCCkVvaQftg+KprbUluJJoLB5oUjZTA\n"
											   "         /wBihXyyWwGO1VfK5GJAM5DAfqNX5j/8FB77wOn7X/iyPVPDOrSTp9gFxNb65EiS/wCgW5BV\n"
											   "         DasY/lIXBZ8lS2QG2L+a+KSg+HafNb+LHe9vhn/Lr+h8vxby/wBmRvb41vftLsZKz6HYf8FC\n"
											   "         ptR1vWls7Wz+LktzLcNbu4Hl6k0gTagLZZlCA4wC2TgZNfbXwG/4KDfBH9oP4hQ/DPwno3iK\n"
											   "         w1G6tpZrVtWsYVjlMY3MgaKaQhtu5huAXCkZyQD8Yalb/DrXf277nQNR8O61uvPitJbSTQ65\n"
											   "         CqiV9RKeYFNqflDndszkr8u/PzV9cfs7f8E6PBH7PfxRtPinp/xE1XU7mxt5o7e1mto4490i\n"
											   "         FCWIySArNwMc4OeMH53gqWexx9RZfGm6LrP2r1ulpa3M97XtZPzPNyJ5gsRL6uouHO+fe9tO\n"
											   "         /wA9il/wUQ+Efxy+K0ngux+EP7WK/Ce3OqW9rqmpReLpNLu7nOtaLdP9lTypIbm4FjZalEkc\n"
											   "         ysjNciN1MUspHk/wb+DHjzwx4y/tH9oj9sKS98I2UfgW587T/wBozxBeXKX1r4auYLlrqa3S\n"
											   "         xgtrG6uEtbhUlVl1Cbz552zPb2cP05+0toXgLVE0y68beO7jQ1FreWrSRWs00P2abyRNLcLG\n"
											   "         fLECbY97XQe0G8CZGDKK891D4RfsxeKYrnw3fftKaZNq2ta5ourSNb+LE8+QW9rbRQJEGuGk\n"
											   "         HmrtcTBjM/nRlpJCqMf0zHZhj6OIlTowg7bc00n8N9r3V3p6an1tbFYinNxgov1aXT/P/M8J\n"
											   "         /ZP/AGdf2jf2d9ftbTxJ+3lpPjnxReR2PhrxZoPib9obXdRSzWXS/DsF7qVrFe+f/p8WtWOu\n"
											   "         NBaiG38+LWURrm2EKW69F8Kf2Rf2hPgl8W7PxTc/tl3d/wCHNL8Zalc6hN4k+PPiHUH8OWcW\n"
											   "         s6jfraNZX7TW+rNdeHZdM0+VLySEaY1nJf2xuJpnlf12fwL8AdHiuPiDpfxutdQg0rWoZ7qL\n"
											   "         Qbcai0bXN/rCLCIbQvJl31m4hTAJV4FJDBXWjxh4J/ZatPiDH4l+I/7QWjfaI7zWhaaTd6jZ\n"
											   "         x2tuL7fFdCaP+EApJCSTHGzk71a4lkklxlmuYRj8EL6O3tEtG7X38ml3at3tH1zEKOqjf/Et\n"
											   "         v6++x8jXv7Cn7bXgO98MeHvCv7ddx4g1i28C+BtI8caV4m/bA8U2C3XimLVRf60saCzuplTU\n"
											   "         7OFbW2kje2lij84iBw5A75/+Cdf/AAUsFy80X7YOpt9l8SeDtTgH/C6/ESRX1npHg65sLvTf\n"
											   "         LNqzWn2vW2iuZblpLsXEbrPPbyS2vk3X0lrtj8AbfTfDvgPU/wBovSbebw7Ppd39o1LVLfzb\n"
											   "         5dKuJXYFyyqz+ZC4mIyY9jblXcDXuqMHQMpyCMg+tepl+JrYnm9ooq1tpJ730dn0/E6qFepU\n"
											   "         vzJfJ3PlL9kj4F/8FLPhh8SNA8RftOfGLwT4y0u4k19/E1rofiHVraHTP7QtNCuIhaW11DOb\n"
											   "         3ydUs9aRPPuIVhttRU2628QTToPq6iivSNgryv4j/sT/ALM/xa8aXnxC+IHw5a/1fUPL+2XX\n"
											   "         9sXkXmbI1jX5Y5VUYRFHAGcZPOTXqlFcuMwOBzCkqeKpRqRTvaSUlfvZp6mNbD0MTHlqwUlv\n"
											   "         ZpNfieUr+xJ+zMvxF/4WwPh03/CQDW/7XGof2zef8fnned5uzzdn+s+bbt29sY4r1aiilhcv\n"
											   "         wOB5vq1KMOZ3fLFK77uyV2FHD4fD39lBRvq7JK787GD44+GvhT4hwiHxJFe5FrJbLNp+qXFn\n"
											   "         KInkikdRJA6MAWgiJwQcLjozA8y37Lnwgh8ReH/FOnaDNb3fhvRv7H09lvpWH9neVJH9mILH\n"
											   "         I/eE7/v5A+bGQfRKKVbL8DiKntKlKLlpq0m9LW1+SCWHo1Jc0opv07Hnfh39lf4IeE/C194O\n"
											   "         8O+F7q1sdRu4Lm68vWrvzvNhn8+Jlm83zE2y5kCqwG4scfM2Ydb/AGRfgB4gh1iDU/BDlfEF\n"
											   "         xBPrHk6rdRG6kiWZVZikgOSJ5t5H+sMjF9xOa9KorP8AsnK3FRdGNkrJcqslrotPN/exfVcN\n"
											   "         ypcisvJf11f3nC61+zb8IPEOrWmtat4fupJrPSxpsarrF0kclllSbWWNZAk0J2rujkDK4GGD\n"
											   "         ZOe4jRYo1jToq4FOoroo4TC4aTlSgot72SV/UuNOnTbcUlcKKKK6DQ//2Q==\n"
											   "        </office:binary-data>\n"
											   "       </draw:image>\n"
											   "      </draw:frame>\n"
											   "     </table:covered-table-cell>\n"
											   "    </table:table-row>\n";


const char * FOdsExportForm::cell_default_open_tag = "     <table:table-cell>\n";
const QString FOdsExportForm::cell_styled_open_tag = "     <table:table-cell table:style-name=\"%1\">\n";
const QString FOdsExportForm::cell_default_span_open_tag = "     <table:table-cell table:number-columns-spanned=\"%1\" table:number-rows-spanned=\"%2\">";
const QString FOdsExportForm::cell_styled_span_open_tag = "     <table:table-cell table:style-name=\"%1\" table:number-columns-spanned=\"%2\" table:number-rows-spanned=\"%3\">\n";
const QString FOdsExportForm::cell_empty_tag = "     <table:table-cell />\n";
const QString FOdsExportForm::cell_empty_covered_tag = "     <table:covered-table-cell table:style-name=\"%1\" />\n";
const QString FOdsExportForm::cell_empty_covered_spanned_tag = "     <table:covered-table-cell table:style-name=\"%1\" table:number-columns-repeated=\"%2\" />\n";
const char * FOdsExportForm::cell_close_tag = "     </table:table-cell>\n";

const QString FOdsExportForm::text_par_tag = "      <text:p>%1</text:p>\n";
const QString FOdsExportForm::text_span_tag = "<text:span text:style-name=\"%1\">%2</text:span>";

const QString FOdsExportForm::timespan_row_label_format = "%1 - %2";

FOdsExportForm::HourFilter::HourFilter(const QStringList& validHours, const QStringList& intervals, const QStringList& relabel)
	: validHours(validHours), breakTimes(intervals), relabel(relabel)
{
}
