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

#include "errorrenderer.h"
#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QCoreApplication>
#include "timetable_defs.h" // VERBOSE variable

void ErrorRenderer::renderErrorCode(QWidget* parent, const ErrorCode& erc)
{
	switch (erc.severity) {
	case ErrorCode::FATAL:
		QMessageBox::critical(parent, erc.getSeverityTitle(), erc.message);
		break;
	case ErrorCode::ERROR:
		LongTextMessageBox::mediumInformation(parent, erc.getSeverityTitle(), erc.message);
		break;
	case ErrorCode::WARNING:
		LongTextMessageBox::mediumInformation(parent, erc.getSeverityTitle(), erc.message);
		break;
	case ErrorCode::INFO:
		LongTextMessageBox::mediumInformation(parent, erc.getSeverityTitle(), erc.message);
		break;
	case ErrorCode::VERBOSE:
		if (VERBOSE)
			LongTextMessageBox::mediumInformation(parent, erc.getSeverityTitle(), erc.message);
		break;
	default:
		break;
	}
}

bool ErrorRenderer::renderSkippableErrorCode(QWidget* parent, const ErrorCode& erc)
{
	int skip = 1;
	switch (erc.severity) {
	case ErrorCode::FATAL:
		QMessageBox::critical(parent, erc.getSeverityTitle(), erc.message);
		return false;
	case ErrorCode::ERROR:
		skip = LongTextMessageBox::mediumConfirmation(parent, erc.getSeverityTitle(), erc.message,
													  QCoreApplication::translate("Rules", "Skip rest"),
													  QCoreApplication::translate("Rules", "See next"),
													  QString(), 1, 0 );
		break;
	case ErrorCode::WARNING:
		skip = LongTextMessageBox::mediumConfirmation(parent, erc.getSeverityTitle(), erc.message,
													  QCoreApplication::translate("Rules", "Skip rest"),
													  QCoreApplication::translate("Rules", "See next"),
													  QString(), 1, 0 );
		break;
	case ErrorCode::INFO:
		skip = LongTextMessageBox::mediumConfirmation(parent, erc.getSeverityTitle(), erc.message,
													  QCoreApplication::translate("Rules", "Skip rest"),
													  QCoreApplication::translate("Rules", "See next"),
													  QString(), 1, 0 );
		break;
	case ErrorCode::VERBOSE:
		if (VERBOSE)
			skip = LongTextMessageBox::mediumConfirmation(parent, erc.getSeverityTitle(), erc.message,
														  QCoreApplication::translate("Rules", "Skip rest"),
														  QCoreApplication::translate("Rules", "See next"),
														  QString(), 1, 0 );
		break;
	default:
		break;
	}

	return skip == 0;
}

void ErrorRenderer::renderErrorList(QWidget* parent, const ErrorList& errors)
{
	QSet<int> ignoredGroupIds;
	foreach (const ErrorCode& erc, errors) {
		if (!erc.groupId) {
			renderErrorCode(parent, erc);
		}
		else if (!ignoredGroupIds.contains(erc.groupId)) {
			bool shallIgnore = renderSkippableErrorCode(parent, erc);
			if(shallIgnore)
				ignoredGroupIds << erc.groupId;
		}
	}
}
