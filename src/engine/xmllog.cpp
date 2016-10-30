#include "xmllog.h"

XmlLog::XmlLog() :
	numberOfUnrecognizedFields(0)
{
}

void XmlLog::log(Mode mode, const QString &text)
{
	if (mode <= this->mode)
		this->text += text;
}

void XmlLog::verbose(const QString &text)
{
	log(VERBOSE, text);
}

void XmlLog::minimum(const QString &text)
{
	log(MINIMUM, text);
}

const QString &XmlLog::get() const
{
	return text;
}

void XmlLog::clear()
{
	text.clear();
}
