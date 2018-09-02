/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef STUDENTSCOMBOBOXHELPER_H
#define STUDENTSCOMBOBOXHELPER_H

#include <QComboBox>
#include <QString>

class Rules;

class StudentsComboBoxHelper
{
public:
	static int populateStudentsComboBox(const Rules& rules, QComboBox* studentsComboBox, const QString& selectedStudentsSet=QString(""), bool addEmptyAtBeginning=false);
};

#endif // STUDENTSCOMBOBOXHELPER_H
