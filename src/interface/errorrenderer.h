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

#ifndef ERRORRENDERER_H
#define ERRORRENDERER_H

#include <QWidget>

#include "errorcode.h"

struct ErrorRenderer
{
	/**
	 * @brief Opens a message dialog for an error/warning/info message
	 * @param parent Parent for the message dialog
	 * @param erc error
	 */
	static void renderErrorCode(QWidget* parent, const ErrorCode& erc);

	/**
	 * @brief Opens a message dialog with skip button for grouped messages
	 * Messages with same Group Id can be skipped
	 * @param parent Parent for the message dialog
	 * @param erc error
	 * @return true if user wants to ignore next error from same group Id
	 */
	static bool renderSkippableErrorCode(QWidget* parent, const ErrorCode& erc);

	/**
	 * @brief Shows a message dialog for each error one at a time
	 * Messages with same Group Id can be skipped
	 * Messages with Group Id equals to -1 are always ignored
	 * @param parent Parent for the message dialog
	 * @param erc error
	 * @param severity minimum acceptable severity. The rest is unrendered/ignored.
	 * @return true if user wants to ignore next error from same group Id
	 */
	static void renderErrorList(QWidget* parent, const ErrorList& errors, ErrorCode::Severity severity = ErrorCode::Verbose);
};

#endif // ERRORRENDERER_H
