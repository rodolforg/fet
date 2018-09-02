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

#ifndef FETGUISETTINGS_H
#define FETGUISETTINGS_H

extern bool USE_GUI_COLORS;

extern bool SHOW_SUBGROUPS_IN_COMBO_BOXES;
extern bool SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING;

extern bool SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES;

extern bool ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY;
extern bool ENABLE_STUDENTS_MAX_GAPS_PER_DAY;

extern bool ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS;

extern bool ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER;

extern bool checkForUpdates;

enum StudentsComboBoxStyle {
	STUDENTS_COMBO_BOXES_STYLE_SIMPLE=0,
	STUDENTS_COMBO_BOXES_STYLE_ICONS=1,
	STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED=2,
};
extern int STUDENTS_COMBO_BOXES_STYLE;

#endif // FETGUISETTINGS_H
