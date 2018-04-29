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
#include <QList>

class ErrorCode
{
public:
	enum Severity {
		Fatal,
		Error,
		Warning,
		Info,
		Verbose,
		NoError
	};

	ErrorCode();
	ErrorCode(Severity severity, QString message, int groupId = 0);

	Severity severity;
	QString message;
	int groupId;

	bool isError() const;

	QString getSeverityTitle() const;
	static QString getSeverityTitle(Severity severity);

	explicit operator bool() const;

	static int nextGroupId();
private:
	static int _nextGroupId;
};

struct ErrorList : public QList<ErrorCode> {
	bool hasError() const;
	bool hasFatal() const;
	/**
	 * Set the groupId of all messages with severity selected by filter
	 * except those with groupId already set, unless override is true.
	 */
	void setGroupIdForMessages(int groupId, ErrorCode::Severity filter, bool override = false);
};

#endif // ERRORCODE_H
