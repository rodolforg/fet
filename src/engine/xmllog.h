#ifndef XMLLOG_H
#define XMLLOG_H

#include <QString>

class XmlLog
{
private:
	QString text;
public:
	XmlLog();

	enum Mode {MINIMUM = 1, VERBOSE = 2} mode;

	void log(Mode mode, const QString &text);
	void verbose(const QString &text);
	void minimum(const QString &text);

	const QString& get() const;

	void clear();

	int numberOfUnrecognizedFields;
};

QString xmlEscape(const QString &str);

#endif // XMLLOG_H
