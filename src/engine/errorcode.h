//
//
// Description: This file is part of FET
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ERRORCODE_H
#define ERRORCODE_H

#include <QString>

class ErrorCode
{
public:
	enum Severity {
		FATAL,
		ERROR,
		WARNING,
		INFO,
		VERBOSE,
		NO_ERROR
	};

	ErrorCode();
	ErrorCode(Severity severity, QString message);

	Severity severity;
	QString message;

	bool isError() const;

	QString getSeverityTitle() const;
	static QString getSeverityTitle(Severity severity);

	explicit operator bool() const;
};

#endif // ERRORCODE_H
